#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>

//BUTool libraries
#include <BUTool/Launcher.hh>
#include <BUTool/CLI.hh>
#include <BUTool/CommandReturn.hh>
#include <BUTool/DeviceFactory.hh>
#include <BUException/ExceptionBase.hh>
#include <BUTool/helpers/parseHelpers.hh>

#include <readline/readline.h> //for rl_delete_text
#include <signal.h> //signals

//========================================================================
//Boost program options
#include <boost/program_options.hpp> //for configfile parsing
#define DEFAULT_CONFIG_FILE "/etc/BUTool" //path to default config file
namespace po = boost::program_options; //making life easier for boost                                 

std::vector<std::string> GetOptions(std::map<std::string,std::vector<std::string> > const & options,
				    std::string const & option){
  std::vector<std::string> ret;
  if(options.find(option) != options.end()){
    ret = options.at(option);
  }
  return ret;
}
void GenerateLibraryCommands(std::map<std::string,std::vector<std::string> > const & options,
			     std::vector<std::string> & commands){
  std::vector<std::string> libraries = GetOptions(options,"lib");
  for(auto itLib = libraries.begin(); itLib != libraries.end();itLib++){
    commands.push_back("add_lib "+*itLib);
  }
}
void GenerateCLICommands(std::map<std::string,std::vector<std::string> > const & options,
			 std::vector<std::string> & commands){
  std::vector<std::string> cmds = GetOptions(options,"cmd");
  for(auto itCmds = cmds.begin(); itCmds != cmds.end();itCmds++){
    commands.push_back(*itCmds);
  }
}
void GenerateScriptCommands(std::map<std::string,std::vector<std::string> > const & options,
			    std::vector<std::string> & commands){
  std::vector<std::string> scripts = GetOptions(options,"script");
  for(auto itScript = scripts.begin(); itScript != scripts.end();itScript++){
    commands.push_back("include "+*itScript);
  }
}
void GenerateAddDeviceCommands(std::map<std::string,std::vector<std::string> > const & options,
			       std::string const & device,
			       std::vector<std::string> & commands){
  //Get the default args for this device
  std::vector<std::string> defaultARGs = GetOptions(options,"DEFAULT_ARGS");
  std::string defaultArg;
  bool foundDefaultArg = false;
  for(auto itDA = defaultARGs.begin();itDA != defaultARGs.end();itDA++){
    if(itDA->substr(0,itDA->find(" ")) == device){
      foundDefaultArg= true;
      defaultArg = itDA->substr(itDA->find(" ")+1);
    }    
  }
  std::vector<std::string> args = GetOptions(options,device);
  for(auto itArgs = args.begin(); itArgs != args.end();itArgs++){
    std::string command;
    command+="add_device "+device+" ";
    if(itArgs->size()){
      command+=*itArgs;
    }else if(foundDefaultArg){
      command+=defaultArg;
    }else{
      throw std::runtime_error("Trying to build a default "+device+" with no default options!\n");      
    }
    commands.push_back(command);
  }
}
void FillOptions(po::parsed_options Options,
		 std::map<std::string,std::vector<std::string> > & parsedOptions){
  for(size_t iOpt = 0; iOpt < Options.options.size();iOpt++){
    std::string optionName = Options.options[iOpt].string_key;      
    std::string optionValue = "";
    if(Options.options[iOpt].value.size()){
      for(size_t i=0;i<Options.options[iOpt].value.size();i++){
	if(i){
	  optionValue+=" ";
	}
	optionValue += Options.options[iOpt].value[i];
      }
    }else{
      optionValue = "";
    }
    parsedOptions[optionName].push_back(optionValue);   
  }  
}
//==========================================================================

#define DevFac BUTool::DeviceFactory::Instance()

using namespace BUTool;

volatile bool running = true;

void signal_handler(int sig){
  if(sig == SIGINT){
    //remove SIGINT from signal_handler
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);    
       
    //Set an alarm for a second from now
    alarm(1);
    
    //cleanup line
#ifdef LINE_CTRL_C
    rl_delete_text(0,rl_end);
    printf("\n");
    rl_on_new_line();
    rl_done=1;
#endif
    rl_forced_update_display();
    
  }else if (sig == SIGALRM){    

    //re-enable SIGINT capture                                                                                                                                                                                    
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
  }
}

