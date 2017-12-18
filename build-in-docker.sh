#!/bin/bash

set -euo pipefail

# Build
cd /app
mkdir build
cd build
cmake ..
cmake --build .

# TODO: tests
# TODO: coverage
