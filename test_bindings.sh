#!/bin/bash
set -euxo pipefail

mkdir -p cpp-tests/build
cd cpp-tests/build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j
CTEST_PARALLEL_LEVEL=0 CTEST_OUTPUT_ON_FAILURE=1 make test
