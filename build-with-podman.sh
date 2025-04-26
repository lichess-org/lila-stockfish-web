#!/bin/bash

VERSION=$(python3 $(dirname "${BASH_SOURCE[0]}")/build.py --emcc-version)
podman run --rm -v "$PWD":/lsfw -w /lsfw "docker.io/emscripten/emsdk:$VERSION" sh -c 'python3 build.py "$@"' -- "$@"