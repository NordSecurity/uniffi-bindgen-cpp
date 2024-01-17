#### v0.2.2+v0.25.0

----

- Implement checksum verifition for the generated bindings.


#### v0.2.1+v0.25.0

----

- Fix incorrect macro invocation in object bindings.

#### v0.2.0+v0.25.0

----

- Move bindgen config under the `bindings.cpp` section in the config.
- Add virtual destructors to callback and error abstract classes.
- Implement destructors for objects.
- Wrap objects in `std::shared_ptr` instead of `std::unique_ptr`.
- Expose access to complex enum variants in the generated bindings.
- Add docstrings to the generated bindings.
- Remove assignment operators and copy constructors for objects to prevent misuse.
- Add generated trait methods (Display, Debug, Eq, Hash) from Rust in ojbects.
- Add support for custom types.

### v0.1.0+v0.25.0

----

- Initial release.
