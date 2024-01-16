pub(crate) mod gen_cpp;

use std::{collections::HashMap, fs};

use anyhow::{bail, Result};
use camino::Utf8Path;
use serde::{Deserialize, Serialize};
use uniffi_bindgen::{BindingGenerator, BindingsConfig, ComponentInterface};

use self::gen_cpp::{generate_cpp_bindings, Bindings};

pub(crate) struct CppBindingGenerator {}

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

impl BindingsConfig for ConfigRoot {
    fn update_from_ci(&mut self, ci: &ComponentInterface) {
        self.bindings.cpp.update_from_ci(ci);
    }

    fn update_from_cdylib_name(&mut self, cdylib_name: &str) {
        self.bindings.cpp.update_from_cdylib_name(cdylib_name);
    }

    fn update_from_dependency_configs(&mut self, config_map: HashMap<&str, &Self>) {
        self.bindings.cpp.update_from_dependency_configs(
            config_map
                .iter()
                .map(|(key, config)| (*key, &config.bindings.cpp))
                .collect(),
        );
    }
}

impl BindingGenerator for CppBindingGenerator {
    type Config = ConfigRoot;

    fn write_bindings(
        &self,
        ci: &ComponentInterface,
        config: &Self::Config,
        out_dir: &Utf8Path,
    ) -> Result<()> {
        let Bindings {
            scaffolding_header,
            header,
            source,
            cpp_scaffolding_header,
        } = generate_cpp_bindings(&ci, &config.bindings.cpp, &config.scaffolding.cpp)?;
        let scaffolding_header_path = out_dir.join(format!("{}_scaffolding.hpp", ci.namespace()));
        let header_path = out_dir.join(format!("{}.hpp", ci.namespace()));
        let source_path = out_dir.join(format!("{}.cpp", ci.namespace()));
        let cpp_scaffolding_header_path = out_dir.join(format!("{}_cpp_scaffolding.cpp", ci.namespace()));

        fs::write(&scaffolding_header_path, scaffolding_header)?;
        fs::write(&header_path, header)?;
        fs::write(&source_path, source)?;
        fs::write(&cpp_scaffolding_header_path, cpp_scaffolding_header)?;

        Ok(())
    }

    fn check_library_path(
        &self,
        _library_path: &Utf8Path,
        cdylib_name: Option<&str>,
    ) -> Result<()> {
        if cdylib_name.is_none() {
            bail!("A path to a library file is required to generate bindings");
        }

        Ok(())
    }
}
