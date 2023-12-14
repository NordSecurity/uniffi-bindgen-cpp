# How to submit changes

Pull requests are welcome!

# How to report issues/bugs?

Create an issue on Github, we will try to get back to you ASAP.

# Checkout the code

```
git clone https://github.com/NordSecurity/uniffi-bindgen-cpp.git
```

# Run tests

To run tests, a `cmake` installation is required. Unlike `uniffi-rs`, there is no integration with
`cargo test`.

- Build `uniffi-bindgen-cpp` executable, `libuniffi_fixtures.so` shared library and produce the bindings.

    ```
    cd cpp-tests
    mkdir build
    cd build
    cmake ..
    make
    ```

- Run tests

    ```
    make test
    ```

# Thank you
