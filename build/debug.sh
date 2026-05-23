#!/usr/bin/env bash

set -e

cmake -S . -B build/build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/build-debug