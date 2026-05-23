#!/usr/bin/env bash

set -e

cmake -S .. -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release

./build-release/test