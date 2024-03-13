#!/bin/bash
set -euxo pipefail

cargo build --release --package uniffi-bindgen-cpp
