#include "helpers/register_helper.hh" 

#include <boost/algorithm/string/case_conv.hpp>
#include <stdio.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h> //for PRI

void BUTool::RegisterHelper::ReCase(std::string & name){
  switch(regCase){
  case LOWER:
    boost::algorithm::to_lower(name);    
    break;
  case UPPER:
    boost::algorithm::to_upper(name);    
    break;
  case CASE_SENSITIVE:
    //Do nothing
  default:
    break;
  }
}

CommandReturn::status BUTool::RegisterHelper::Read(std::vector<std::string> strArg,
						   std::vector<uint64_t> intArg){
  // sort out arguments                                                                                       
  size_t readCount = 1;
  std::string flags("");
  bool numericAddr = true;

  switch( strArg.size() ) {
  case 0:                     // no arguments is an error                                                     
    return CommandReturn::BAD_ARGS;
  case 3:                     // third is flags                                                               
    flags = strArg[2];

    // fall through to others                                                                                 
  case 2:                     // second is either count or flags                                              
    if( isdigit( strArg[1].c_str()[0])){
      readCount = intArg[1];
    } else if(2 == strArg.size()){
      //Since this isn't a digit it must be the flags
      // of a two argument request
      flags = strArg[1];
    }else{
      //This is a non-digit second argument of a three argument.
      return CommandReturn::BAD_ARGS;
    }

    // fall through to address                                                                                
  case 1:                     // one must be an address                                                       
    numericAddr = isdigit( strArg[0].c_str()[0]);
    ReCase(strArg[0]);
    break;
  default:
    printf("Too many arguments after command\n");
    return CommandReturn::BAD_ARGS;
  }

  //Convert flags to capital
  boost::algorithm::to_upper(flags);
  bool printWord64      = (flags.find("D") != std::string::npos);
  bool skipPrintZero    = (flags.find("N") != std::string::npos);

  if(numericAddr){
    uint32_t addr_incr = printWord64 ? 2 : 1;
    uint32_t readNumber = 0;
    uint32_t lineWordCount = printWord64 ? 4 : 8;

    for(uint32_t addr = intArg[0]; addr < (intArg[0] + readCount*addr_incr);addr+=addr_incr){

      //Print the address
      if(readNumber % lineWordCount == 0){
	printf("%08x: ",  addr);
      }      
      //read the value
      uint64_t val = RegReadAddress(addr);
      readNumber++;
      if(printWord64){
	//Grab the upper bits if we are base 64
	val |= (uint64_t(RegReadAddress(addr+1)) << 32);
      }
      //Print the value if we are suppose to
      if(!skipPrintZero ||  (val != 0)){
	printf(" %0*" PRIX64, printWord64?16:8, val);	
      }else{
	printf(" %*s", printWord64?16:8," ");	
      }
      //End line
      if(readNumber % lineWordCount == 0){
	printf("\n");
      }
    }
    //final end line
    printf("\n");
  } else {
    std::vector<std::string> names = RegNameRegexSearch(strArg[0]);
    for(size_t iName = 0; iName < names.size();iName++){
      uint32_t val = RegReadRegister(names[iName]);
      if(!skipPrintZero || (val != 0)){
	printf("%50s: 0x%08X\n",names[iName].c_str(),val);
      }
    }
  }
  return CommandReturn::OK;
}

CommandReturn::status BUTool::RegisterHelper::Write(std::vector<std::string> strArg,
						    std::vector<uint64_t> intArg) {

  if (strArg.size() ==0){
    return CommandReturn::BAD_ARGS;
  }
  std::string saddr = strArg[0];
  ReCase(saddr);
  
  switch( strArg.size()) {
  case 1:			// address only means Action(masked) write
    printf("Mask write to %s\n", saddr.c_str() );
    RegWriteAction(saddr);
    break;
  case 2:
    printf("Write to ");
    if( isdigit( saddr.c_str()[0])) {
      printf("address %s\n", saddr.c_str() );
      RegWriteAddress(intArg[0],uint32_t(intArg[1]));
    } else {
      printf("register %s\n", saddr.c_str());
      RegWriteRegister(saddr,uint32_t(intArg[1]));
    }
    break;
  default:
    return CommandReturn::BAD_ARGS;
  }	

  return CommandReturn::OK;
}



