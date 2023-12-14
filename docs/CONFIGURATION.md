# Configuration options

It's possible to configure some settings by passing `--config` argument to the generator. All
configuration keys are defined in `bindings.cpp` section.

```bash
uniffi-bindgen-cpp path/to/definitions.udl --config path/to/uniffi.toml
```

- `cdylib_name` - override the dynamic library name linked by generated bindings, excluding `lib`
    prefix and `.dll` file extension. When using `--library` mode, defaults to library's name.
    In standalone mode this value is required, and error will be produced if its missing.

    ```toml
    # For library `libgreeter.dll`
    [bindings.cpp]
    cdylib_name = "greeter"
    ```

- `custom_types` - properties for custom type defined in UDL with `[Custom] typedef string Url;`.

    ```toml
    # Represent MyString as a C++ native `CustomString` class. The underlying type of MyString is a string.
    [bindings.cpp.custom_types.MyString]
    imports = ["CustomString.hpp"]
    type_name = "CustomString"
    into_custom = "CustomString({})"
    from_custom = "{}.to_string()"
    ```

  - `imports` (optional) - any imports required to satisfy this type.

  - `type_name` (optional) - the name to represent the type in generated bindings. Default is the
        type alias name from UDL, e.g. `Url`.

  - `into_custom` (required) - an expression to convert from the underlying type into custom type. `{}` will
        will be expanded into variable containing the underlying value. The expression is used in a
        return statement, i.e. `return <expression(value)>;`.

  - `from_custom` (required) - an expression to convert from the custom type into underlying type. `{}` will
        will be expanded into variable containing the custom value. The expression is used in a
        return statement, i.e. `return <expression(value);>`.
