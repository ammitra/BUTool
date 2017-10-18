#!/bin/sh

# Generate env.sh and useprefix.sh
# PREFIX must be set when running this script; resulting env.sh can then be
#   used to restore this value of PREFIX

make -f mk/envscripts.mk -B || exit 1
echo "Source env.sh and run 'make install' to install BUTool"
echo "Source useprefix.sh before building and using any BUTool device libraries"
