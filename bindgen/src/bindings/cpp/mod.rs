pub(crate) mod gen_cpp;

use std::{fmt::Debug, fs};

use anyhow::Result;
use serde::{Deserialize, Serialize};
use uniffi_bindgen::{
    backend::Literal, BindingGenerator, Component, ComponentInterface, GenerationSettings,
};

use self::gen_cpp::{generate_cpp_bindings, Bindings};

pub(crate) struct CppBindingGenerator {
    pub scaffolding_mode: bool,
}

#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ConfigRoot {
    #[serde(default)]
    bindings: ConfigBindings,
    #[serde(default)]
    scaffolding: ConfigScaffolding,
}

#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ConfigBindings {
    #[serde(default)]
    cpp: gen_cpp::Config,
}

#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ConfigScaffolding {
    #[serde(default)]
    cpp: gen_cpp::ScaffoldingConfig,
}

/// A Trait to help render types in a language specific format.
pub trait CodeType: Debug {
    /// The language specific label used to reference this type. This will be used in
    /// method signatures and property declarations.
    fn type_label(&self, ci: &ComponentInterface) -> String;

    /// A representation of this type label that can be used as part of another
    /// identifier. e.g. `read_foo()`, or `FooInternals`.
    ///
    /// This is especially useful when creating specialized objects or methods to deal
    /// with this type only.
    fn canonical_name(&self) -> String;

    fn literal(&self, _literal: &Literal, ci: &ComponentInterface) -> String {
        unimplemented!("Unimplemented for {}", self.type_label(ci))
    }

    /// Name of the FfiConverter
    fn ffi_converter_name(&self) -> String {
        format!("FfiConverter{}", self.canonical_name())
    }

    /// A list of imports that are needed if this type is in use.
    /// Classes are imported exactly once.
    fn imports(&self) -> Option<Vec<String>> {
        None
    }

    /// Function to run at startup
    fn initialization_fn(&self) -> Option<String> {
        None
    }
}

impl BindingGenerator for CppBindingGenerator {
    type Config = gen_cpp::Config;

    fn new_config(&self, root_toml: &toml::Value) -> Result<Self::Config> {
        Ok(match root_toml.get("bindings").and_then(|b| b.get("cpp")) {
            Some(v) => v.clone().try_into()?,
            None => Default::default(),
        })
    }

    fn update_component_configs(
        &self,
        _settings: &GenerationSettings,
        _components: &mut Vec<uniffi_bindgen::Component<Self::Config>>,
    ) -> Result<()> {
        return Ok(());
    }

    fn write_bindings(
        &self,
        settings: &GenerationSettings,
        components: &[uniffi_bindgen::Component<Self::Config>],
    ) -> Result<()> {
        for Component { ci, config, .. } in components {
            if self.scaffolding_mode {
                // let Scaffolding {
                //     cpp_scaffolding_source,
                // } = generate_cpp_scaffolding(ci, &scaffolding_config)?;
                //
                // let cpp_scaffolding_path = settings
                //     .out_dir
                //     .join(format!("{}_cpp_scaffolding.cpp", ci.namespace()));
                //
                // fs::write(&cpp_scaffolding_path, cpp_scaffolding_source)?;
            } else {
                let Bindings {
                    scaffolding_header,
                    header,
                    source,
                } = generate_cpp_bindings(&ci, &config)?;

                let scaffolding_header_path = settings
                    .out_dir
                    .join(format!("{}_scaffolding.hpp", ci.namespace()));
                let header_path = settings.out_dir.join(format!("{}.hpp", ci.namespace()));
                let source_path = settings.out_dir.join(format!("{}.cpp", ci.namespace()));

                fs::write(&scaffolding_header_path, scaffolding_header)?;
                fs::write(&header_path, header)?;
                fs::write(&source_path, source)?;
            }
        }

        Ok(())
    }
}
