#include "tool/CLIHelper.hh"
#include "tool/Launcher.hh"

//gnu readline
#include <readline/readline.h>
#include <readline/history.h>

namespace BUTool{

  //Auto-completing state variables
  static Launcher * launcher;
  static std::string command;

  static char ** helperFunction(char const * text, int start, int end);
  static char * helperFunctionCommand(char const * text,int state);
  static char * helperFunctionSubCommand(char const * text,int state);


  std::vector<std::string> SplitString(std::string command)
  {
    std::vector<std::string> commandArgv;
    while(!command.empty())
      {
	//Find first split char
	size_t pos = command.find(' ');
	if(pos != std::string::npos)
	  {
	    //Add this string to our list
	    commandArgv.push_back(command.substr(0,pos));
	    //Remove it from the command
	    command = command.substr(pos+1,std::string::npos);
	  }
	else
	  {
	    //This is the last command, so add it to the list
	    commandArgv.push_back(command);
	    //Empty the string
	    command.clear();
	  }
      }
    //return this command
    return commandArgv;
  }

  //Set launcher state
  char ** (*CLISetAutoComplete(Launcher * _launcher))(char const *, int, int)
  {
    //Load the local launcher pointer
    launcher = _launcher;
    return helperFunction;
  }


  //===============================================================================
  // Readline helper functions
  //===============================================================================

  //Helper function for calling the AutoComplete function from Launcher
  static char * helperFunctionCommand(char const * text,int state)
  {
    if(launcher != NULL){
      //Auto complete from launcher
      std::string complete = std::string(launcher->AutoCompleteCommand(text,state));
      if(complete.empty()){
	return NULL;
      }
    
      //Allocate the return string (with NULL)
      char * ret = (char *) malloc(complete.size()+1);
      if(ret == NULL){
	std::bad_alloc e;
	throw e;
      }
      //copy string
      memcpy(ret,complete.c_str(),complete.size());
      ret[complete.size()] = '\0'; //Null terminate
      return ret;
    }
    return NULL;
  }

  //Helper function for calling the AutoComplete function from Launcher
  static char * helperFunctionSubCommand(char const * text,int state)
  {
    if(launcher != NULL){
      //Auto complete from launcher
      std::string complete = launcher->AutoCompleteSubCommand(SplitString(rl_line_buffer),text,state);
      if(complete.empty()){
	return NULL;
      }

      //Allocate the return string (with NULL)
      char * ret = (char *) malloc(complete.size()+1);
      if(ret == NULL){
	std::bad_alloc e;
	throw e;
      }
      //copy string
      memcpy(ret,complete.c_str(),complete.size());
      ret[complete.size()] = '\0'; //Null terminate
      return ret;
    }
    return NULL;
  }


  //helper function for calling... our helper functions
  //It determines if we are auto-completing a command or a sub command 
  //and then calls the appropriate matching functions
  static char ** helperFunction(char const * text, int start, int end)
  {
    (void) end; // makes compiler not complain about unused arg

    //Check if we are at the beginning of the line
    if(start == 0){

      //Reset state
      command.clear();

      //Find matches
      return  rl_completion_matches(text,helperFunctionCommand);
    } 

    //Determine if the command has been completed
    command.assign(rl_line_buffer);
    size_t commandEndPosition = command.find(' ');
    if(commandEndPosition == std::string::npos){
      command.clear();
    } else {
      command=command.substr(0,commandEndPosition);
    }

    //Match subcommands
    if (command.size() > 0){
      return rl_completion_matches(text,helperFunctionSubCommand);
    }
    //auto complete filenames
    return NULL;
  }



}
