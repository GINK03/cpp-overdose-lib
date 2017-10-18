#include "Split.hpp"
#include <vector>
#include <string>
#include <regex>
namespace OVERDOSE {
  std::vector<std::string> Split(const std::string& source, const std::string& pattern ) {
    std::vector<std::string> result;
    std::regex reg{pattern};
    std::copy(
      std::sregex_token_iterator(source.begin(), source.end(), reg, -1),
      std::sregex_token_iterator(),
      std::back_inserter(result)
    );
    return result; 
  }
};
