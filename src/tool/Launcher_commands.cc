#include "tool/Launcher.hh"
#include <boost/tokenizer.hpp>
#include <stdio.h>  

using namespace BUTool;

void Launcher::LoadCommandList()
{
  //Very special commands that are really in the command line processor. 
  AddCommand("include",&Launcher::InvalidCommandInclude,"Read a script file"); //This command exists inside of the CLI class and should never be called.  
  //InvalidCommandInclude is the least wrong thing to put here, it will never be called.
  AddCommandAlias("load","include");               //This is the only allowed alias right now and is only allowed because it is also part of the CLI class
  //DO NOT ADD ANY MORE ALIASES TO include/load UNLESS THEY ARE IN THE CLI CLASS! 
  //LISTEN TO THE LINE ABOVE! YES YOU! -Dan dgastler@bu.edu
  // general commands (Launcher_commands)
  AddCommand("help",&Launcher::Help,"List commands. 'help <command>' for additional info and usage",
	     &Launcher::autoComplete_Help);
  AddCommandAlias( "h", "help");
  AddCommand("quit",&Launcher::Quit,"Close program");
  AddCommandAlias("q", "quit");
  AddCommand("exit",&Launcher::Quit,"Close program");
  AddCommand("echo",&Launcher::Echo,
	     "Echo to screen\n" \
	     "  Usage:\n" \
	     "  echo <text>");
  AddCommand("sleep",&Launcher::Sleep,
	     "Delay in seconds \n" \
	     "  Usage:\n" \
	     "  sleep <integer number of seconds to be delayed>");
  AddCommand("add_device",&Launcher::AddDevice,
	     "Add a new device and make it active.\n"\
	     "  Usage:\n"\
	     "  add_device <type> <args>\n",
	     &Launcher::autoComplete_AddDevice);  
  AddCommand("add_lib",&Launcher::AddLib,
	     "Load a new class of device.\n"\
	     "  Usage:\n"\
	     "  add_lib <path>\n");  
  AddCommand("list",&Launcher::ListDevices,
	     "List devices currently connected\n"\
	     "  * indicated the active device\n"
	     );  
  AddCommand("select",&Launcher::SelectDevice,
	     "Switch the active device.\n"\
	     "  Usage:\n"\
	     "  select <device number>\n");  
  AddCommand("verbose",&Launcher::SetVerbosity,
	     "Change the print level for exceptions (9 == all).\n"\
	     "  Usage:\n"\
	     "  verbose <level>\n");  
}

CommandReturn::status Launcher::SetVerbosity(std::vector<std::string> strArg,std::vector<uint64_t> intArg){
  (void) strArg; // makes compiler not complain about unused parameter
  if(intArg.size() > 0){
    if(intArg[0] > 9){
      verbosity = 9;
    }else{
      verbosity = intArg[0];
    }
  }else{
    verbosity = 0;
  }
  return CommandReturn::OK;
}

CommandReturn::status Launcher::InvalidCommandInclude(std::vector<std::string>,std::vector<uint64_t>)
{
  return CommandReturn::OK;
}
  
CommandReturn::status Launcher::Quit(std::vector<std::string>,
		   std::vector<uint64_t>)
{
  //Quit CLI so return -1
  return CommandReturn::EXIT;
}
  
CommandReturn::status Launcher::Echo(std::vector<std::string> strArg,
		   std::vector<uint64_t> intArg) {
  (void) intArg; // makes compiler not complain about unused arg
  for(size_t iArg = 0; iArg < strArg.size();iArg++)
    {
      printf("%s ",strArg[iArg].c_str());
    }
  printf("\n");
  return  CommandReturn::OK;
}

