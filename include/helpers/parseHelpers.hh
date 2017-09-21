#include <vector>
#include <string>
#include <utility> //std::pair

std::vector<std::string> splitString(std::string const & line,
				     std::string const & separator = ",");

std::vector<std::pair<size_t,size_t> > parseRange(std::string const & line);

std::vector<size_t> parseList(std::string const & line);
