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
    collections::{BTreeSet, HashMap, HashSet},
};

use anyhow::{Context, Result};
use askama::Template;
use filters::CppCodeOracle;
use serde::{Deserialize, Serialize};
use topological_sort::{DependencyLink, TopologicalSort};
use uniffi_bindgen::{
    interface::{AsType, FfiDefinition, Type, UniffiTrait},
    ComponentInterface,
};

// TemplateExpression was removed in UniFFI 0.30, using String instead
type TemplateExpression = String;

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
            .into_iter()
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
    recursive_types: RefCell<HashSet<String>>,
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

        // Detect recursive types
        let recursive_types = Self::detect_recursive_types(ci);

        Self {
            ci,
            config,
            includes: includes.into(),
            recursive_types: recursive_types.into(),
        }
    }

    // Detect recursive types by checking if a type references itself
    // directly or indirectly through its fields
    fn detect_recursive_types(ci: &ComponentInterface) -> HashSet<String> {
        let mut recursive_types = HashSet::new();
        let mut visited = HashSet::new();
        let mut path = Vec::new();

        // Check each type
        for ty in ci.iter_local_types() {
            if let Some(name) = type_name(&ty) {
                visited.clear();
                path.clear();
                if Self::has_cycle(ci, name, &mut visited, &mut path) {
                    recursive_types.insert(name.to_string());
                    // Also mark all types in the cycle
                    for type_in_cycle in &path {
                        recursive_types.insert(type_in_cycle.clone());
                    }
                }
            }
        }

        recursive_types
    }

    // Check if a type has a cycle (references itself directly or indirectly)
    fn has_cycle(
        ci: &ComponentInterface,
        type_name: &str,
        visited: &mut HashSet<String>,
        path: &mut Vec<String>,
    ) -> bool {
        // If we've seen this type in the current path, we have a cycle
        if path.contains(&type_name.to_string()) {
            return true;
        }

        // If we've already fully explored this type, no need to check again
        if visited.contains(type_name) {
            return false;
        }

        visited.insert(type_name.to_string());
        path.push(type_name.to_string());

        // Get the dependencies of this type
        let mut has_cycle = false;
        if let Some(record) = ci.get_record_definition(type_name) {
            for field in record.fields() {
                if let Some(dep_name) = Self::extract_type_name(&field.as_type()) {
                    if Self::has_cycle(ci, &dep_name, visited, path) {
                        has_cycle = true;
                        break;
                    }
                }
            }
        } else if let Some(enum_def) = ci.get_enum_definition(type_name) {
            for variant in enum_def.variants() {
                for field in variant.fields() {
                    if let Some(dep_name) = Self::extract_type_name(&field.as_type()) {
                        if Self::has_cycle(ci, &dep_name, visited, path) {
                            has_cycle = true;
                            break;
                        }
                    }
                }
            }
        }

        path.pop();
        has_cycle
    }

    // Extract the type name from a Type, looking through Optional, Sequence, Map, etc.
    fn extract_type_name(ty: &Type) -> Option<String> {
        match ty {
            Type::Record { name, .. } | Type::Enum { name, .. } | Type::Object { name, .. } => {
                Some(name.clone())
            }
            Type::Optional { inner_type } | Type::Sequence { inner_type } => {
                Self::extract_type_name(inner_type)
            }
            Type::Map { value_type, .. } => Self::extract_type_name(value_type),
            _ => None,
        }
    }

    // Check if a type is recursive (part of a cycle)
    pub(crate) fn is_recursive_type(&self, type_name: &str) -> bool {
        self.recursive_types.borrow().contains(type_name)
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
            // For now, just add remaining types without sorting
            // This handles cases where cyclic dependencies exist
            eprintln!("Warning: Potential cyclic dependencies detected ({} types), using fallback ordering", definition_topology.len());
            // Get all remaining items from topological sort
            for name in definition_topology.into_iter() {
                if let Some(ty) = self.ci.get_type(&name) {
                    if !sorted.iter().any(|t| *t == ty) {
                        sorted.push(ty.clone());
                    }
                }
            }
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
        // Type::External was removed in UniFFI 0.30
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
        self.ci
            .iter_local_types()
            .map(|t| CppCodeOracle.find(t))
            .filter_map(|ct| ct.initialization_fn())
            .collect()
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