void Launcher::HelpPrinter(std::string const &commandName,
			   std::vector<std::string> const & commandAliases,
			   std::string const & commandHelp,
			   bool fullPrint){
  //Build a string with the command name and aliases
  std::string nameString(commandName);

  //Append the alias names if they exist
  if(commandAliases.size()> 0){
    //We have sub commands
    nameString+='(';
    for(std::vector<std::string>::const_iterator it = commandAliases.begin();
	it != commandAliases.end();
	it++){
      nameString.append(*it);
      nameString+=',';
    }
    nameString[nameString.size()-1] = ')';
  }

  //print the help for this command
  if(!fullPrint && commandHelp.find('\n')){ 
    printf(" %-20s:   %s\n",nameString.c_str(),
	   commandHelp.substr(0,commandHelp.find('\n')).c_str());
  } else { // Print full help
    printf(" %-20s:   ",nameString.c_str());
    boost::char_separator<char> sep("\n");
    boost::tokenizer<boost::char_separator<char> > tokens(commandHelp, sep);
    boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
    if(it != tokens.end()){
      printf("%s\n",(*it).c_str());
      it++;
    }
    for ( ;it != tokens.end();++it){
      printf("                         %s\n",(*it).c_str());
    }
  }  
}
CommandReturn::status Launcher::Help(std::vector<std::string> strArg,std::vector<uint64_t> intArg) 
{
  //Check the arguments to see if we are looking for
  //  simple help:    all commands listed with 1-line help
  //  specific help:  full help for one command
  //  full help:      full help for all commands
  
  (void) intArg; // Makes compiler not complain about unused arg

  //Get a list of commands from Launcher
  std::map<std::string,std::vector<std::string> > commandList = GetCommandList();
  //Get a list of commands from the active device
  std::map<std::string,std::vector<std::string> > deviceCommandList;
  //  std::vector<DeviceContainer *>::iterator itActiveDevice = device.begin()+activeDevice;  
  std::vector<CommandListBase *>::iterator itActiveDevice = device.end();
  if(activeDevice >= 0){
    itActiveDevice = device.begin()+activeDevice;
    if(itActiveDevice != device.end()){  
      deviceCommandList = (*itActiveDevice)->GetCommandList();      
    }
  }

  bool fullPrint = false;   
  if(strArg.size() > 0){    
    if(strArg[0].size() > 0){
      if(strArg[0][0] == '*'){
	//Check if we want a full print and skip to the end if we do
	fullPrint = true;
      } else {
	//We want a per command full help
	
	//Search Launcher commands
	if(commandList.find(strArg[0]) != commandList.end()){
	  //We have a launcher command
	  HelpPrinter(strArg[0],
		      commandList[strArg[0]],
		      GetHelp(strArg[0]),
		      true);
	  return CommandReturn::OK;
	}else if(deviceCommandList.find(strArg[0]) != deviceCommandList.end()){
	  //We have a device command
	  HelpPrinter(strArg[0],
		      deviceCommandList[strArg[0]],
		      (*itActiveDevice)->GetHelp(strArg[0]),
		      true);
	  return CommandReturn::OK;
	  
	}
      }
    }	
  }
  
  //We are printing out all the helps

  //Print the Launcher helps
  for(std::map<std::string,std::vector<std::string> >::iterator it = commandList.begin();
      it != commandList.end();
      it++){
    HelpPrinter(it->first,
		it->second,
		GetHelp(it->first),
		fullPrint);
  }

  //Print the Device helps
  for(std::map<std::string,std::vector<std::string> >::iterator it = deviceCommandList.begin();
      it != deviceCommandList.end();
      it++){
    bool printDeviceCommand = true;
    //Skip help print if the device command is superseded by a Launcher command
    for(std::map<std::string,std::vector<std::string> >::iterator itPrimary = commandList.begin();
	itPrimary != commandList.end();
	itPrimary++){
      //check if the device command matches a Launcher command
      if(it->first.compare(itPrimary->first) == 0){
	printDeviceCommand= false;
	break;
      }else{
	//check if the device command matches a launcher command alias
	for(std::vector<std::string>::iterator itAlias = itPrimary->second.begin();
	    itAlias != itPrimary->second.end();
	    itAlias++){
	  if(it->first.compare(*itAlias) == 0){
	    printDeviceCommand= false;
	    itPrimary = commandList.end();
	    break;	    
	  }
	}
      }
    }
    //Print the device command help if needed
    if(printDeviceCommand){
      HelpPrinter(it->first,
		  it->second,
		  (*itActiveDevice)->GetHelp(it->first),
		  fullPrint);
    }
  }
  return CommandReturn::OK;
}


