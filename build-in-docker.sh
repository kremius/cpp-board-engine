#!/bin/bash

set -euo pipefail

# Build
cd /app
mkdir build
cd build
cmake ..
cmake --build .

# Tests
./cpp-board-engine-tests

# TODO: coverage
