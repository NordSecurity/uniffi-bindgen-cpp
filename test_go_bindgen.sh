#!/bin/bash
set -euxo pipefail

mkdir -p cpp-tests/build
pushd cpp-tests/build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make uniffi_fixtures

popd

SCRIPT_DIR="${SCRIPT_DIR:-$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )}"
ROOT_DIR="$SCRIPT_DIR"

GENERATOR_DIR="$ROOT_DIR/3rd-party/uniffi-bindgen-go"
BINDINGS_DIR="$GENERATOR_DIR/binding_tests"
BINARIES_DIR="$ROOT_DIR/cpp-tests/build"

pushd $GENERATOR_DIR
cargo build
./build_bindings.sh

pushd $BINDINGS_DIR
LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}:$BINARIES_DIR" \
	CGO_LDFLAGS="-luniffi_fixtures -L$BINARIES_DIR -lm -ldl" \
	CGO_ENABLED=1 \
	go test arithmetic_test.go -v
