export VIVADO_VERSION=2015.4
export VIVADO_SDK="/opt/Xilinx/SDK/"$VIVADO_VERSION"/settings64.sh"

source $VIVADO_SDK

export CXX=arm-xilinx-linux-gnueabi-g++
export CC=arm-xilinx-linux-gnueabi-gcc

export CROSS_HOST=arm-xilinx-linux-gnueabi

export MAKEOBJDIRPREFIX="$PROJECT_ROOT/obj"
export DESTDIR="$PROJECT_ROOT/install"

export BUTOOL_COMPILETIME_PATH="$DESTDIR/$PREFIX"
export BUTOOL_RUNTIME_PATH="$PREFIX"
