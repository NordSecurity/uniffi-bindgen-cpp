#!/bin/bash
set -euxo pipefail

docker run \
    -ti --rm \
    --volume $PWD:/mounted_workdir \
    --workdir /mounted_workdir \
    ghcr.io/nordsecurity/uniffi-bindgen-cpp-test-runner:v0.3.0 bash
