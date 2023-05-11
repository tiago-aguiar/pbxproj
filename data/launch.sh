#!/bin/bash

# list all devices by name
# $ xcrun simctl list | grep "iPhone 14"

xcrun simctl install {{sim_uuid}} \
      run_tree/Build/Products/Debug-iphonesimulator/{{project_name}}.app

xcrun simctl launch --console --terminate-running-process {{sim_uuid}} \
      co.tiagoaguiar.codelab.{{project_name}}

# launch lldb
# lldb -n {{project_name}} -w
