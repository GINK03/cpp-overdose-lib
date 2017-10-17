#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <regex>
#include <map>
#include <algorithm> 
#include <iterator>
#include <future>
#include <chrono>
#include <queue>
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
  template<typename SOURCE, typename TARGET>
  auto zip(std::vector<TARGET> target)  {
    return [target](const std::vector<SOURCE>& source) {
      std::vector<std::pair<SOURCE,TARGET>> ret;
      for(int i=0; i < source.size() && i < target.size(); i++) {
        try{
          auto key = source[i];
          auto val = target[i];
          ret.push_back( std::make_pair(key, val) );
        } catch(const std::exception& ex) {
          continue;
        }
      }
      return ret;
    };
  };  
  template<typename T>
  auto sum() {
    return [](std::vector<T> source) {
      auto y = std::decay_t<decltype(source[0])>();
      for(auto s: source) {
        y += s;
      }
      return y;
    };
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
        RETURN r = f(index, s); 
        tmp.push_back(std::make_tuple( index, r));
        index++;
      }
      return tmp;
    };
  };
  
  namespace concurrent {
    template<typename SOURCE, typename RETURN, typename FUNCTOR>
    auto mapper(const FUNCTOR& functor) {
      int cpu_num = std::thread::hardware_concurrency();
      return [cpu_num, functor](const std::vector<SOURCE>& source) {
        std::queue<SOURCE> que; 
        // build dataset at first.
        for(SOURCE s:source) que.push(s);
        // result onmemory-store
        std::vector<RETURN> returns;
        // task scheduler.
        std::map<int, std::optional<std::future<RETURN>>> workers_table; 
        for (int i = 0; i < cpu_num; ++i) {
          workers_table[i] = std::optional( std::async(std::launch::async, functor, que.front() ) ); 
          que.pop(); 
          if(que.size() == 0) 
            break;
        }
        while( [&](){ return true; }() ) {
          int cpu_counter = 0;
          for (auto& [index, element] : workers_table) {
            if( element != std::nullopt ) { 
              auto status = element->wait_for(std::chrono::seconds(1));
              if (status == std::future_status::deferred) {
              } else if (status == std::future_status::timeout) {
              } else if (status == std::future_status::ready) {
                auto value = element->get();
                // push result to onmemory-store
                returns.push_back( value );
                if(que.size() == 0) {
                  workers_table[index] = std::nullopt;
                } else {
                  workers_table[index] = std::optional( std::async(std::launch::async, functor, que.front()) );
                  que.pop();
                }
              } 
            } else {
              cpu_counter += 1; 
            } // end std::optional if
          } // end for
          // 空きスロットが全てのCPUで発生したら終了
          if( cpu_counter == cpu_num ) break;
        }
        return returns;
      };
    };
  };//end concurrent
  template<typename SOURCE, typename TARGET, typename F>
  auto reducer(const TARGET& target, const F& f) {
    return [target, f](std::vector<SOURCE> source) {
      TARGET res = TARGET(target);
      for(SOURCE s:source) {
        f(res, s); 
      }
      return res;
    };
  }
  
  template<typename INPUT, typename OUTPUT, typename FUNCTOR>
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

  template<typename KEY, typename RETURN, typename FUNCTOR>
  auto groupBy(const FUNCTOR& f) {
    // functorはキー生成用
    return [f](std::vector<RETURN> source) {
      std::map<KEY, std::vector<RETURN>> tmp; 
      for(RETURN s:source) {
        KEY key = f(s);
        if( tmp.find(key) == tmp.end() ) tmp[key] = std::vector<RETURN>();
        tmp[key].push_back(s);
      }
      std::vector<std::tuple<KEY, std::vector<RETURN>>> ret;
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
  
  template<typename KEY, typename INPUT, typename FUNCTOR>
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

  template<typename KEY, typename INPUT, typename FUNCTOR>
  auto sortBy(const FUNCTOR& f) {
    // functorはキー生成用
    return [f](const std::vector<INPUT>& inputs) {
      std::vector<std::tuple<KEY, INPUT>> target; 
      //std::copy(inputs.begin(), inputs.end(), std::back_inserter(target));
      for(INPUT input:inputs) {
        KEY key = f(input);
        target.push_back( std::make_pair(key, input) );
      }
      std::sort(target.begin(), target.end(), [](auto& a, auto& b){ return std::get<0>(a) < std::get<0>(b);});

      std::vector<INPUT> ret;
      for(auto [key,value]:target) ret.push_back( value );
      return ret;
    };
  }
};
