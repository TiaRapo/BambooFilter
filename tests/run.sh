#!/usr/bin/env bash

set -euo pipefail

usage() {
    echo "Usage:"
    echo "  $0 file <PATH <K-MER> <COUNT>"
    echo "  $0 random <COUNT>"
}

if [ "$#" -lt 1 ]; then
    usage
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
make -C "$BUILD_DIR"

case "$1" in
    file)
        if [ "$#" -ne 4 ]; then
            usage
            exit 1
        fi
        cd "$SCRIPT_DIR"
        "$BUILD_DIR/test_file" "$2" "$3" "$4"
        ;;

    random)
        if [ "$#" -ne 2 ]; then
            usage
            exit 1
        fi
        cd "$SCRIPT_DIR"
        "$BUILD_DIR/test_random" "$2"
        ;;

    *)
        usage
        exit 1
        ;;
esac