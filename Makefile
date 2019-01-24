
CXXFLAGS+=-g -O2 -std=c++03 -Wall -Werror
#CPPFLAGS+=-Iexternal/include -I$(DESTDIR)/$(PREFIX)/include/edit
CPPFLAGS+=-Iinclude


include platform/$(PLATFORM)/Makefile.inc

print-%:
	@echo '$*=$($*)'

include package.gmk

name=BUException
lib=BUException
include=BUException
sources_cxx=$(wildcard src/BUException/*.cc)
include generic.lib.gmk

name=BUTool_lib
lib=BUTool
ldflags=-lBUException
include=BUTool
sources_cxx=$(wildcard src/tool/*.cc src/helpers/*.cc src/helpers/StatusDisplay/*.cc src/g2quad_device/*.cc)
include generic.lib.gmk

name=BUTool_exe
exe=BUTool
ldflags=-lBUTool -lBUException
# Let BUTool find device libraries in lib/BUTool/device
ldflags+=-Wl,-rpath=$(PREFIX)/lib/BUTool/device
sources_cxx=src/tool/BUTool.cxx
depends=BUTool_lib
include generic.executable.gmk

cleaninst:
	@rm -rf ./install >& /dev/null


#pkg_name=butool


