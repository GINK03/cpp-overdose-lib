#include <vector>
#include <string>
#include <iostream>
#include <tuple>
#include "Arrow.hpp"
#include "Range.hpp"

namespace OD=OVERDOSE;
using namespace OVERDOSE_EXT;
using namespace std;
int main() {
  auto vs = OD::Range(1,10000);
  vs >> mapper<int,string>( [](int i){
    if(i%15 == 0)  return "FizzBuzz";
    else if(i%3 == 0) return "Fizz";
    else if(i%5 == 0) return "Buzz";
    else return "Other";
  }) >> groupBy<string, string>( [](auto str) {
    return str;
  }) >> mapper<tuple<string,vector<string>>, int>( [](auto tup) {
    auto [key,vals] = tup;
    cout << "KEY: " << key << " VALUE_SIZE: " << vals.size() << endl;
    return 0;
  });
}
