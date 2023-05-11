#!/bin/sh

if [[ ! -d ../build ]]; then
   mkdir -p ../build
fi

pushd ../build

clang ../src/main.c -g -fdiagnostics-absolute-paths -o pbxproj

popd
