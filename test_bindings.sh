#!/bin/bash
set -euxo pipefail

mkdir -p cpp-tests/build
cd cpp-tests/build
cmake ..
make
make test
