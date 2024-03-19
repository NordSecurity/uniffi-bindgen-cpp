# C++ Scaffolding

## Prerequisites

To be able to generate the C++ scaffolding, only a `.udl` file is needed, to learn more about how the UDL syntax works, refer to the [official documentation](https://mozilla.github.io/uniffi-rs/Overview.html) from Mozilla.

## Scaffolding generation

To generate the C++ scaffolding you can run:

```bash
uniffi-bindgen-cpp --scaffolding your_definitions.udl --out-dir output_directory
```

And it will generate scaffolding in the provided location.

### Limitations

Currently, a `Cargo.toml` file has to be present in the parent directory of the UDL file that contains the following:
```
[package]
name = "name-of-your-package"
```

The name provided in the `[package]` section, will be used as the namespace for the generated C++ and bindings code.

Once the scaffolding is generated, the following file will be created:

- `{namespace}_cpp_scaffolding.cpp`

This file should be included once in the C++ library, and it will contain all of the necessary code to interface with foreign uniffi generated bindings.

## Scaffolding integration

To ensure that the generated code is able to interface with the target C++ library, the following guidelines should be followed:

- The library should be compiled with C++20 or later.
- When exposing a struct or class, all of the members mentioned in the UDL file should be public.
- For object types, all constructors mentioned in the UDL file should have a matching public constructor in the C++ library.
- When exposing callback interfaces, it is recommended to not have any processing-intensive or global data modifying logic in the constructors and destructors of the backing C++ class, as due to the way uniffi internally handles callback interfaces, callback instances may be dynamically constructed multiple times during the runtime of the application.
- All of the exports mentioned in the UDL file should be under the same namespace that is specified in the `Cargo.toml` file in the C++ library.

### Custom types

Custom types allow the library to use types not directly exposed in the UDL file, more information can be found in the [Official documentation](https://mozilla.github.io/uniffi-rs/udl/custom_types.html).

Similar to Rust, the library code should contain a converter exposed that converts a custom type to a builtin type, the converter should follow these requirements:

- The converter should be a class or a struct named after the custom type with the prefix `UniffiCustomTypeConverter`
- The converter should contain two static public-accessible methods:
    - `custom_type into_custom(builtin_type)` for converting a builtin type into the custom type
    - `builtin_value from_custom(custom_type)` for converting a custom type into a builtin type

For example, having a custom converter for type `Handle` to `uint64_t` could look like this:
```cpp
struct Handle {
    uint64_t inner;

    // rest of the implementation ...
};

struct UniffiCustomTypeConverterHandle {
    static Handle into_custom(uint64_t val) {
        Handle handle = Handle { .inner = val } // or some other more intricate logic

        return handle;
    }

    static uint64_t from_custom(Handle handle) {
        return handle.inner; // or some other more intricate logic
    }
};
```

### Additional notes

Some of the types in the UDL file might not have a direct mapping to a C++ type. To see the full list of mappings, refer to the [UDL to C++ type mapping](#udl-to-c-type-mapping-table) table.

Additionally, for examples of how to use the C++ scaffolding, refer to the [C++ scaffolding tests](../cpp-tests/scaffolding_tests).

## UDL to C++ type mapping table

| UDL Type                       | C++ Type                                             |
|--------------------------------|------------------------------------------------------|
| `i8/i16/i32/i64`               | `int8_t/int16_t/int32_t/int64_t`                     |
| `u8/u16/u32/u64`               | `uint8_t/uint16_t/uint32_t/uint64_t`                 |
| `f32/float`                    | `float`                                              |
| `f64/double`                   | `double`                                             |
| `boolean`                      | `bool`                                               |
| `string`                       | `std::string`                                        |
| `bytes`                        | `std::vector<uint8_t>`                               |
| `timestamp`                    | `std::chrono::time_point<std::chrono::system_clock>` |
| `duration`                     | `std::chrono::duration<int64_t, std::nano>`          |
| `T?`                           | `std::optional<T>`                                   |
| `sequence<T>`                  | `std::vector<T>`                                     |
| `record<string, T>`            | `std::unordered_map<std::string, T>`                 |
| `enum`                         | `enum class`                                         |
| `dictionary`                   | `struct/class`                                       |
| `interface`                    | `std::shared_ptr<struct/class>`                      |

## Unsupported features

The following features are currently not supported in the C++ scaffolding:

- Async
- Default argument values
