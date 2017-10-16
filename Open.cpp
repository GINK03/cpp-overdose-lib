#include <fstream>
#include <string>
#include "Vec.hpp"
namespace OVERDOSE { 
Vec<std::string> Open(const std::string& name) { 
  std::ifstream file(name);
  std::string line; 
  Vec<std::string> vec;
  while (std::getline(file, line)) {
    vec.push_back(line);
  }
  return vec;
}

}; // end OVERDOSE
