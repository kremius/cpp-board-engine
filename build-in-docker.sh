#!/bin/bash

set -euo pipefail

# Build
cd /app
mkdir build
cd build
cmake .. -DBUILD_COVER=ON
cmake --build .

# Tests
./cpp-board-engine-tests

# Coverage
lcov -c -d . -o coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --remove coverage.info 'test/*' --output-file coverage.info
lcov --remove coverage.info 'build/*' --output-file coverage.info
lcov --list coverage.info
coveralls-lcov --repo-token=${COVERALLS_TOKEN} coverage.info;
