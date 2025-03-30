#!/bin/bash

docker run --rm -u $(id -u):$(id -g) -v "$PWD":/lsfw -w /lsfw emscripten/emsdk:4.0.6 sh -c 'python3 build.py "$@"' -- "$@"
