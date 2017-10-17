#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <regex>
#include <map>
#include <algorithm> 
#include <iterator>
#include "List.hpp"
#pragma once
namespace OVERDOSE_EXT {
  template<typename F> 
  auto operator>>(const std::string& str, const F& f) { 
    return f(str); 
  }
  template<typename F> 
  void operator%(int source, const F& f) { return f(source); }

  template<typename T>
  void echo1(const T& source) { std::cout << source << std::endl; };  
  void echo1(const int& source) { std::cout << source << std::endl; };  

  template<typename F> 
  auto operator>>(const double& source, const F& f) { 
    return f(source); 
  }
  template<typename F> 
  auto operator>>(const std::vector<std::string>& source, const F& f) { 
    return f(source); 
  }
  template<typename U, typename F> 
  auto operator>>(const std::vector<U>& source, const F& f) { 
    return f(source); 
  }
  std::function< std::vector<std::string>(std::string) > split = [](std::string source) {
    std::vector<std::string> result;
    std::regex reg{" "};
    std::copy(
      std::sregex_token_iterator(source.begin(), source.end(), reg, -1),
      std::sregex_token_iterator(),
      std::back_inserter(result)
    );
    return result;
  };

  auto splitWith(const std::string& delimiter) {
    return [delimiter](std::string source){
      std::vector<std::string> result;
      std::regex reg{delimiter};
      std::copy(
        std::sregex_token_iterator(source.begin(), source.end(), reg, -1),
        std::sregex_token_iterator(),
        std::back_inserter(result)
      );
      return result;
    };
  };  

  template<typename SOURCE>
  auto echo() {
    return [](const std::vector<SOURCE>& source) {
      std::vector<std::string> tmp; 
      for (auto s: source) {
        if constexpr ( std::is_same<SOURCE, std::string>{} ) {
          tmp.push_back(s);
        } else {
          tmp.push_back(std::to_string(s));
        }
        tmp.push_back(",");
      }
      tmp.pop_back();
      std::cout << "[";
      for( auto str:tmp) std::cout << str;
      std::cout << "]" << std::endl;
    };
  };  
  
  template<typename SOURCE, typename TARGET>
  auto zipMap(std::vector<TARGET> target)  {
    return [target](const std::vector<SOURCE>& source) {
      std::map<SOURCE,TARGET> ma;
      for(int i=0; i < source.size() && i < target.size(); i++) {
        try{
          auto key = source[i];
          auto val = target[i];
          ma[key] = val;
        } catch(const std::exception& ex) {
          continue;
        }
      }
      return ma;
    };
  };  
  template<typename T>
  T sum(const std::vector<T>& source) {
    auto y = std::decay_t<decltype(source[0])>();
    for(auto s: source) {
      y += s;
    }
    return y;
  };  

  template<typename SOURCE, typename RETURN, typename FUNCTOR>
  auto mapper(const FUNCTOR& f) {
    return [f](std::vector<SOURCE> source) {
      std::vector<RETURN> tmp; 
      for(auto s:source) {
        RETURN r = f(s); 
        tmp.push_back(r);
      }
      return tmp;
    };
  };
  
  template<typename SOURCE, typename RETURN, typename FUNCTOR>
  auto mapperIndexed(const FUNCTOR& f) {
    return [f](std::vector<SOURCE> source) {
      std::vector<std::tuple<int, RETURN>> tmp; 
      int index = 0;
      for(auto s:source) {
        RETURN r = f(s); 
        tmp.push_back(std::make_tuple( index, r));
        index++;
      }
      return tmp;
    };
  };

  template<typename R, typename F>
  auto reducer(const R& r, const F& f) {
    return [r, f](std::vector<R> source) {
      R res = R(r);
      for(auto s:source) {
        f(res, s); 
      }
      return res;
    };
  }
  
  template<typename INPUT, typename FUNCTOR>
  auto filter(const FUNCTOR& f) {
    return [f](const std::vector<INPUT>& source) {
      std::vector<INPUT> res;
      for(INPUT s:source) {
        if( f(s) == true)
          res.push_back(s);
      }
      return res;
    };
  }

  template<typename KEY, typename RET, typename F>
  auto groupBy(const F& f) {
    return [f](std::vector<RET> source) {
      std::map<KEY, std::vector<RET>> tmp; 
      for(RET s:source) {
        KEY key = f(s);
        if( tmp.find(key) == tmp.end() ) tmp[key] = std::vector<RET>();
        tmp[key].push_back(s);
      }
      std::vector<std::tuple<KEY, std::vector<RET>>> ret;
      for( auto [key, vec] : tmp) ret.push_back( std::make_pair(key, vec) );
      return ret;
    };
  }

  template<typename R,typename F>
  auto let(const F& f)  { 
    //using Decl = decltype(f());
    return [f](const R& r) {
      return f(r);
    };
  }

  template<typename INPUT>
  auto joinToString(const std::string& str) {
    return [str](const std::vector<INPUT>& inputs) {
      std::vector<std::string> buff;
      for(INPUT input:inputs) {
        buff.push_back(std::to_string(input));
        buff.push_back(str);
      }
      buff.pop_back();
      std::string mozaic = "";
      for(auto b:buff) mozaic += b;
      return mozaic;
    };
  }

  template<typename INPUT>
  auto max() {
    return [](const std::vector<INPUT>& inputs) {
      INPUT buff = inputs[0];
      for(INPUT input:inputs) {
        if( input > buff ) buff = input;
      }
      return buff;
    };
  }
  
  template<typename INPUT>
  auto min() {
    return [](const std::vector<INPUT>& inputs) {
      INPUT buff = inputs[0];
      for(INPUT input:inputs) {
        if( input < buff ) buff = input;
      }
      return buff;
    };
  }
  
  template<typename INPUT, typename OUTPUT=double>
  auto mean() {
    return [](const std::vector<INPUT>& inputs) {
      INPUT buff = inputs[0];
      int size = inputs.size();
      for(INPUT input:inputs) buff += input;
      return OUTPUT(buff)/size;
    };
  }
  
  template<typename INPUT, typename OUTPUT=double>
  auto flatten() {
    return [](const std::vector<std::vector<INPUT>>& inputs) {
      std::vector<INPUT> tmp;
      for(const std::vector<INPUT> input:inputs) {
        for(const INPUT entry:input) tmp.push_back(entry);
      }
      return tmp; 
    };
  }
  
  template<typename INPUT, typename KEY, typename FUNCTOR>
  auto distinct(const FUNCTOR& f) {
    return [f](const std::vector<INPUT>& inputs) {
      std::map<KEY, INPUT> ma;
      for(auto input:inputs) {
        auto key = f(input);
        ma[key] = input;
      }
      std::vector<INPUT> tmp;
      for(auto [key, val]: ma) {
        tmp.push_back(val);
      }

      return tmp; 
    };
  }

  template<typename INPUT, typename FUNCTOR>
  auto sortBy(const FUNCTOR& f) {
    return [f](const std::vector<INPUT>& inputs) {
      std::vector<INPUT> target; std::copy(inputs.begin(), inputs.end(), std::back_inserter(target));
      std::sort(target.begin(), target.end(), f);
      return target;
    };
  }
};
