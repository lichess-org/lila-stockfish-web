#!/bin/bash

VERSION=$(python3 $(dirname "${BASH_SOURCE[0]}")/build.py --emcc)
docker run --rm -u $(id -u):$(id -g) -v "$PWD":/lsfw -w /lsfw "emscripten/emsdk:$VERSION" sh -c 'python3 build.py "$@"' -- "$@"
