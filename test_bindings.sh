#!/bin/bash
set -euxo pipefail

cd cpp-tests/build
CTEST_OUTPUT_ON_FAILURE=1 make test
