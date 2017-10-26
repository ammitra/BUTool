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
== Basic Build and Install instructions
===============================================================================
After checkout out the BUTool source:
  $ ./setup.sh
  $ source env.sh
  $ make

===============================================================================
== Advanded Build and Install instructions
===============================================================================
  $ PREFIX=<path to eventual instalation directory on target system> ./setup.sh
  $ source buildenv.sh
  $ make
For installation to $PREFIX on same host:
  $ make install
For installation to another host:
  $ mkdir install
  $ make install DESTDIR=./install
  $ cp -r install/* <path to target filesystem>/$PREFIX/

===============================================================================
== Bug/issue tracking: 
===============================================================================
   http://gauss.bu.edu/redmine/projects/butool/issues
   http://gauss.bu.edu/redmine/projects/butool/issues/new


