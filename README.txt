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
== Bug/issue tracking: 
===============================================================================
   http://gauss.bu.edu/redmine/projects/butool/issues
   http://gauss.bu.edu/redmine/projects/butool/issues/new


