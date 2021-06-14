#set BUTool.exe in path
export PATH+=:$PWD/bin/tool/

#setup LD_LIBRARY_PATHs for BUTool and any plugins
#must be source AFTER build finishes
plugin_paths=$(find $PWD/plugins/*/lib -type d | xargs)
export LD_LIBRARY_PATH+=:$PWD/lib
for path in $plugin_paths
do
    export LD_LIBRARY_PATH+=:$path
done
  
