pkg_depends=external/tclap
BUTool_exe_ldflags+=-lreadline
BUTool_lib_ldflags+=-ldl
CXXFLAGS += -DLINE_CTRL_C