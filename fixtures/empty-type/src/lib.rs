pub struct Empty {}

pub fn get_empty_type() -> Empty {
    Empty {}
}

pub fn send_empty_type(_empty: Empty) {}

uniffi::include_scaffolding!("empty_type");
