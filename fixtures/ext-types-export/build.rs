fn main() {
    uniffi::generate_scaffolding("src/ext_types_export.udl").unwrap();
}
