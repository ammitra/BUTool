#!/bin/sh

if [ -z "$PLATFORM" ]; then
    export PLATFORM=linux-native
fi

# Generate env.sh and buildenv.sh
make -f mk/envscripts.mk -B $@ || exit 1
if [ -z "$@" ]; then
echo "Source env.sh and run 'make' to build BUTool"
fi
