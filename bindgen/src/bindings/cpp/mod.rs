pub(crate) mod gen_cpp;

use std::{fs, collections::HashMap};

use anyhow::{Result, bail};
use camino::Utf8Path;
use uniffi_bindgen::{BindingGenerator, ComponentInterface, BindingsConfig};
use serde::{Deserialize, Serialize};

use self::gen_cpp::{Bindings, generate_cpp_bindings};

pub(crate) struct CppBindingGenerator {}

#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ConfigRoot {
    #[serde(default)]
    bindings: ConfigBindings,
}

#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ConfigBindings {
    #[serde(default)]
    cpp: gen_cpp::Config,
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
            scaffolding_header, header, source
        } = generate_cpp_bindings(&ci, &config.bindings.cpp)?;
        let scaffolding_header_path = out_dir.join(format!("{}_scaffolding.hpp", ci.namespace()));
        let header_path = out_dir.join(format!("{}.hpp", ci.namespace()));
        let source_path = out_dir.join(format!("{}.cpp", ci.namespace()));

        fs::write(&scaffolding_header_path, scaffolding_header)?;
        fs::write(&header_path, header)?;
        fs::write(&source_path, source)?;

        Ok(())
    }

    fn check_library_path(&self, library_path: &Utf8Path, cdylib_name: Option<&str>) -> Result<()> {
        if cdylib_name.is_none() {
            bail!("A path to a library file is required to generate bindings");
        }

        Ok(())
    }
}
