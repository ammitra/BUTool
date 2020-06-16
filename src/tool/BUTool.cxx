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

using namespace BUTool;                                                 

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



std::string LimitStringLines(std::string source,size_t beginLineCount = 5,size_t endLineCount = 2) {
  //Load the first beginLineCount lines.
  if((source.size() > 0)&&(source.find('\n') == std::string::npos)){
    source=source+'\n';
  }
  std::string beginString;
  while( beginLineCount && !source.empty()) {
    //Find the next new line
    size_t pos = source.find('\n');
    if(pos == std::string::npos) {
      source.clear();
      break;
    }
    
    //append the line associated with it to our begin string with a tab at the beginning
    beginString += std::string("\t") + source.substr(0,pos) + std::string("\n");
    //Move past the newline
    pos++;
    //trim string
    source = source.substr(pos,source.size()-pos);
    
    beginLineCount--;
  }

  std::string endString;
  while(endLineCount && !source.empty()) {
    //Find the next new line
    size_t pos = source.rfind('\n');
    
    if(pos == std::string::npos) {
      //We didn't find a newline, so this is the last line
      pos = 0;
    } else if(++pos == source.size()) { //Move past the null line, but catch if it was the last char.
      source.resize(source.size()-1);
      continue;
    }
    
    //reverse append the line associated with it to our begin string with a tab at the beginning
    endString = std::string("\t") + source.substr(pos,source.size()-pos) + 
      std::string("\n") + endString;
    
    //trim source string
    if(pos >0) {
      pos--; //Move back to the newline
      source = source.substr(0,pos); //trim up to the newline
    } else { // nothing left, so clear
      source.clear();
    }
    
    endLineCount--;
  }
  
  //Build final string
  if(!source.empty()) {
    //Count the number of skipped lines if non-zero
    size_t skippedLineCount = 1;
    for(size_t iStr = 0; iStr < source.size();iStr++) {
      if(source[iStr] == '\n')
	skippedLineCount++;
    }
    std::ostringstream s;
    s << "*** Skipping " << skippedLineCount << " lines! ***\n";
    beginString += s.str();
  }
  beginString += endString;
  return beginString;
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
					    cmd);

    //connections
    std::map<std::string,TCLAP::MultiArg<std::string>* >connections;
    std::vector<std::string> Devices = DevFac->GetDeviceNames();
    for(size_t iDevice = 0;
	iDevice < Devices.size();
	iDevice++){
      std::string  CLI_flag;      
      std::string  CLI_full_flag;
      std::string  CLI_description;

      if(DevFac->CLIArgs(Devices[iDevice],CLI_flag,CLI_full_flag,CLI_description)){
	  connections[Devices[iDevice]] = new TCLAP::MultiArg<std::string>(CLI_flag,       //one char flag
									   CLI_full_flag,  // full flag name
									   CLI_description,//description
									   false,          //required
									   "string",       // type
									   cmd);
      }
    }
    
    //Device libraries
    TCLAP::MultiArg<std::string> libraries("l",                    //one char flag
					   "add_library",          // full flag name
					   "Device library to add",//description
					   false,                  //required
					   "string",               // type
					   cmd);



    //Parse the command line arguments
    cmd.parse(argc,argv);

    //Load requested device libraries
    for(std::vector<std::string>::const_iterator it = libraries.getValue().begin(); 
	it != libraries.getValue().end();
	it++)
      {
	cli.ProcessString("add_lib " + *it);
      }


    //setup connections
    //Loop over all device types
    for(std::map<std::string,TCLAP::MultiArg<std::string>* >::iterator itDeviceType = connections.begin();
	itDeviceType != connections.end();
	itDeviceType++){
      //Loop over connections requests for each device
      for(std::vector<std::string>::const_iterator itDev = itDeviceType->second->getValue().begin(); 
	  itDev != itDeviceType->second->getValue().end();
	  itDev++)
	{
	  cli.ProcessString("add_device " + itDeviceType->first + " " +  *itDev);
	}
    }


    //Load scripts
    if(scriptFile.getValue().size()){
      cli.ProcessFile(scriptFile.getValue());
    }



  } catch (TCLAP::ArgException &e) {
    fprintf(stderr, "Error %s for arg %s\n",
	    e.error().c_str(), e.argId().c_str());
    return 0;
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

boost::program_options::variables_map loadConfig(std::string const & configFileName,
						 boost::program_options::options_description const & fileOptions) {
  // This is a container for the information that fileOptions will get from the config file
  boost::program_options::variables_map vm;  
  // Check if config file exists
  std::ifstream ifs{configFileName};
  printf("Config file \"%s\" %s\n",configFileName.c_str(), (!ifs.fail()) ? "exists" : "does not exist");
  if(ifs) {
    // If config file exists, parse ifs into fileOptions and store information from fileOptions into vm
    boost::program_options::store(parse_config_file(ifs, fileOptions), vm);
    printf("checkpoint1\n");
  }
  return vm;
}


std::string getFromConfig(std::string configFile) {
  printf("using .xml connection file defined in %s\n", configFile.c_str());
  //Getting connection file from config file
  std::string connectionFile=DEFAULT_CONNECTION_FILE;
  // fileOptions is for parsing config files
  boost::program_options::options_description fileOptions{"File"};
  //something to do with C++ magic
  fileOptions.add_options()
    ("connectionFile",
     boost::program_options::value<std::string>()->default_value(DEFAULT_CONNECTION_FILE),
     "connection file");
  boost::program_options::variables_map configOptions;
  try{
    configOptions = loadConfig(configFile,fileOptions);
    printf("checkpoint2\n");
    // Check for information in configOptions
    if(configOptions.count("connectionFile")) {
      connectionFile = configOptions["connectionFile"].as<std::string>();
    }
  } catch(const boost::program_options::error &ex){
    printf("Caught exception in function loadConfig(): %s \n", ex.what());
  }
  return connectionFile;
}

arg[0] = getFromConfig(DEFAULT_CONFIG_FILE);
