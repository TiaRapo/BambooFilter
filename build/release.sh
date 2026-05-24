#!/usr/bin/env bash

set -e

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <test_name>"
    exit 1
fi

cmake -S .. -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release


./build-release/"$1"