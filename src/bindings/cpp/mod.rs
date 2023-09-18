pub(crate) mod gen_cpp;

use std::fs;

use anyhow::Result;
use camino::Utf8Path;
use uniffi_bindgen::{BindingGenerator, ComponentInterface};

use self::gen_cpp::{Bindings, Config, generate_cpp_bindings};

pub(crate) struct CppBindingGenerator {}

impl BindingGenerator for CppBindingGenerator {
    type Config = Config;

    fn write_bindings(
        &self,
        ci: ComponentInterface,
        config: Self::Config,
        out_dir: &Utf8Path,
    ) -> Result<()> {
        let Bindings {
            scaffolding_header, header, source
        } = generate_cpp_bindings(&ci, config)?;
        let scaffolding_header_path = out_dir.join(format!("{}_scaffolding.hpp", ci.namespace()));
        let header_path = out_dir.join(format!("{}.hpp", ci.namespace()));
        let source_path = out_dir.join(format!("{}.cpp", ci.namespace()));

        fs::write(&scaffolding_header_path, scaffolding_header)?;
        fs::write(&header_path, header)?;
        fs::write(&source_path, source)?;

        Ok(())
    }
}
