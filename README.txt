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
== Build instructions (linux native, this probably means you)
===============================================================================
After checking out the BUTool source:
  $ ./setup.sh
  $ source env.sh
  $ make

===============================================================================
== Build and Install instructions (BU zynq petalinux builds)
===============================================================================
After checking out the BUTool source:
  $ PLATFORM=petalinux ./setup.sh
  $ source env.sh
  $ make
  $ make install

You may need to update env.sh to set the working prefix for current zynq projects
You will have to change env.sh's MAKEFLAGS and PROJECT_ROOT directories for your checkout
  $ source env.sh 
  $ make
  $ make install
This will build the directory structure for the zynq in ./install/
You should copy its contents to "/" on the zynq

===============================================================================
== Bug/issue tracking: 
===============================================================================
   http://gauss.bu.edu/redmine/projects/butool/issues
   http://gauss.bu.edu/redmine/projects/butool/issues/new


