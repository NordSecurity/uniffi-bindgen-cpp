#### v0.10.0+v0.31.0

----
- Core: Update bindgen to UniFFI v0.31.0. Minimum Rust version is now `1.87`
- Core: **BREAKING** Interfaces cross the FFI as a `uint64_t` handle instead of a `void *`
- Core: **BREAKING** Trait interfaces declared without `WithForeign` now generate an abstract
  base class `<Name>` and a concrete `<Name>Impl`
- Core: **BREAKING** Passing a Rust-implemented trait object back to Rust now clones its
  handle, so the reference count on the Rust side is one higher than before
- Core: Add methods on records and enums
- Core: Add uniffi traits (`Display`, `Debug`, `Eq`, `Hash`, `Ord`) on records, enums and errors
- Core: Enums whose variants have no fields are generated as a C++ `enum class`, which cannot
  have member functions, so their methods and uniffi traits become free functions
- Core: Add support for objects implementing external traits
- Core: Add support for custom types used as error types
- Core: Add renaming through the `uniffi.toml` file
- Core: Add `#[uniffi(default)]` without a literal, for primitives, `String`, `Bytes`,
  optionals, sequences, maps, custom types, objects with a constructor that takes no
  arguments, and records whose fields all have a default or a type safe to zero
- Core: Fix cases that aborted generation or produced invalid C++: optional fields with a
  non-null default, enum variants with unnamed fields, arguments named with C++ keywords,
  methods returning their own type, and error types passed as arguments or returned from a
  callback interface.

#### v0.9.0+v0.29.4

----
- Add support for external types

#### v0.8.1+v0.29.4

----
- Core: Fix optional compound type flattening not applying to default value assignment

#### v0.8.0+v0.29.4

----
- Core: Update bindgen to UniFFI v0.29.4

#### v0.7.4+v0.28.3

----
- Core: Expand the set of names for which `_` is appended to include few commonly used macros

#### v0.7.3+v0.28.3

----
- Core: Append `_` to field names which use c++ reserved keywords

#### v0.7.2+v0.28.3

----
- Core: Remove `_uniffi_internal` prefix from the `throw_underlying` function

#### v0.7.1+v0.28.3

----
- Core: Generate used code only in the scaffolding header

#### v0.7.0+v0.28.3

----
- Core: Update bindgen to UniFFI v0.28.3

#### v0.6.4+v0.25.0

----
- Core: Explicitly include `vector` in the generated wrapper header
- Core: Explicitly include `algorithm` in the generated wrapper header


#### v0.6.3+v0.25.0

----
- Core: Forward-declare non-flat uniffi enums #46
- Core: Fix invalid allocation_size code generation when dealing with empty records #45


#### v0.6.2+v0.25.0

----
- Core: Fix constructor argument ordering for `RustStream`
- Core: Fix topological sorting not taking into account structurally recursive types #43

#### v0.6.1+v0.25.0

----
- Core: Added header guard for internal FFI structs in scaffolding header

#### v0.6.0+v0.25.0

----
- Core: Added ability to customize enum variant naming styles
- Core: **BREAKING** Changed default enum variant naming style to `kEnumVariant`

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