//static void ReplaceStringInPlace(std::string& subject, std::string const& search,
//				 std::string const & replace) {
//  size_t pos = 0;
//  while ((pos = subject.find(search, pos)) != std::string::npos) {
//    subject.replace(pos, search.length(), replace);
//    pos += replace.length();
//  }
//}

std::vector<std::string> BUTool::RegisterHelper::RegNameRegexSearch(std::string regex)
{
  // return a list of nodes matching regular expression

  //THis wrapper function creates a common framework for the regex syntax
  // convert regex so "." is literal, "*" matches any string
  // "perl:" prefix leaves regex unchanged
  ReCase(regex);
//  if( regex.size() > 6 && regex.substr(0,5) == "PERL:") {
//    printf("Using PERL-style regex unchanged\n");
//    regex = regex.substr( 5);
//  } else {
//    ReplaceStringInPlace( regex, ".", "#");
//    ReplaceStringInPlace( regex, "*",".*");
//    ReplaceStringInPlace( regex, "#","\\.");
//  }  
  //Run the regex on the derived class's myMatchRegex
  return myMatchRegex(regex);
}


CommandReturn::status BUTool::RegisterHelper::ListRegs(std::vector<std::string> strArg,
						       std::vector<uint64_t> intArg){
  (void) intArg; // keeps compiler from complaining about unused args
  std::vector<std::string> regNames;
  std::string regex;


  //Get display parameters
  bool debug = false;
  bool describe = false;
  bool help = false;
  if( strArg.size() < 1) {
    printf("Need regular expression after command\n");
    return CommandReturn::BAD_ARGS;
  }    
  regex = strArg[0];

  if( strArg.size() > 1) {
    boost::algorithm::to_upper(strArg[1]);
    switch(strArg[1][0]) {
    case 'D':
      debug = true;
      break;
    case 'V':
      describe = true;
      break;
    case 'H':
      help = true;
      break;
    }
  }

  //Get the list of registers associated with the search term
  regNames = RegNameRegexSearch(regex);
  size_t matchingRegCount = regNames.size();
  for(size_t iReg = 0; iReg < matchingRegCount;iReg++){
    std::string const & regName = regNames[iReg];
    //Get register parameters
    uint32_t addr = GetRegAddress(regName);
    uint32_t mask = GetRegMask(regName);
    uint32_t size = GetRegSize(regName);
    
    //Print main line
    printf("  %3zu: %-60s (addr=%08x mask=%08x) ", iReg+1, regNames[iReg].c_str(), addr,
	   mask);
    //Print mode attribute
    printf("%s",GetRegMode(regName).c_str());
    //Print permission attribute
    printf("%s",GetRegPermissions(regName).c_str());
    if(size > 1){
      //Print permission attribute
      printf(" size=0x%08X",size);    
    }
    //End first line
    printf("\n");

    //optional description
    if(describe){
      printf("       %s\n",GetRegDescription(regName).c_str());
    }
    
    //optional debugging info
    if(debug){
      printf("%s\n",GetRegDebug(regName).c_str());
    }
    //optional help
    if(help){
      printf("%s\n",GetRegHelp(regName).c_str());
    }

  }
  return CommandReturn::OK;
}


std::string BUTool::RegisterHelper::RegisterAutoComplete(std::vector<std::string> const & line , std::string const & currentToken,int state){
  (void) line; // casting to void to keep comiler from complaining about unused param
  static size_t pos;
  static std::vector<std::string> completionList;
  if(!state) {
    //Check if we are just starting out
    pos = 0;
    completionList = RegNameRegexSearch(currentToken+std::string("*"));
  } else {
    //move forward in pos
    pos++;
  }
    
  if(pos < completionList.size()){
    return completionList[pos];
  }
  //not found
  return std::string("");    
}
