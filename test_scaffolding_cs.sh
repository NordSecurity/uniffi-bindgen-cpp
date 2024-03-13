#!/bin/bash
set -euxo pipefail

SCRIPT_DIR="${SCRIPT_DIR:-$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )}"
ROOT_DIR="$SCRIPT_DIR"

GENERATOR_DIR="$ROOT_DIR/3rd-party/uniffi-bindgen-cs"
BINDINGS_DIR="$GENERATOR_DIR/binding_tests"
BINARIES_DIR="$ROOT_DIR/cpp-tests/build"

pushd $GENERATOR_DIR
./build.sh
./generate_bindings.sh

SOLUTION_DIR="dotnet-tests"

export LD_LIBRARY_PATH="$SCRIPT_DIR/target/debug/:${LD_LIBRARY_PATH:-}"
cd $SOLUTION_DIR
dotnet test --filter \
    "FullyQualifiedName!~Docstring \
    & FullyQualifiedName!~Numeric \
    & FullyQualifiedName!~Stringify \
    & FullyQualifiedName!~Disposable \
    & FullyQualifiedName!~TestTraitMethods \
    & FullyQualifiedName!~OptionalParameterTests \
    & FullyQualifiedName!~TestCustomTypes \
    & FullyQualifiedName!~TestGlobalMethodsClassName \
    & FullyQualifiedName!~DateTimeMinMax \
    & FullyQualifiedName!~ChronologicalWorks \
    & FullyQualifiedName!~TimespanMax" \
    -l "console;verbosity=normal"
