#!/usr/bin/env bash

set -e

cmake -S .. -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug

./build-debug/test