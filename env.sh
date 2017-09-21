#!/bin/bash
if [[ $_ == $0 ]]; then  
  echo "$0 is meant to be sourced:"
  echo "  source $0"
  exit 0
fi

#tool stuff
BUTOOL_PREFIX=$PWD
BUTOOL_EXE_PATH=$BUTOOL_PREFIX/bin/tool/
BUTOOL_INCLUDE_PATH=$BUTOOL_PREFIX/include
BUTOOL_LIB_PATH=$BUTOOL_PREFIX/lib

PATH="${BUTOOL_EXE_PATH}:${PATH}"
LD_LIBRARY_PATH="${BUTOOL_PREFIX}/lib/:${LD_LIBRARY_PATH}"



export LD_LIBRARY_PATH PATH BUTOOL_INCLUDE_PATH BUTOOL_LIB_PATH
