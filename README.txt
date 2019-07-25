===============================================================================
==BUTool 
===============================================================================
BUTool is a CLI tool used to interact with hardware built at the Boston University
Electronics Design Facility.
It makes use of plug-in libraries for interacting with different pieces of
hardware.
Instructions on how to add plug-ins can be found on the BUTool wiki
http://gauss.bu.edu/redmine/projects/butool/wiki/BUTool_Wiki


===============================================================================
== Common pre-req's
===============================================================================
boost-devel
readline-devel
ld

===============================================================================
== Build instructions (linux native, this probably means you)
===============================================================================
After checking out the BUTool source:
  $cp make/Makefile.x86 ./Makefile
  $ make
  $ source env.sh
  $ BUTool.exe

===============================================================================
== Build and Install instructions (BU zynq petalinux builds)
===============================================================================
After checking out the BUTool source:
  $ cp make/Makefile.zynq ./Makefile
  $ mount "project_path"/os/zynq_os/images/linux/rootfs.ext4 /mnt
  $ make

Then copy "env.sh", ./bin, and ./lib to /work on the zynq system. 
Afer sourcing env.sh on the zynq, you can call BUTool.exe

If you want to mount the zynq image someplace else, please modify the COMPILETIME_ROOT variable in the makefile to point to where you mounted the code

===============================================================================
== Adding plugins
===============================================================================
It is recommended to check out plugins into the plugins/ directory.
The main Makefile will call make for plugins added into the plugins/ directory.
The following variables will be passed on to plugin Makefiles:
  BUTOOL_PATH
  BUILD_MODE
  CXX
  RUNTIME_LDPATH
  COMPILETIME_ROOT
  FALLTHROUGH_FLAGS
Plugins can use these variables to build with the same settings as BUTool,
e.g. deciding whether building for linux native or for zynq petalinux

You can also specify common variables and cross-dependencies for plugins by modifying:
  plugins/Makefile

To build only BUTool, call:
  $ make self
To build only plugins, call:
  $ make plugin

If you wish to build plugins seperately from their own location, source env.sh first.

===============================================================================
== Bug/issue tracking: 
===============================================================================
   http://gauss.bu.edu/redmine/projects/butool/issues
   http://gauss.bu.edu/redmine/projects/butool/issues/new


