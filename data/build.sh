#!/bin/bash

echo "Compiling."

mkdir -p ./run_tree

xcodebuild \
    -configuration Debug \
    -scheme {{project_name}} \
    -destination "platform=iOS Simulator,id={{sim_uuid}}" \
    -sdk iphonesimulator \
    -derivedDataPath ./run_tree \
    -parallelizeTargets \
    -quiet

echo "Done."
