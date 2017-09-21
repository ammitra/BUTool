#ifndef __COMMANDDATASTRUCUTRE_HPP__
#define __COMMANDDATASTRUCUTRE_HPP__

#include <vector>
#include <string>
#include <stdint.h>

#include "tool/ToolException.hh"

//===================================
//Command data structure functor class
//  This holds the command function pointer, help, and autocomplete function pointer
//  This also provides a set, get help, autocomplete and functor() memebers
#define CALL_MEMBER_FUNCTION(object,ptrToMember) ((object).*(ptrToMember))

template<class T>
class CMD_DS{
public:
  //Constructor creates empty objecs
  CMD_DS():FcnPtr(NULL),HelpString(),AutoCompletePtr(){
  };
  //Set all the internal data members 
  void set(CommandReturn::status (T::*_FcnPtr)(std::vector<std::string>,std::vector<uint64_t>),
	   std::string _Help,
	   std::string (T::* _AutoComplete)(std::vector<std::string> const &,std::string const &,int)){
    FcnPtr = _FcnPtr;
    HelpString = _Help;
    AutoCompletePtr = _AutoComplete;	
  }

  /*=============================================================================
    Description: 
      Class functor () that executes the stored function pointer using objPtr as the "this" pointer
    Return:
      Passes on the return value of the called function pointer
  =============================================================================*/
  CommandReturn::status operator()(T * objPtr,std::vector<std::string> s,std::vector<uint64_t> i){
    if( objPtr == NULL){
      BUException::COMMAND_LIST_ERROR e;
      e.Append("NULL Object pointer used with command function pointer\n");
      throw e;
    }
    if( FcnPtr == NULL){
      BUException::COMMAND_LIST_ERROR e;
      e.Append("NULL function pointer called for command\n");
      throw e;
    }
    //Call the function pointer on the class it is in with s,i vectors
    //    return *objPtr.*(FcnPtr)(s,i);      
    return CALL_MEMBER_FUNCTION(*objPtr,FcnPtr)(s,i);
  }
  
  /*=============================================================================
    Description: 
      Returns the help for this command
    Return:
      Command help
  =============================================================================*/
  //Help function
  std::string const & help() {
    return HelpString;
  };

  /*=============================================================================
    Description: 
      Call the auto complete function for this commmand from object objPtr
    Return:
      Autocomplete string
  =============================================================================*/  

  std::string autoComplete(T* objPtr,std::vector<std::string> const & line,std::string const & currentToken,int state){
    //Make sure the objPtr is valid
    if(objPtr == NULL){
      BUException::COMMAND_LIST_ERROR e;
      e.Append("NULL Object pointer used with command auto-complete function pointer\n");
      throw e;
    }
    if(AutoCompletePtr != NULL){
      //Call autocomplete function pointer if specified
      //      return (*objPtr.*(AutoCompletePtr))(line,currentToken,state);
      return CALL_MEMBER_FUNCTION(*objPtr,AutoCompletePtr)(line,currentToken,state);
    }
    //No auto complete for this command
    return std::string("");
  }
private:
  //Function pointer for this command
  CommandReturn::status (T::*FcnPtr)(std::vector<std::string>,std::vector<uint64_t>);
  //Help for this command
  std::string HelpString;
  //Optional auto-complete for this command
  std::string (T::* AutoCompletePtr)(std::vector<std::string> const &,std::string const &,int);  
};

#endif
