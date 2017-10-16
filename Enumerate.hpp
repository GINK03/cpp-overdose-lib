#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <regex>
namespace OVERDOSE {
  template <typename U> 
  auto Enumerate(const U& us) {
    int counter = 0;
    using I = std::decay_t<decltype(us[0])>;
    std::vector<std::tuple<int, I>> tmp;
    for(I u:us) {
      //std::cout << u << std::endl;
      tmp.push_back(std::make_tuple(counter, I(u)) ); 
      counter ++;
    }
    return tmp;
  }
};
