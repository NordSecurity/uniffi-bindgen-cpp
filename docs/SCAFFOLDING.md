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
- All arguments are passed value by default, to pass an argument by reference the `[ByRef]` tag has to be used.

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

- Associated enums
- Associated errors
- Returning errors from functions
- Custom types
- Async
- Default argument values
