pub(crate) mod gen_cpp;

use std::fs;

use anyhow::Result;
use serde::{Deserialize, Serialize};
use uniffi_bindgen::{BindingGenerator, Component, GenerationSettings};

use self::gen_cpp::{generate_cpp_bindings, generate_cpp_scaffolding, Bindings, Scaffolding};

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
        settings: &GenerationSettings,
        components: &mut Vec<uniffi_bindgen::Component<Self::Config>>,
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
