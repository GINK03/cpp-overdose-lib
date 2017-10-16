#include <vector>
#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#pragma once
template <typename T>
class Vec : public std::vector<T> {
  public:
  std::vector<T> super;
  Vec() {
  }
  Vec(std::vector<T>& ts) 
    : super(ts) {
    for(auto t : ts) {
    //  this.push_back(t);
    }
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
  Vec<I> map(const F& f) {
    std::vector<I> tmps;
    for(auto it: super) {
      //tmps.push_back( f(it) );
      tmps.push_back( f(it) );
    }
    return Vec<I>(tmps);
  }
  template <typename F>
  Vec<T> filter(const F& f) {
    std::vector<T> tmps;
    for(auto it: super) {
      //tmps.push_back( f(it) );
      if( f(it) == true ) {
        tmps.push_back(it);
      }
    }
    return Vec<T>(tmps);
  }
  
  template <typename F>
  Vec<T> orderBy(const F& f) {
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
};

