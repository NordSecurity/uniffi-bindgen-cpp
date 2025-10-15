struct Foo {
    this: u32,
}

fn assert_this_is_42(foo: Foo) {
    assert_eq!(42, foo.this);
}

uniffi::include_scaffolding!("reserved_field_name");
