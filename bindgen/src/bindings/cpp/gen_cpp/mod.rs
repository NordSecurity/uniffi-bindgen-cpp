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
use topological_sort::{DependencyLink, TopologicalSort};
use uniffi_bindgen::{
    backend::TemplateExpression,
    interface::{AsType, FfiType, Type, UniffiTrait},
    BindingsConfig, ComponentInterface,
};

#[derive(Serialize, Deserialize, Clone, Debug)]
pub enum EnumStyle {
    Capitalized,
    Google,
}

impl Default for EnumStyle {
    fn default() -> Self {
        EnumStyle::Google
    }
}

#[derive(Clone, Deserialize, Serialize, Debug, Default)]
struct CustomTypesConfig {
    imports: Option<Vec<String>>,
    type_name: Option<String>,
    into_custom: TemplateExpression,
    from_custom: TemplateExpression,
}

#[derive(Clone, Deserialize, Serialize, Debug, Default)]
pub(crate) struct Config {
    #[serde(default)]
    custom_types: HashMap<String, CustomTypesConfig>,
    #[serde(default)]
    enum_style: EnumStyle,
}

#[derive(Clone, Deserialize, Serialize, Debug, Default)]
pub(crate) struct ScaffoldingConfig {
    #[serde(default)]
    namespace: Option<String>,
    #[serde(default)]
    enum_style: EnumStyle,
}

impl BindingsConfig for Config {
    fn update_from_ci(&mut self, _ci: &ComponentInterface) {}

    fn update_from_cdylib_name(&mut self, _cdylib_name: &str) {}

    fn update_from_dependency_configs(&mut self, _config_map: HashMap<&str, &Self>) {}
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "cpp_scaffolding.cpp")]
struct CppScaffolding<'a> {
    ci: &'a ComponentInterface,
    config: &'a ScaffoldingConfig,
}

impl<'a> CppScaffolding<'a> {
    fn new(ci: &'a ComponentInterface, config: &'a ScaffoldingConfig) -> Self {
        Self { ci, config }
    }
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "internal_types.cpp")]
struct InternalTypeRenderer<'a> {
    ci: &'a ComponentInterface,
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "types.cpp")]
struct TypeRenderer<'a> {
    ci: &'a ComponentInterface,
    config: &'a Config,
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
        let includes = config.custom_types.values().fold(
            BTreeSet::new(),
            |mut acc: BTreeSet<String>, custom_type| {
                if let Some(imports) = &custom_type.imports {
                    acc.extend(imports.iter().cloned());
                }
                acc
            },
        );

        Self {
            ci,
            config,
            includes: includes.into(),
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
        let mut definition_topology = self
            .ci
            .iter_types()
            .filter_map(|type_| {
                // We take into account only the Record and Enum types, as they are the
                // only types that can have member variables that reference other structures
                match type_ {
                    Type::Record { name, .. } => self
                        .ci
                        .get_record_definition(name)
                        .map(|record| (name, record.iter_types())),
                    Type::Enum { name, .. } => self
                        .ci
                        .get_enum_definition(name)
                        .map(|enum_| (name, enum_.iter_types())),
                    _ => None,
                }
            })
            .flat_map(|(name, types)| {
                types
                    .filter_map(type_name)
                    .map(|field_name| DependencyLink {
                        prec: field_name,
                        succ: name,
                    })
            })
            .collect::<TopologicalSort<_>>();

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

    pub(crate) fn includes(&self) -> Vec<String> {
        self.includes.borrow().iter().cloned().collect()
    }
}

fn type_name(ty: &Type) -> Option<&str> {
    match ty {
        Type::Record { name, .. }
        | Type::Object { name, .. }
        | Type::Enum { name, .. }
        | Type::External { name, .. }
        | Type::Custom { name, .. } => Some(name),
        _ => None,
    }
}

#[allow(dead_code)]
#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "wrapper.cpp")]
struct CppWrapper<'a> {
    ci: &'a ComponentInterface,
    config: &'a Config,
    internal_type_helper_code: String,
    type_helper_code: String,
}

impl<'a> CppWrapper<'a> {
    pub(crate) fn new(ci: &'a ComponentInterface, config: &'a Config) -> Self {
        Self {
            ci,
            config,
            internal_type_helper_code: InternalTypeRenderer { ci }.render().unwrap(),
            type_helper_code: TypeRenderer { ci, config }.render().unwrap(),
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

pub(crate) struct Scaffolding {
    pub(crate) cpp_scaffolding_source: String,
}

pub(crate) fn generate_cpp_scaffolding(
    ci: &ComponentInterface,
    config: &ScaffoldingConfig,
) -> Result<Scaffolding> {
    let cpp_scaffolding_source = CppScaffolding::new(ci, config)
        .render()
        .context("generating C++ scaffolding source failed")?;

    Ok(Scaffolding {
        cpp_scaffolding_source,
    })
}
