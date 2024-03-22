#### v0.5.0+v0.25.0

----

- Core: **POTENTIALLY BREAKING** changed `timestamp` type from `time_point<system_clock>`, to `time_point<system_clock, nanoseconds>`


#### v0.4.2+v0.25.0

----

- Scaffolding: Add support for custom types
- Scaffolding: Allow multiple scaffolding implementations to exist in a compiled library/executable
- Scaffolding: Add support for errors
- Scaffolding: Add internal ref counts for object types
- Scaffolding: Add support for associated enums
- Core: Change the underlying type of `RustStream` and `RustStreamBuffer` to `char` from `uint8_t`

#### v0.4.1+v0.25.0

----

- Scaffolding: Decorate public functions with `__declspec(dllexport)` under Windows and `__attribute__((visibility("default")))` on other platforms
- Core: Make complex function arguments be passed by `const&` for non-callback functions
- Core: Write enums based on variant instead of casting to uint during conversion

#### v0.4.0+v0.25.0

----

- Add experimental C++ scaffolding generation option

#### v0.3.0+v0.25.0

----

- Dereference optional objects in the generated bindings
- **IMPORTANT**: Fix callback code generation

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
