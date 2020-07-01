#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>

//TCLAP parser
#include <tclap/CmdLine.h>
#include <BUTool/Launcher.hh>
#include <BUTool/CLI.hh>
#include <BUTool/CommandReturn.hh>
#include <BUTool/DeviceFactory.hh>

#include <BUException/ExceptionBase.hh>

#include <readline/readline.h> //for rl_delete_text
#include <signal.h> //signals

#include <BUTool/helpers/parseHelpers.hh>
#include <boost/program_options.hpp> //for configfile parsing

#define BUTOOL_AUTOLOAD_LIBRARY_LIST "BUTOOL_AUTOLOAD_LIBRARY_LIST"
#define DEFAULT_CONFIG_FILE          "/etc/BUTool.cfg" //path to default config file
//#define DEFAULT_CONFIG_FILE "/home/mikekremer/work/misc/BUTool.cfg"

using namespace BUTool;
namespace po = boost::program_options; //making life easier for boost                                 

#define DevFac BUTool::DeviceFactory::Instance()

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

  //Create CLI
  CLI cli;

  //Create Command launcher (early, so we can set things)
  Launcher launcher;

  std::vector<std::string> emptyVector = {"", ""};

  //Setup Boost programoptions
  po::options_description options("BUTool Options");
  options.add_options()
    ("help,h",                                                  "Help screen")
    ("test,t",    po::value<std::string>()->implicit_value(""), "for testing")
    ("script,X",  po::value<std::string>()->implicit_value(""), "Script filename")
    ("library,l", po::value<std::string>()->implicit_value(""), "Device library to add");
    
  //Load libraries from env variable
  if (NULL != getenv(BUTOOL_AUTOLOAD_LIBRARY_LIST)){
    std::vector<std::string> libFiles = splitString(getenv(BUTOOL_AUTOLOAD_LIBRARY_LIST),":");
    for(size_t iLibFile = 0 ; iLibFile < libFiles.size() ; iLibFile++){
      //Add a add_lib command for each library
      cli.ProcessString("add_lib " + libFiles[iLibFile]);      
      //Ask the CLI to process this command.
      std::vector<std::string> command = cli.GetInput(&launcher);
      //If the command was well formed, tell the launcher to launch it. 
      if(command.size() > 0){
	//Launch command function (for add lib)
	launcher.EvaluateCommand(command);
	//Ignore the return value.  It eithe works or not.
      }
    }
  }

  

  
  try {
    /*
    TCLAP::CmdLine cmd("Tool for talking to HW modules.",
		       ' ',
		       "BUTool v1.0");    
    
    //Script files
    TCLAP::ValueArg<std::string> scriptFile("X",              //one char flag
					    "script",         // full flag name
					    "script filename",//description
					    false,            //required
					    std::string(""),  //Default is empty
					    "string",         // type
					    cmd); */
					    
    //connections
    std::map<std::string,TCLAP::MultiArg<std::string>* >connections2;
    std::vector<std::string> connections;
    int connections_count = 0;

    std::vector<std::string> Devices = DevFac->GetDeviceNames();
    for(size_t iDevice = 0; iDevice < Devices.size(); iDevice++){
      std::string  CLI_flag;      
      std::string  CLI_full_flag;
      std::string  CLI_description;
      
      if(DevFac->CLIArgs(Devices[iDevice],CLI_flag,CLI_full_flag,CLI_description)){
	std::string tmpFlag = CLI_full_flag;
	std::string tmpName = CLI_full_flag + "," + CLI_flag;
	std::string tmpDesc = CLI_description;
	char *cFlag = new char[tmpFlag.size() + 1];
	char *cName = new char[tmpName.size() + 1];
	char *cDesc = new char[tmpDesc.size() + 1];
	strcpy(cFlag, tmpFlag.c_str());
	strcpy(cName, tmpName.c_str());
	strcpy(cDesc, tmpDesc.c_str());
	options.add_options()
	  (cName, po::value<std::string>()->implicit_value(""), cDesc);
	delete[] cFlag;
	delete[] cName;
	delete[] cDesc;
	connections.push_back(tmpFlag);
	connections_count++;
	/*connections2[Devices[iDevice]] = new TCLAP::MultiArg<std::string>(CLI_flag,       //one char flag
									 CLI_full_flag,  // full flag name
									 CLI_description,//description
									 false,          //required
									 "string",       // type
									 cmd);*/
      }
    }
    
    //Device libraries
    /*TCLAP::MultiArg<std::string> libraries("l",                    //one char flag
    					   "add_library",          // full flag name
    					   "Device library to add",//description
    					   false,                  //required
    					   "string",               // type
    					   cmd); */
    
    std::ifstream configFile(DEFAULT_CONFIG_FILE);
    po::variables_map commandMap;
    po::variables_map configMap;
    
    try { //get options from command line,
      po::store(parse_command_line(argc, argv, options), commandMap);
    } catch (std::exception &e) {
      fprintf(stderr, "Error in BOOST parse_command_line: %s\n", e.what());
      std::cout << options << '\n';
      return 0;
    }
    try { //get options from config file
      po::store(parse_config_file(configFile,options,true), configMap);
    } catch (std::exception &e) {
      fprintf(stderr, "Error in BOOST parse_config_file: %s\n", e.what());
      std::cout << options << '\n';
      return 0;
    }

    //help option
    if(commandMap.count("help")){
      std::cout << options << '\n';
      return 0;
    }

    if(!commandMap.count("test")){
      printf("running with no test option\n");
    } else {
      std::string userOpt = commandMap["test"].as<std::string>();
      if(userOpt == ""){
	std::string configOpt = configMap["test"].as<std::string>();
	printf("running with config test option: %s\n", configOpt.c_str());
      } else {
	printf("running with user option: %s\n", userOpt.c_str());      
      }
    }

    //Parse the command line arguments
    if(commandMap.count("library")){
      std::string commandString;
      std::string userOpt = commandMap["library"].as<std::string>();
      if(userOpt == "") {
	printf("Using library defined in config file at %s\n", DEFAULT_CONFIG_FILE);
	std::string configOpt = configMap["library"].as<std::string>();
	commandString = "add_lib " + configOpt;
      } else {
	commandString = "add_lib " + userOpt;
      }
      cli.ProcessString(commandString);
    }

    //setup connections
    for(int i = 0; i < connections_count; i++){
      if(commandMap.count(connections[i])){
	std::string commandString;
	std::string userOpt = commandMap[connections[i]].as<std::string>();
	if(userOpt == "") {
	  printf("Using connections defined in config file at %s\n", DEFAULT_CONFIG_FILE);
	  std::string configOpt = configMap[connections[i]].as<std::string>();
	  commandString = "add_device " + connections[i] + " " + configOpt;
	  //printf("BOOST config: add_device %s %s\n", connections[i].c_str(), configOpt.c_str());
	} else {
	  printf("BOOST user: add_device %s %s\n", connections[i].c_str(), userOpt.c_str());
	  //commandString = "add_device " + connections[i] + " " + userOpt;
	}
	cli.ProcessString(commandString);
      }
    }

    //Load scripts    
    if(commandMap.count("script")){
      std::string userOpt = commandMap["script"].as<std::string>();
      if(userOpt == "") {
	printf("Using script defined in config file at %s\n", DEFAULT_CONFIG_FILE);
	std::string configOpt = configMap["script"].as<std::string>();
	cli.ProcessFile(configOpt);
      } else {
	cli.ProcessFile(userOpt);
      }
    }

    //TCLAP Methods for option handling
    /*cmd.parse(argc,argv);

      for(std::vector<std::string>::const_iterator it = libraries.getValue().begin(); 
      it != libraries.getValue().end();
      it++)
      {
      cli.ProcessString("add_lib " + *it);
      std::string tmpPrint = "add_lib " + *it;
      printf("from TCLAP: %s\n", tmpPrint.c_str());
      }
      
      //Loop over all device types
      for(std::map<std::string,TCLAP::MultiArg<std::string>* >::iterator itDeviceType = connections2.begin(); itDeviceType != connections2.end(); itDeviceType++){
      //Loop over connections requests for each device
      for(std::vector<std::string>::const_iterator itDev = itDeviceType->second->getValue().begin(); itDev != itDeviceType->second->getValue().end(); itDev++){
      cli.ProcessString("add_device " + itDeviceType->first + " " +  *itDev);
      std::string tmpPrint = "add_device " + itDeviceType->first + " " + *itDev;
      printf("from TCLAAP: %s\n", tmpPrint.c_str());
      }
      }
     
      //Run script
      if(scriptFile.getValue().size()){
      cli.ProcessFile(scriptFile.getValue());
      }*/

  } catch (TCLAP::ArgException &e) {
    fprintf(stderr, "Error %s for arg %s\n",
	    e.error().c_str(), e.argId().c_str());
    //return 0;
  }

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
