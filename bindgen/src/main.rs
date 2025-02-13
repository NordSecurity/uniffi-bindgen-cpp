mod bindings;

use anyhow::Context;
use camino::Utf8PathBuf;
use clap::Parser;

use bindings::cpp::CppBindingGenerator;

#[derive(Parser)]
struct Args {
    #[clap(long, short)]
    config: Option<Utf8PathBuf>,
    #[clap(long, short)]
    out_dir: Option<Utf8PathBuf>,
    #[clap(long)]
    lib_file: Option<Utf8PathBuf>,
    #[clap(long = "library", conflicts_with_all = ["config", "lib_file"], requires = "out_dir")]
    library_mode: bool,
    #[clap(long = "scaffolding")]
    scaffolding_mode: bool,
    #[clap(long = "crate")]
    crate_name: Option<String>,
    source: Utf8PathBuf,
}

fn main() {
    let args = Args::parse();

    if args.library_mode {
        let config_supplier = {
            use uniffi_bindgen::cargo_metadata::CrateConfigSupplier;
            let cmd = ::cargo_metadata::MetadataCommand::new();
            let metadata = cmd.exec().context("error running cargo metadata").unwrap();
            CrateConfigSupplier::from(metadata)
        };

        uniffi_bindgen::library_mode::generate_bindings(
            &args.source,
            args.crate_name,
            &CppBindingGenerator {
                scaffolding_mode: args.scaffolding_mode,
            },
            &config_supplier,
            args.config.as_deref(),
            &args.out_dir.unwrap(),
            false,
        )
        .context("Failed to generate bindings using library mode")
        .unwrap();
    } else {
        uniffi_bindgen::generate_external_bindings(
            &CppBindingGenerator {
                scaffolding_mode: args.scaffolding_mode,
            },
            args.source,
            args.config.as_deref(),
            args.out_dir,
            args.lib_file,
            args.crate_name.as_deref(),
            false,
        )
        .context("Failed to generate external bindings")
        .unwrap();
    }
}
