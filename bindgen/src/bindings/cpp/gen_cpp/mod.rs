mod callback_interface;
mod compounds;
mod custom;
mod enum_;
mod filters;
mod miscellany;
mod object;
mod primitives;
mod record;

use std::{
    borrow::Borrow,
    cell::RefCell,
    collections::{BTreeSet, HashMap},
};

use anyhow::{Context, Result};
use askama::Template;
use serde::{Deserialize, Serialize};
use topological_sort::TopologicalSort;
use uniffi_bindgen::{
    interface::{AsType, Type},
    BindingsConfig, ComponentInterface,
};

#[derive(Clone, Deserialize, Serialize, Debug, Default)]
struct CustomTypesConfig {
    imports: Option<Vec<String>>,
}

#[derive(Clone, Deserialize, Serialize, Debug, Default)]
pub(crate) struct Config {
    cdylib_name: Option<String>,
    #[serde(default)]
    custom_types: HashMap<String, CustomTypesConfig>,
}

impl BindingsConfig for Config {
    fn update_from_ci(&mut self, ci: &ComponentInterface) {
        self.cdylib_name
            .get_or_insert_with(|| format!("uniffi_{}", ci.namespace()));
    }

    fn update_from_cdylib_name(&mut self, cdylib_name: &str) {
        self.cdylib_name
            .get_or_insert_with(|| cdylib_name.to_string());
    }

    fn update_from_dependency_configs(&mut self, _config_map: HashMap<&str, &Self>) {}
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "types.cpp")]
struct TypeRenderer<'a> {
    ci: &'a ComponentInterface,
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "scaffolding.hpp")]
struct ScaffoldingHeader<'a> {
    ci: &'a ComponentInterface,
}

impl<'a> ScaffoldingHeader<'a> {
    fn new(ci: &'a ComponentInterface) -> Self {
        Self { ci }
    }
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "wrapper.hpp")]
struct CppWrapperHeader<'a> {
    ci: &'a ComponentInterface,
    config: &'a Config,
    includes: RefCell<BTreeSet<String>>,
}

impl<'a> CppWrapperHeader<'a> {
    fn new(ci: &'a ComponentInterface, config: &'a Config) -> Self {
        Self {
            ci,
            config,
            includes: RefCell::new(BTreeSet::new()),
        }
    }

    pub(crate) fn contains_callbacks(&self, types: impl Iterator<Item = &'a Type>) -> bool {
        types
            .into_iter()
            .any(|t| matches!(t, Type::CallbackInterface { .. }))
    }

    // XXX: This is somewhat evil, but necessary.
    //      Context: C++.
    //
    //      Certain types (e.g. records or objects) may depend on other types
    //      defined within the same interface definition, yet they have to be
    //      defined so in a specific order. This here method sorts types in
    //      different ways as required by different types.
    pub(crate) fn sorted_types(
        &self,
        types: impl Iterator<Item = &'a Type>,
    ) -> impl Iterator<Item = Type> {
        let mut definition_topology = TopologicalSort::<String>::new();

        // We take into account only the Record and Enum types, as they are the
        // only types that can have member variables that reference other structures
        for type_ in self.ci.iter_types() {
            match type_ {
                Type::Record { name, .. } => {
                    if let Some(record) = self.ci.get_record_definition(name) {
                        for field in record.iter_types() {
                            match field.as_type() {
                                Type::Record {
                                    name: field_name, ..
                                }
                                | Type::Enum {
                                    name: field_name, ..
                                }
                                | Type::Object {
                                    name: field_name, ..
                                }
                                | Type::Custom {
                                    name: field_name, ..
                                } => {
                                    definition_topology.add_dependency(field_name, name);
                                }
                                _ => {}
                            }
                        }
                    }
                }
                Type::Enum { name, .. } => {
                    if let Some(enum_) = self.ci.get_enum_definition(name) {
                        enum_.variants().iter().for_each(|v| {
                            v.fields().iter().for_each(|f| match f.as_type() {
                                Type::Record {
                                    name: field_name, ..
                                }
                                | Type::Enum {
                                    name: field_name, ..
                                }
                                | Type::Object {
                                    name: field_name, ..
                                }
                                | Type::Custom {
                                    name: field_name, ..
                                } => {
                                    definition_topology.add_dependency(field_name, name);
                                }
                                _ => {}
                            });
                        });
                    }
                }
                _ => {}
            }
        }

        let mut sorted: Vec<Type> = Vec::new();
        while !definition_topology.peek_all().is_empty() {
            let list = definition_topology.pop_all();
            for name in list {
                match self.ci.get_type(&name) {
                    Some(type_) => sorted.push(type_.clone()),
                    None => {
                        panic!("Type {} not found", name)
                    }
                }
            }
        }

        if definition_topology.len() != 0 {
            panic!("Cyclic dependency detected");
        }

        let rest = types
            .cloned()
            .filter(|t| !sorted.contains(t))
            .collect::<BTreeSet<_>>();

        sorted.into_iter().chain(rest)
    }

    pub(crate) fn add_include(&self, include: &str) -> &str {
        self.includes.borrow_mut().insert(include.to_string());

        ""
    }

    pub(crate) fn includes(&self) -> Vec<String> {
        self.includes.borrow().iter().cloned().collect()
    }
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "wrapper.cpp")]
struct CppWrapper<'a> {
    ci: &'a ComponentInterface,
    config: &'a Config,
    type_helper_code: String,
}

impl<'a> CppWrapper<'a> {
    pub(crate) fn new(ci: &'a ComponentInterface, config: &'a Config) -> Self {
        Self {
            ci,
            config,
            type_helper_code: TypeRenderer { ci }.render().unwrap(),
        }
    }
}

pub(crate) struct Bindings {
    pub(crate) scaffolding_header: String,
    pub(crate) header: String,
    pub(crate) source: String,
}

pub(crate) fn generate_cpp_bindings(ci: &ComponentInterface, config: &Config) -> Result<Bindings> {
    let scaffolding_header = ScaffoldingHeader::new(ci)
        .render()
        .context("generating scaffolding header failed")?;
    let header = CppWrapperHeader::new(ci, config)
        .render()
        .context("generating C++ bindings header failed")?;
    let source = CppWrapper::new(ci, config)
        .render()
        .context("generating C++ bindings failed")?;

    Ok(Bindings {
        scaffolding_header,
        header,
        source,
    })
}
