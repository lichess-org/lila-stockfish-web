#!/bin/bash

docker run --rm -u $(id -u):$(id -g) -v "$PWD":/lsfw -w /lsfw emscripten/emsdk:3.1.59 sh -c 'python3 build.py "$@"' -- "$@"