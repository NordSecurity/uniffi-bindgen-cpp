#!/bin/bash
set -euxo pipefail

mkdir -p cpp-tests/build
cd cpp-tests/build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
