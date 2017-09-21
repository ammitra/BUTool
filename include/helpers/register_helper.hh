#ifndef __REGISTER_HELPER_HH__
#define __REGISTER_HELPER_HH__

#include "tool/CommandReturn.hh"

#include <string>
#include <vector>
#include <stdint.h>


namespace BUTool{  
  class RegisterHelper{  
  protected:
    enum RegisterNameCase {UPPER,LOWER,CASE_SENSITIVE};
  public:    
    RegisterHelper(){regCase = UPPER;}
    RegisterHelper(RegisterNameCase _regCase){regCase = _regCase;}
  protected:
    //Handle address table name case (default is upper case)
    RegisterNameCase GetCase(){return regCase;};
    void SetCase(RegisterNameCase _regCase){regCase = _regCase;};

    void ReCase(std::string & name);

    CommandReturn::status Read(std::vector<std::string> strArg,std::vector<uint64_t> intArg);
    CommandReturn::status Write(std::vector<std::string> strArg,std::vector<uint64_t> intArg);
    CommandReturn::status ListRegs(std::vector<std::string> strArg,std::vector<uint64_t> intArg);
    std::string RegisterAutoComplete(std::vector<std::string> const &,std::string const &,int);

    std::vector<std::string> RegNameRegexSearch(std::string regex);

    virtual std::vector<std::string> myMatchRegex(std::string regex)=0;
    virtual uint32_t RegReadAddress(uint32_t addr)=0;
    virtual uint32_t RegReadRegister(std::string const & reg)=0;
    virtual void RegWriteAction(std::string const & reg)=0;
    virtual void RegWriteAddress(uint32_t addr,uint32_t data)=0;
    virtual void RegWriteRegister(std::string const & reg, uint32_t data)=0;

    virtual uint32_t GetRegAddress(std::string const & reg)=0;
    virtual uint32_t GetRegMask(std::string const & reg)=0;
    virtual uint32_t GetRegSize(std::string const & reg)=0;
    virtual std::string GetRegMode(std::string const & reg)=0;
    virtual std::string GetRegPermissions(std::string const & reg)=0;
    virtual std::string GetRegDescription(std::string const & reg)=0;
    virtual std::string GetRegDebug(std::string const & reg){(void) reg; return "";}; // casting reg to void to keep comiler from complaining about unused var
    virtual std::string GetRegHelp(std::string const & reg){(void) reg; return "";}; // casting reg to void to keep comiler from complaining about unused var

  private:
    RegisterNameCase regCase;
  };

}
#endif
