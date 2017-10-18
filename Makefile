CXXFLAGS+=-g -O0 -std=c++03 -Wall -Werror
CPPFLAGS+=-Iexternal/include
CPPFLAGS+=-Iinclude

name=BUException
lib=BUException
ldflags=-lboost_regex
include=include/BUException
sources_cxx=$(wildcard src/BUException/*.cc)
include generic.lib.dynamic.gmk

name=BUTool_lib
lib=BUTool
ldflags=-lBUException -lreadline -ldl
include=include/BUTool
sources_cxx=$(wildcard src/tool/*.cc src/helpers/*.cc src/helpers/StatusDisplay/*.cc)
include generic.lib.dynamic.gmk

name=BUTool_exe
exe=BUTool
ldflags=-lBUTool -lBUException -lreadline
# Let BUTool find device libraries in lib/BUTool/
ldflags+=-Wl,-rpath=$(PREFIX)/lib/BUTool
sources_cxx=src/tool/BUTool.cxx
depends=BUTool_lib
include generic.executable.gmk
