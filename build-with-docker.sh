#!/bin/bash

docker run --rm -u $(id -u):$(id -g) -v "$PWD":/lsfw -w /lsfw emscripten/emsdk:3.1.74 sh -c 'python3 build.py "$@"' -- "$@"
