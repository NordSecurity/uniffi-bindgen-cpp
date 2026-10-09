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
    format,
};

use anyhow::{Context, Result};
use askama::Template;
use filters::CppCodeOracle;
use serde::{Deserialize, Serialize};
use topological_sort::{DependencyLink, TopologicalSort};
use uniffi_bindgen::{interface::*, Component, ComponentInterface};

use crate::bindings::cpp::gen_cpp::filters::callback_interface_name;

#[derive(Serialize, Deserialize, Clone, Debug, Default)]
pub enum EnumStyle {
    Capitalized,
    #[default]
    Google,
}

#[derive(Clone, Deserialize, Serialize, Debug, Default)]
struct CustomTypesConfig {
    imports: Option<Vec<String>>,
    type_name: Option<String>,
    into_custom: String,
    from_custom: String,
}

impl CustomTypesConfig {
    fn lift(&self, name: &str) -> String {
        self.into_custom.replace("{}", name)
    }
    fn lower(&self, name: &str) -> String {
        self.from_custom.replace("{}", name)
    }
}

#[derive(Clone, Deserialize, Serialize, Debug, Default)]
pub(crate) struct Config {
    #[serde(default)]
    custom_types: HashMap<String, CustomTypesConfig>,
    #[serde(default)]
    enum_style: EnumStyle,
    #[serde(default)]
    rename: toml::Table,
}

pub(crate) fn apply_renames(components: &mut [Component<Config>]) {
    let renames: HashMap<String, toml::Table> = components
        .iter()
        .filter(|component| !component.config.rename.is_empty())
        .map(|component| {
            (
                component.ci.crate_name().to_string(),
                component.config.rename.clone(),
            )
        })
        .collect();

    if !renames.is_empty() {
        for component in components.iter_mut() {
            rename(&mut component.ci, &renames);
        }
    }
}

#[derive(Clone, Deserialize, Serialize, Debug, Default)]
pub(crate) struct ScaffoldingConfig {
    #[serde(default)]
    namespace: Option<String>,
    #[serde(default)]
    enum_style: EnumStyle,
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

    pub fn scaffolding_definitions(&self) -> impl Iterator<Item = FfiDefinition> + '_ {
        self.ci
            .callback_interface_definitions()
            .iter()
            .map(|cb| cb.vtable_definition())
            .chain(
                self.ci
                    .object_definitions()
                    .iter()
                    .flat_map(|o| o.vtable_definition()),
            )
            .map(Into::into)
            .chain(
                self.ci
                    .iter_ffi_function_definitions_non_async()
                    .map(Into::into),
            )
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
            .iter_local_types()
            .filter_map(|type_| {
                // Records and Enums are ordered by their field types, as those are embedded by
                // value. Only field types matter. A signature only needs a forward
                // declaration, which we always emit, so including them would make a method
                // returning its own type look like a cycle.
                //
                // Objects are ordered by the traits they implement. Those become base classes,
                // and a base class has to be complete since a forward declaration is not enough
                // for inheritance.
                match type_ {
                    Type::Record { name, .. } => {
                        self.ci.get_record_definition(name.as_str()).map(|record| {
                            (
                                name,
                                record
                                    .fields()
                                    .iter()
                                    .flat_map(Field::iter_types)
                                    .collect::<Vec<_>>(),
                            )
                        })
                    }
                    Type::Enum { name, .. } => {
                        self.ci.get_enum_definition(name.as_str()).map(|enum_| {
                            (
                                name,
                                enum_
                                    .variants()
                                    .iter()
                                    .flat_map(Variant::iter_types)
                                    .collect::<Vec<_>>(),
                            )
                        })
                    }
                    Type::Object { name, .. } => {
                        self.ci.get_object_definition(name).map(|object| {
                            (
                                name,
                                object
                                    .trait_impls()
                                    .iter()
                                    .map(|trait_impl| &trait_impl.trait_ty)
                                    .collect::<Vec<_>>(),
                            )
                        })
                    }
                    _ => None,
                }
            })
            .flat_map(|(name, types)| {
                types
                    .into_iter()
                    .filter_map(type_name)
                    .map(|field_name| DependencyLink {
                        prec: field_name,
                        succ: name,
                    })
            })
            .collect::<TopologicalSort<_>>();

        let local_types: HashMap<&str, &Type> = self
            .ci
            .iter_local_types()
            .filter_map(|type_| type_name(type_).map(|name| (name, type_)))
            .collect();

        let mut sorted: Vec<Type> = Vec::new();
        let mut emitted_names = BTreeSet::new();
        while !definition_topology.peek_all().is_empty() {
            let mut list = definition_topology.pop_all();
            list.sort();
            for name in list {
                // TopologicalSort preserves duplicate dependency links. A record or
                // rich enum can mention the same named type more than once, causing
                // that type to be returned once per link. Definitions, however,
                // must be emitted exactly once.
                if !emitted_names.insert(name) {
                    continue;
                }
                // External types are defined in their own namespace's header with their own
                // converters declared, which we `#include`. They must not enter the local
                // definition ordering, or we'd try to emit a definition we don't have.
                if let Some(type_) = local_types.get(name) {
                    sorted.push((*type_).clone());
                }
            }
        }

        if !definition_topology.is_empty() {
            panic!("Cyclic dependency detected");
        }

        let rest = types
            .filter(|t| type_name(t).map_or(true, |name| !emitted_names.contains(name)))
            .cloned()
            .collect::<BTreeSet<_>>();

        sorted.into_iter().chain(rest)
    }

    pub(crate) fn external_namespaces(&self) -> Vec<String> {
        self.ci
            .iter_external_types()
            .filter_map(|ty| self.ci.namespace_for_type(ty).ok())
            .filter(|namespace| *namespace != self.ci.namespace())
            .map(str::to_string)
            .collect::<std::collections::BTreeSet<_>>()
            .into_iter()
            .collect()
    }

    pub(crate) fn external_ffi_converters(&self) -> Vec<String> {
        self.ci
            .iter_external_types()
            .filter_map(|ty| {
                let namespace = self.ci.namespace_for_type(ty).ok()?;
                if namespace == self.ci.namespace() {
                    return None;
                }
                Some(format!(
                    "{}::uniffi::{}",
                    namespace,
                    CppCodeOracle.find(ty).ffi_converter_name()
                ))
            })
            .collect::<std::collections::BTreeSet<_>>()
            .into_iter()
            .collect()
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

    pub(crate) fn initialization_fns(&self) -> Vec<String> {
        // Local callback/trait interfaces register their own vtable init.
        let local_types = self
            .ci
            .iter_local_types()
            .map(|t| CppCodeOracle.find(t))
            .filter_map(|ct| ct.initialization_fn());

        // External foreign-implementable callback/trait interfaces' vtable is registered in
        // the defining namespace, but the consuming namespace must still call its `init()` so Rust
        // can call back into a foreign implementation passed across
        let external_types = self
            .ci
            .iter_external_types()
            .filter(|t| match t {
                Type::Object { imp, .. } => imp.has_callback_interface(),
                Type::CallbackInterface { .. } => true,
                _ => false,
            })
            .filter_map(|t| {
                let namespace = self.ci.namespace_for_type(t).ok()?;
                let canonical = CppCodeOracle.find(t).canonical_name();
                Some(format!(
                    "{}::uniffi::{}::init",
                    namespace,
                    callback_interface_name(&canonical).ok()?
                ))
            });

        local_types.chain(external_types).collect()
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

#[allow(unused)]
pub(crate) struct Scaffolding {
    pub(crate) cpp_scaffolding_source: String,
}

#[allow(unused)]
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
