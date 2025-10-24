struct Foo {
    this: u32,
    errno: u32,
    assert: u32,
}

fn assert_fields_are_42(foo: Foo) {
    assert_eq!(42, foo.this);
    assert_eq!(42, foo.errno);
    assert_eq!(42, foo.assert);
}

uniffi::include_scaffolding!("reserved_field_name");
