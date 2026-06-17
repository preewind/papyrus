#!/usr/bin/env bash
set -euo pipefail

# run_tests.sh - convenience wrapper to build and run tests
# Usage: ./run_tests.sh [TEST_REGEX]

TEST_REGEX=""
if [ "$#" -ge 1 ]; then
  TEST_REGEX="$1"
fi

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

echo "[run_tests] Pattern: $TEST_REGEX"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "[run_tests] Configuring (cmake ..)"
cmake ..

echo "[run_tests] Building project (all targets)"
cmake --build .

if [ -n "$TEST_REGEX" ]; then
  echo "[run_tests] Running CTest for pattern: $TEST_REGEX"
  ctest -R "$TEST_REGEX" --output-on-failure
else
  echo "[run_tests] Running all tests"
  ctest --output-on-failure
fi

echo "[run_tests] Done"