std::string Launcher::autoComplete_Help(std::vector<std::string> const & line,std::string const & currentToken ,int state)
{  
  if(line.size() > 0){
    if((line.size() > 1) && (currentToken.size() == 0)){
      return std::string("");
    }

    static std::vector<std::string> commandName;
    static size_t iCommand;

    //Reload lists if we are just starting out at state == 0
    if(state == 0){
      //Get a list of commands from Launcher
      std::map<std::string,std::vector<std::string> > commandList = GetCommandList();
      for(std::map<std::string,std::vector<std::string> >::iterator it = commandList.begin();
	  it != commandList.end();
	  it++)
      {
	//append the current command
	commandName.push_back(it->first);
	//append aliases to that command
	for(std::vector<std::string>::iterator itAlias = it->second.begin();
	    itAlias != it->second.end();
	    itAlias++){
	  commandName.push_back(*itAlias);
	}
      }

      if((activeDevice >= 0) && (size_t(activeDevice) < device.size())){
	//Get a list of commands from the active device
	std::map<std::string,std::vector<std::string> > deviceCommandList = device[activeDevice]->GetCommandList();
	for(std::map<std::string,std::vector<std::string> >::iterator it = deviceCommandList.begin();
	    it != deviceCommandList.end();
	    it++)
	{
	  //append the current command
	  commandName.push_back(it->first);
	  //append aliases to that command
	  for(std::vector<std::string>::iterator itAlias = it->second.begin();
	      itAlias != it->second.end();
	      itAlias++){
	    commandName.push_back(*itAlias);
	  }
	}	
      }
      iCommand = 0;
    }else{
      iCommand++;
    }

    for(;iCommand < commandName.size();iCommand++)
      {
      //Do the string compare and make sure the token is found a position 0 (the start)
	if(commandName[iCommand].find(currentToken) == 0){
	  return commandName[iCommand];
	}
      }
  }
  //not found
  return std::string("");  
}

CommandReturn::status Launcher::Sleep(std::vector<std::string> strArg,
		    std::vector<uint64_t> intArg) {
  if( strArg.size() < 1) {
    printf("Need a delay time in seconds\n");
  } else {
    double s_time = strtod( strArg[0].c_str(), NULL);
    if( s_time > 5.0) {
      sleep( intArg[0]);
    } else {
      usleep( (useconds_t)(s_time * 1e6) );
    }
  }
  return CommandReturn::OK;
}

#define TYPE_PADDING 10
CommandReturn::status Launcher::ListDevices(std::vector<std::string>,std::vector<uint64_t>){
  if(device.size() > 0){
    printf("Connected devices\n");
    for(size_t iDevice = 0;
	iDevice < device.size();
	iDevice++){    
      if(iDevice==size_t(activeDevice)){
	//Add "*" to currently active device
	printf("*%-2zu: %-*s%s\n",iDevice,
	       TYPE_PADDING,device[iDevice]->GetType().c_str(),
	       device[iDevice]->GetInfo().c_str());
      }else{
	printf("%-3zu: %-*s%s\n",iDevice,
	       TYPE_PADDING,device[iDevice]->GetType().c_str(),
	       device[iDevice]->GetInfo().c_str());
      }
    }
  }
  return CommandReturn::OK;
}

CommandReturn::status Launcher::SelectDevice(std::vector<std::string>,std::vector<uint64_t> iArg){
  if(iArg.size() > 0){
    if(iArg[0] < device.size()){
      activeDevice = iArg[0];
    }else{
      printf("Error: %zu out of range (%zu)",iArg[0],device.size());
    }
    return CommandReturn::OK;
  }
  return CommandReturn::BAD_ARGS;
}

