#!/bin/bash
# Quick build and test script for Linux/macOS
# Updated for new folder structure: code/, gtests/, CMake/

set -e

echo "=== Creating build directory ==="
mkdir -p build
cd build

echo "=== Running CMake ==="
cmake ../CMake

echo "=== Building main and tests ==="
cmake --build .

echo "=== Running Google Tests ==="
./gtests_build/test_main

echo "=== All tests passed! ==="