int main(int argc, char* argv[]) 
{
  //signal handling
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGALRM,&sa, NULL);

  //Create CLI and Create Command launcher (early, so we can set things)
  CLI cli;
  Launcher launcher;

  po::options_description autoLibraries("AUTOLOAD");
  autoLibraries.add_options()
    ("LIB,L", po::value<std::vector<std::string> >(), "Libraries automatically loaded");
    
  std::ifstream librariesConfigFile(DEFAULT_CONFIG_FILE); //Open config from default path
  po::variables_map librariesConfigMap; //container for config file arguments
    
  try { //get enviornmental libraries from config file
    po::store(parse_config_file(librariesConfigFile,autoLibraries,true), librariesConfigMap);
  } catch (std::exception &e) {
    fprintf(stderr, "Error in BOOST parse_config_file: %s\n", e.what());
    std::cout << autoLibraries << '\n';
    return 0;
  }

  if(librariesConfigMap.count("LIB")) { //libraries are defined in configfile
    std::vector<std::string> libFiles = librariesConfigMap["LIB"].as<std::vector<std::string> >(); //get args from config file
    for(uint iLibFile = 0; iLibFile < libFiles.size(); iLibFile++) {
      cli.ProcessString("add_lib " + libFiles[iLibFile]);
      std::vector<std::string> command = cli.GetInput(&launcher);
      if(command.size() >> 0){
	launcher.EvaluateCommand(command);
      }
    }
  }

  //============================================================================
  //Setup Boost programoptions
  //============================================================================
  po::options_description cli_options("BUTool Options");
  cli_options.add_options()
    ("help,h",    "Help screen")
    ("script,X", po::value<std::string>()->multitoken(), "Script files to load")    
    ("library,l",    po::value<std::string>()->multitoken(), "Device library plugin to load")
    ("cmd",      po::value<std::string>()->multitoken(), "Command to run");    
  
  po::options_description cfg_options("BUTool Options");
  cfg_options.add_options()
    ("DEFAULT_ARGS",   po::value<std::string >(), "Add all devices listed in config file as default")
    ("library",        po::value<std::string >()->multitoken(), "Device library plugin to load");
    
  //Load connections as program options based on DevFac
  std::vector<std::string> connections;
  int connections_count = 0; 
  std::vector<std::string> Devices = DevFac->GetDeviceNames();
  for(size_t iDevice = 0; iDevice < Devices.size(); iDevice++){
    std::string  CLI_flag;      
    std::string  CLI_full_flag;
    std::string  CLI_description;  
    if(DevFac->CLIArgs(Devices[iDevice],CLI_flag,CLI_full_flag,CLI_description)){
      std::string tmpName = CLI_full_flag + "," + CLI_flag;
      char *cName = new char[tmpName.size() + 1];
      char *cDesc = new char[CLI_description.size() + 1];
      strcpy(cName, tmpName.c_str());
      strcpy(cDesc, CLI_description.c_str());
      cli_options.add_options()
	(cName, po::value<std::string>()->multitoken()->implicit_value(""), cDesc);
      cfg_options.add_options()
	(cName, po::value<std::string>(), cDesc);
      delete[] cName;
      delete[] cDesc;
      connections.push_back(CLI_full_flag);
      connections_count++;
    }
  }

  //============================================================================
  //Run Program Options
  //============================================================================
  //Map of all parsed options
  std::map<std::string,std::vector<std::string> > allOptions;
  //Get options from config file
  std::ifstream configFile(DEFAULT_CONFIG_FILE);   
  if(configFile){
    try { 
      FillOptions(parse_config_file(configFile,cfg_options,true),
		  allOptions);
    } catch (std::exception &e) {
      fprintf(stderr, "Error in BOOST parse_config_file: %s\n", e.what());
    }
  }
  //Get options from command line,
  try { 
    FillOptions(parse_command_line(argc, argv, cli_options),
		allOptions);
  } catch (std::exception &e) {
    fprintf(stderr, "Error in BOOST parse_command_line: %s\n", e.what());
    return 0;
  }
  //Bail quickly if "help" was specified
  if(allOptions.find("help") != allOptions.end()){
    std::cout << cli_options << std::endl;
    return 0;
  }
  std::vector<std::string> commands;
  GenerateLibraryCommands(allOptions,commands);
  for(auto itDevice = connections.begin(); itDevice != connections.end();itDevice++){
    GenerateAddDeviceCommands(allOptions,*itDevice,commands);
  }
  GenerateScriptCommands(allOptions,commands);
  GenerateCLICommands(allOptions,commands);
  for(auto itCMD = commands.begin();itCMD != commands.end();itCMD++){
    std::cout << *itCMD << std::endl;
    cli.ProcessString(*itCMD);
  }


  // std::ifstream configFile(DEFAULT_CONFIG_FILE); //Open config from default path
  // po::variables_map commandMap; //container for command line arguments
  // po::variables_map configMap; //container for config file arguments
    
  // try { //get options from command line,
  //   po::store(parse_command_line(argc, argv, options), commandMap);
  // } catch (std::exception &e) {
  //   fprintf(stderr, "Error in BOOST parse_command_line: %s\n", e.what());
  //   std::cout << options << '\n';
  //   return 0;
  // }
  // try { //get options from config file
  //   po::store(parse_config_file(configFile,options,true), configMap);
  // } catch (std::exception &e) {
  //   fprintf(stderr, "Error in BOOST parse_config_file: %s\n", e.what());
  //   std::cout << options << '\n';
  //   return 0;
  // }

  // //help option
  // if(commandMap.count("help")){
  //   std::cout << options << '\n';
  //   return 0;
  // }

  // //Libraries defined in command line or config
  // if(commandMap.count("library")){ //If library flag is found
  //   std::string commandString;
  //   std::vector<std::string> userOpt = commandMap["library"].as<std::vector<std::string> >(); //get args from command line
    
  //   if(!userOpt.size()){//flag present with no arguments, use config file arguments
  //     try {
  // 	std::vector<std::string> configOpt = configMap["library"].as<std::vector<std::string> >(); //get args from config file
  // 	for (uint i = 0; i < configOpt.size(); i++) {//iterate through arguments in config file
  // 	  commandString = "add_lib " + configOpt[i];
  // 	  cli.ProcessString(commandString);
  // 	}
  //     } catch (std::exception &e) { //tried to use argument from config file but arg is not defined config file
  // 	fprintf(stderr, "ERROR getting library from config file at %s: %s\n", DEFAULT_CONFIG_FILE, e.what());
  // 	return 0;
  //     }
    
  //   } else {//iterate through arguments on commandline
  //     for(uint i = 0; i < userOpt.size(); i++){
  // 	commandString = "add_lib " + userOpt[i];
  // 	cli.ProcessString(commandString);
  //     }
  //   }
  // }

  // //If default flag is used, run all arguments in config file
  // if(commandMap.count("DEFAULT_ARGS")){
  //   std::vector<std::string> defaultArgs = configMap["DEFAULT_ARGS"].as<std::vector<std::string> >();
  //   for (uint iDefault = 0; iDefault < defaultArgs.size(); iDefault++){
  //     std::string commandString = "add_device " + defaultArgs[iDefault];
  //     cli.ProcessString(commandString); //std::cout << commandString << std::endl;
  //   }
  // }

  // //Iterate through devices
  // for (uint iDevice = 0; iDevice < connections.size(); iDevice++) {
  //   //If flag for device is found
  //   if(commandMap.count(connections[iDevice])) {
  //     //Get argument from command line, 
  //     std::vector<std::string> userOpt = commandMap[connections[iDevice]].as<std::vector<std::string> >();
  //     //If argument is "", Then flag was used with no defaults. So use defaults from config file
  //     if(!userOpt.size()) {
  // 	//Get all default args from config file
  // 	std::vector<std::string> configOpt = configMap["DEFAULT_ARGS"].as<std::vector<std::string> >();
  // 	//loop through default args
  // 	for (uint iDefault = 0; iDefault < configOpt.size(); iDefault++) {
  // 	  std::string configLine = configOpt[iDefault];
  // 	  std::string deviceName = configLine.substr(0, configLine.find(" "));
  // 	  //if Device Name of DEFAULT_ARG matches device name of argument flag used
  // 	  if(deviceName == connections[iDevice]){
  // 	    //copy this DEFAULT_ARG and run it
  // 	    std::string commandString = "add_device " + configOpt[iDefault];
  // 	    cli.ProcessString(commandString);//std::cout << commandString << std::endl;
  // 	  }
  // 	}
  // 	//flag on command line is found with user options so run it
  //     } else {
  // 	//run user options
  // 	for (uint iUser = 0; iUser < userOpt.size(); iUser++) {
  // 	  std::string commandString = "add_device " + connections[iDevice] + " " + userOpt[iUser];
  // 	  cli.ProcessString(commandString);//std::cout << commandString << std::endl;
  // 	}
  //     }
  //   }
  // }

  // // //setup connections
  // // for(int i = 0; i < connections_count; i++){
  // //   if(commandMap.count(connections[i])){
  // //     std::string commandString;
  // //     std::vector<std::string> userOpt = commandMap[connections[i]].as<std::vector<std::string> >(); //get args from command line

  // //     if(!userOpt.size()){//flag present with no arguments, use config file arguments
  // // 	try {
  // // 	  std::vector<std::string> configOpt = configMap[connections[i]].as<std::vector<std::string> >(); //get args from config file
  // // 	  for (uint j = 0; j < configOpt.size(); j++) {//iterate through arguments in config file
  // // 	    commandString = "add_device " + connections[i] + " " + configOpt[j];
  // // 	    cli.ProcessString(commandString);
  // // 	  }
  // // 	} catch (std::exception &e) { //tried to use argument from config file but arg is not defined config file
  // // 	  fprintf(stderr, "ERROR getting %s from config file at %s: %s\n", connections[i].c_str(), DEFAULT_CONFIG_FILE, e.what());
  // // 	  return 0;
  // // 	}
      
  // //     } else {//iterate through arguments on commandLine
  // // 	commandString = "add_device " + connections[i];
  // // 	for(uint j = 0; j < userOpt.size(); j++){
  // // 	  commandString = "add_device " + connections[i] + " " + userOpt[j];
  // // 	  cli.ProcessString(commandString);
  // // 	}
  // //     }
  // //   }
  // // }

  // //Load scripts    
  // if(commandMap.count("script")){
  //   std::vector<std::string> userOpt = commandMap["script"].as<std::vector<std::string> >(); //get args from command line

  //   if(!userOpt.size()) {//flag present with no arguments, use config file arguments
  //     try {
  // 	std::vector<std::string> configOpt = configMap["script"].as<std::vector<std::string> >(); //get args from config file
  // 	for (uint i = 0; i < configOpt.size(); i++) {//iterate through arguments in config file
  // 	  cli.ProcessFile(configOpt[0]);
  // 	}
  //     } catch (std::exception &e) { //tried to use argument from config file but arg is not defined config file
  // 	fprintf(stderr, "ERROR getting script from config file at %s: %s\n", DEFAULT_CONFIG_FILE, e.what());
  // 	return 0;
  //     }
    
  //   } else {//iterate through arguments on commandLine
  //     for(uint i = 0; i < userOpt.size(); i++){
  // 	cli.ProcessFile(userOpt[i]);
  //     }
  //   }
  // }

  //============================================================================
  //Main loop
  //============================================================================

  while (running) 
    {
      try {
	//Get parsed command
	std::vector<std::string> command = cli.GetInput(&launcher);

	//Check if this is just the user hitting return (do nothing if it is)
	if(command.size() > 0){
	  //Launch command function
	  CommandReturn::status ret = launcher.EvaluateCommand(command);

	  //check return value
	  if(ret == CommandReturn::EXIT){
	    //Shutdown tool
	    running = false;       
	  }else if ( ret == CommandReturn::NOT_FOUND ){
	    if(cli.InScript()){ //Fail to command line if in script
	      std::cout << "Bad command in script. Exit!\n";
	      running = false;	  
	    }else{
	      std::cout << "Bad command\n";
	    }
	  }
	}
      }catch (BUException::exBase & e){

	uint32_t verbose_level = launcher.GetVerbosity();

       	std::string errorstr(e.what());
       	std::cout << "\n\n\nCaught Tool exception: " << errorstr << std::endl;
	
	switch(verbose_level){
	case 1:
	  std::cout << LimitStringLines(e.Description(),5,5);
	  break;
	case 2:
	  std::cout << LimitStringLines(e.Description(),10,10);
	  break;
	case 3:
	  std::cout << LimitStringLines(e.Description(),20,20);
	  break;
	case 9:
	  std::cout << LimitStringLines(e.Description(),0,1000);
	  break;
	default:
	  std::cout << LimitStringLines(e.Description());
	  break;
	}
       	if(9 == verbose_level){
	  std::cout << e.StackTrace();
	}
	if(cli.InScript()){ //Fail to command line if in script
       	  running = false;
	}
       	cli.ClearInput(); //Clear any scripted commands
      }catch (std::exception  & e){
	std::string errorstr(e.what());
	errorstr.erase(std::remove(errorstr.begin(), 
				   errorstr.end(), 
				   '\n'), 
		       errorstr.end());
	std::cout << "\n\n\nCaught std::exception " << errorstr << ".\n";
	//std::cout << "\n\n\nCaught std::exception " << errorstr << ". Shutting down.\n";
	//running = false;
	cli.ClearInput(); //Clear any scripted commands
      }
    }
  return 0;
}
