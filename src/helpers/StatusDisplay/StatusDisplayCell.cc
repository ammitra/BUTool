#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm> //std::max
#include <iomanip> //for std::setw
#include <ctype.h> // for isDigit()

#include <stdio.h> //snprintf
#include <stdlib.h> //strtoul

#include <BUTool/ToolException.hh>
#include <BUTool/helpers/StatusDisplay/StatusDisplayCell.hh>

//For PRI macros
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace BUTool{

  using boost::algorithm::iequals;


  //=============================================================================
  //===== Cell Class
  //=============================================================================
  void StatusDisplayCell::Clear()
  {
    address.clear();
    description.clear();
    row.clear();
    col.clear();
    word.clear();
    wordShift.clear();
    format.clear();
    displayRule.clear();
    enabled = true;
    statusLevel = 0;
  }
  void StatusDisplayCell::Setup(std::string const & _address,  //stripped of Hi/Lo
		   std::string const & _description,
		   std::string const & _row, //Stripped of Hi/Lo
		   std::string const & _col, //Stripped of Hi/Lo
		   std::string const & _format,
		   std::string const & _rule,
		   std::string const & _statusLevel,
		   bool _enabled)
  {
    //These must all be the same
    CheckAndThrow("Address",address,_address);
    CheckAndThrow(address + " row",row,_row);
    CheckAndThrow(address + " col",col,_col);
    CheckAndThrow(address + " format",format,_format);
    CheckAndThrow(address + " rule",displayRule,_rule);
    
    //Append the description for now
    description += _description;

    //any other formatting
    statusLevel = strtoul(_statusLevel.c_str(),
		     NULL,0);
    enabled = _enabled;
  }

  void StatusDisplayCell::Fill(uint32_t value,
		  size_t bitShift)
  {
    word.push_back(value);
    wordShift.push_back(bitShift);
  }

  int StatusDisplayCell::DisplayLevel() const {return statusLevel;}

  bool StatusDisplayCell::SuppressRow( bool force) const
  {
    // Compute the full value for this entry
    uint64_t val = ComputeValue();
    bool suppressRow = (iequals( displayRule, "nzr") && (val == 0)) && !force;
    return suppressRow;
  }

  std::string const & StatusDisplayCell::GetRow() const {return row;}
  std::string const & StatusDisplayCell::GetCol() const {return col;}
  std::string const & StatusDisplayCell::GetDesc() const {return description;}
  std::string const & StatusDisplayCell::GetAddress() const {return address;}

  void StatusDisplayCell::SetAddress(std::string const & _address){address = _address;}
  uint32_t const & StatusDisplayCell::GetMask() const {return mask;}
  void StatusDisplayCell::SetMask(uint32_t const & _mask){mask = _mask;}


  bool StatusDisplayCell::Display(int level,bool force) const
  {
    // Compute the full value for this entry
    uint64_t val = ComputeValue();

    // Decide if we should display this cell
    bool display = (level >= statusLevel) && (statusLevel != 0);

    // Check against the print rules
    if(iequals(displayRule,"nz")){
      display = display & (val != 0); //Show when non-zero
    } else if(iequals(displayRule,"z")){
      display = display & (val == 0); //Show when zero
    }

    //Apply "channel"-like enable mask
    display = display && enabled;

    //Force display if we want
    display = display || force;
    return display;
  }
  uint64_t StatusDisplayCell::ComputeValue() const
  {
    //Compute full value
    uint64_t val = 0;
    for(size_t i = 0; i < word.size();i++){
      if(word.size() > 1){//If we have multiple values to merge
	val += (uint64_t(word[i]) << wordShift[i]);
      }else{//If we have just one value
	val += uint64_t(word[i]);
      }
    }
    
    //We do not support signed integers that have more than 32 bits
    if(iequals(format,std::string("d")) &&
       word.size() == 1){
      //This is goign to be printed with "d", so we need to sign extend the number we just comptued
      uint64_t temp_mask = GetMask();

      //Count bits in mask 
      uint64_t b; // c accumulates the total bits set in v

      for (b = 0; temp_mask; temp_mask >>= 1)
	{
	  b += temp_mask & 1;
	}


      // sign extend magic from https://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
      int64_t x = val;      // sign extend this b-bit number to r
      int64_t r;      // resulting sign-extended number
      int64_t const m = 1U << (b - 1); // mask can be pre-computed if b is fixed

      x = x & ((1U << b) - 1);  // (Skip this if bits in x above position b are already zero.)
      r = (x ^ m) - m;
      val = (uint64_t) r;
    }

    return val;
  }

  std::string StatusDisplayCell::Print(int width = -1,bool html) const
  { 
    (void) html; // casting to void to keep comiler from complaining about unused var
    const int bufferSize = 20;
    char buffer[bufferSize+1];  //64bit integer can be max 20 ascii chars (as a signed int)
    memset(buffer,' ',20);
    buffer[20] = '\0';

    //Build the format string for snprintf
    std::string fmtString("%");
    if((format.size() > 1) && (('t' == format[0]) || ('T' == format[0]))){      
      boost::char_separator<char> sep(",");
      std::string workingString = format.substr(2);
      boost::tokenizer<boost::char_separator<char> > tokenizedFormat(workingString,sep);
      //      snprintf(buffer,strlen(buffer)," ");
      uint64_t regValue = ComputeValue();
      
      for(boost::tokenizer<boost::char_separator<char> >::iterator itTok = tokenizedFormat.begin();
	  itTok != tokenizedFormat.end();
	  ++itTok){
	//check if this token contains a space
//	if(itTok->find(' ') == std::string::npos){
//	  BUException::BAD_VALUE e;
//	  std::string error("Bad format option: ");
//	  error += format;
//	  e.Append(error.c_str());
//	  throw e;
//	}
	//get the numeric value from the first part of this token
	uint64_t numericValue = strtoul(itTok->substr(0,itTok->find(' ')).c_str(),NULL,0);
	//printf("0x%016" PRIX64 " 0x%016" PRIX64 "\n",numericValue,regValue);
	if(regValue == numericValue){
	  //	  printf("0x%016" PRIX64 " 0x%016" PRIX64 " %s\n",numericValue,regValue,itTok->c_str());
	  if('t' == format[0]){
	    //Just format for 't'
	    snprintf(buffer,bufferSize,"%s",itTok->substr(itTok->find(' ')+1).c_str());
	  }else{
	    //format and number in hex for 'T'
	    snprintf(buffer,bufferSize,"%s (0x%" PRIX64 ")",itTok->substr(itTok->find(' ')+1).c_str(),regValue);
	  }
	  break;
	}
      }
    }else if((format.size() > 1) && (('m' == format[0]) || ('M' == format[0]))){      
      //Split the '_' separated values
      std::vector<uint64_t> mathValues;
      size_t iFormat = 1;
      while(mathValues.size() != 6 && iFormat < format.size()){
	if(format[iFormat] == '_'){
	  //start parsing
	  for(size_t jFormat=++iFormat;jFormat <format.size();jFormat++){
	    if((format[jFormat] == '_') || (jFormat == (format.size()-1))){
	      //convert value to number
	      if(jFormat == (format.size()-1)){
		jFormat++;
	      }
	      uint64_t val = strtoul(format.substr(iFormat,jFormat-iFormat).c_str(),NULL,0);
	      mathValues.push_back(val);
	      iFormat = jFormat;
	      break;
	    }
	  }
	}else{
	  iFormat++;
	}
      }
      //check that there are 6 values
      if(mathValues.size() != 6){
	return std::string(buffer);	
      }
      //check that no demoniator is 0
      if((mathValues[2] == 0) || (mathValues[5] == 0)){
	return std::string(buffer);
      }

      //computer the value ((m * x) + b)      
      double transformedValue = double(ComputeValue())*(double(mathValues[1])/double(mathValues[2])); //multiply by absolute value of m
      if(mathValues[0] == 0){
	//apply sign of m
	transformedValue *= -1;
      }
      
      double b = double(mathValues[4])/double(mathValues[5]);
      if(mathValues[3] == 0){
	b *= -1;
      }
      transformedValue += b;
      //print it
      snprintf(buffer,bufferSize,	       
	       "%3.2f",transformedValue);

    }else{
      if(iequals(format,std::string("x")) && ComputeValue() >= 10){
	fmtString.assign("0x%");
	if(width >= 0){
	  width -= 2;
	}
      } else if (iequals(format,std::string("x")) && ComputeValue() < 10) {
	// get rid of the leading zeros, looks better
	fmtString.assign("%");
      }
      
      //if we are specifying the width, add a *
      if(width >= 0){
	fmtString.append("*");
      }

      //add the PRI stuff for our uint64_t
      if(iequals(format,std::string("x"))){
	fmtString.append(PRIX64);
      }else if(iequals(format,std::string("d"))){
	fmtString.append(PRId64);
      }else if(iequals(format,std::string("u"))){
	fmtString.append(PRIu64);
      }
 

      //Generatethe string
      if(width == -1){      
	snprintf(buffer,bufferSize,
		 fmtString.c_str(),ComputeValue());
      }else{
	snprintf(buffer,bufferSize,	       
		 fmtString.c_str(),width,ComputeValue());
      }
    }
    //return the string
    return std::string(buffer);
  }

  void StatusDisplayCell::CheckAndThrow(std::string const & name,
			   std::string & thing1,
			   std::string const & thing2) const
  {
    //Checks
    if(thing1.size() == 0){
      thing1 = thing2;
    } else if(!iequals(thing1,thing2)) {
      BUException::BAD_VALUE e;
      e.Append(name);
      e.Append(" mismatch: "); 
      e.Append(thing1); e.Append(" != ");e.Append(thing2);
      throw e;
    }    
  }
  
  
}
