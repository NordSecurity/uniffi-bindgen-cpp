#!/bin/bash
set -euxo pipefail

SCRIPT_DIR="${SCRIPT_DIR:-$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )}"
ROOT_DIR="$SCRIPT_DIR"

GENERATOR_DIR="$ROOT_DIR/3rd-party/uniffi-bindgen-go"
BINDINGS_DIR="$GENERATOR_DIR/binding_tests"
BINARIES_DIR="$ROOT_DIR/cpp-tests/build"

pushd $GENERATOR_DIR
cargo build
./build_bindings.sh

pushd $BINDINGS_DIR
# We exclude tests that are not part of the main Rust fixtures
EXCLUDE_LIST="custom_types_test\
|destroy_test\
|docstring_test\
|errors_test\
|ext_types_test\
|foreign_executor_test\
|futures_test\
|name_case_test\
|objects_test\
|proc_macro_test\
|simple_fns_test\
|simple_iface_test\
|trait_methods_test\
|type_limits_test"

FILTERED_LIST=$(ls -1 *.go | grep -E -v "$EXCLUDE_LIST")

# We add the skip section, that skips a few chronological test functions
# due to C++ chrono library not supporting large timestamp values
LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}:$BINARIES_DIR" \
	CGO_LDFLAGS="-luniffi_fixtures -L$BINARIES_DIR -lm -ldl" \
	CGO_ENABLED=1 \
	go test $FILTERED_LIST \
	-skip "TestTimestampMinMax|TestPreEpochTimestampsSerializesCorrectly" \
	-v
