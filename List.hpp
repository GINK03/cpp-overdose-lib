#include <vector>
#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <list>
#include <initializer_list>
#include <set>
#include <map>

#pragma once

namespace OVERDOSE { 

template <typename T>
class List : public std::list<T> {
  public:
  std::list<T> super;
  List():super() {
  }
  List(std::list<T>& ts) 
    : super(ts) {
    for(auto t : ts) {
    //  this.push_back(t);
    }
  }
  List(std::initializer_list<T> il) {
    for( auto i:il ) {
      super.push_back(i);
    }
  }
  List(const std::vector<T>& v) {
    for( auto x:v) {
      super.push_back(x);
    }
  }
  
  void push_back(const T& t) {
    super.push_back(t);
  }

  void print() {
    for(auto it:super) {
      std::cout << it;
    }
    std::cout << std::endl;
  }
  void print(std::string sep) {
    for(auto it:super) {
      std::cout << it << sep;
    }
    std::cout << std::endl;
  }
  template <typename F>
  void test(const F& f) {
    f();
  }

  template <typename I, typename F>
  List<I> map(const F& f) {
    std::list<I> tmps;
    for(auto it: super) {
      //tmps.push_back( f(it) );
      tmps.push_back( f(it) );
    }
    return List<I>(tmps);
  }
  template <typename F>
  List<T> filter(const F& f) {
    std::list<T> tmps;
    for(auto it: super) {
      //tmps.push_back( f(it) );
      if( f(it) == true ) {
        tmps.push_back(it);
      }
    }
    return List<T>(tmps);
  }
  
  template <typename F>
  List<T> orderBy(const F& f) {
    std::sort(
      super.begin(),
      super.end(),
      f
    );
    return *this;
  }
  template <typename Y, typename F>
  Y reduce(const Y& y, const F& f) {
    Y yt = Y(y);
    for(auto it: super) {
      //tmps.push_back( f(it) );
      f(yt, it);
    }
    return yt;
  }
  template <typename Y>
  auto toSet() {
    //using Y = std::decay_t<decltype(*super.begin())>;
    std::list<Y> tmp;
    //  std::cout << "aaaa" << super.size() << std::endl;
    for(auto x : super) {
      //std::cout << x << std::endl;
      tmp.push_back(x);
    }
    tmp.sort();
    tmp.unique();
    return List<Y>(tmp);
  }
  
  // Fはkeyを取り出す関数
  // Yはkeyの型
  // Selfはvalueのデータコンテナの型
  template <typename Y, typename F>
  auto groupBy(const F& f) {
    //using Y = std::decay_t<decltype(f())>;
    using Self = std::decay_t<decltype(*this)>;
    std::map<Y,Self> m;
    for(auto s: super) {
      Y key = f(s);     
      m[key] = Self{};
    }
    for(auto s: super) {
      Y key = f(s);
      //std::cout << "Debug groupBy " << s << std::endl;
      m[key].push_back(decltype(s)(s));
    }
    std::list<std::tuple<Y, Self>> tmp;
    for(auto [k,v] : m) {
      tmp.push_back( std::make_tuple(k, v) );
    }

    return List<std::tuple<Y, Self>>(tmp);
  }
};

} // end OVERDOSE
