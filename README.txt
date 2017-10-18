=================================================================================
==BUTool
=================================================================================
BUTool is a CLI tool used to interact with hardware built at the Boston University
Electronics Design Facility.
It makes use of plug-in libraries for interacting with different pieces of
hardware.
Instructions on how to add plug-ins can be found on the BUTool wiki
http://gauss.bu.edu/redmine/projects/butool/wiki/BUTool_Wiki

=================================================================================
== Basic Build and Install instructions
=================================================================================
After checkout out the BUTool source:
  Create an an installation directory or use an existing directory
  $ PREFIX=<path to instalation directory> ./setup.sh
  $ source env.sh
  $ make install
  To add the installation directory to PATH and to set env vars for building
    BUTool plugins:
  $ source useprefix.sh

=================================================================================
== Bug/issue tracking: 
=================================================================================
   http://gauss.bu.edu/redmine/projects/butool/issues
   http://gauss.bu.edu/redmine/projects/butool/issues/new


