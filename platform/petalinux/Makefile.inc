pkg_depends=external/libedit external/boost external/tclap external/libbsd external/ncurses
pkg_depends_install=external/libedit external/boost external/ncurses external/libbsd
#pkg_depends=external/boost
#pkg_depends_install=external/boost
CPPFLAGS+=-I${pkg_include_libedit}/edit
BUTool_exe_ldflags+=-ledit -lncurses -lbsd
BUTool_lib_ldflags+=-ldl
