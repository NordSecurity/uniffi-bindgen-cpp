mod bindings;

use camino::Utf8PathBuf;
use clap::Parser;
use uniffi_bindgen::generate_external_bindings;

use bindings::cpp::CppBindingGenerator;

#[derive(Parser)]
struct Args {
    #[clap(long, short)]
    config: Option<Utf8PathBuf>,
    #[clap(long, short)]
    out_dir: Option<Utf8PathBuf>,
    source: Utf8PathBuf,
}

fn main() {
    let args = Args::parse();

    generate_external_bindings(
        CppBindingGenerator {},
        &args.source,
        args.config,
        args.out_dir,
    )
    .unwrap();
}
