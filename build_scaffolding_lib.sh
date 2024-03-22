#!/bin/bash
set -euxo pipefail

cargo build --release --package uniffi-bindgen-cpp

mkdir -p cpp-tests/build
cd cpp-tests/build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make uniffi_fixtures
