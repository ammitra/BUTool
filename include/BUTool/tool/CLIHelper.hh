#ifndef __CLI_HELPER_HH__
#define __CLI_HELPER_HH__

#include <vector>
#include <string>

namespace BUTool{

//Forward declaration of Launcher class
class Launcher;
  
//Set launcher state for use with gnu readline's auto-complete
char ** (*CLISetAutoComplete(Launcher * _launcher))(char const *, int, int);
  
std::vector<std::string> SplitString(std::string command);
}
#endif
