# C++ Scaffolding

## Prerequisites

To generate the C++ scaffolding, only a `.udl` file is needed, to learn more about how the UDL syntax works, refer to the [official documentation](https://mozilla.github.io/uniffi-rs/Overview.html) from Mozilla.

## Scaffolding integration

To ensure that the generated code is able to interface with the target C++ library, the following guidelines should be followed:

- The library should be compiled with C++20 or later.
- When exposing a struct or class, all of the members mentioned in the UDL file should be public.
- For object types, all constructors mentioned in the UDL file should have a matching public constructor in the C++ library.
- When exposing callback interfaces, it's not recommended to have any logic in the constructors or destructors of the backing C++ class, as they will be called multiple times during the runtime of the application.
- All of the exports mentioned in the UDL file should be under the same namespace in the C++ library.

Once the scaffolding is generated, the following file will be created:

- `{namespace}_cpp_scaffolding.cpp`

This file should be included once in the C++ library, and it will contain all of the necessary code to interface the uniffi generated bindings.

Some of the types in the UDL file might not a direct mapping to a C++ type, to see the full list of mappings, refer to the [UDL to C++ type mapping](#udl-to-c-type-mapping-table) table.

Additionally, for examples of how to use the C++ scaffolding, refer to the [C++ scaffolding tests](../cpp-tests/scaffolding_tests).

## UDL to C++ type mapping table

| UDL Type | C++ Type |
|----------|----------|
| `i8/i16/i32/i64` | `int8_t/int16_t/int32_t/int64_t` |
| `u8/u16/u32/u64` | `uint8_t/uint16_t/uint32_t/uint64_t` |
| `f32/float` | `float` |
| `f64/double` | `double` |
| `boolean` | `bool` |
| `string` | `std::string` |
| `bytes` | `std::vector<uint8_t>` |
| `timestamp` | `std::chrono::time_point<std::chrono::system_clock>` |
| `duration` | `std::chrono::duration<int64_t, std::nano>` |
| `T?` | `std::optional<T>` |
| `sequence<T>` | `std::vector<T>` |
| `record<string, T>` | `std::unordered_map<std::string, T>` |
| `enum` | `enum class` |
| `dictionary` | `struct/class` |
| `interface` | `std::shared_ptr<struct/class>` |
