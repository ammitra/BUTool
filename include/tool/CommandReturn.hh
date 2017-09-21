#ifndef __COMMANDRETURN_HPP__
#define __COMMANDRETURN_HPP__
namespace CommandReturn{
  enum status {
    EXIT = -1,    
    NOT_FOUND = 0, 
    OK = 1,
    BAD_ARGS = 2
  };
}
#endif
