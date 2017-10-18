#include <string>
#include <iostream>
#include "Arrow.hpp"
#include <string.h>
#pragma once
namespace OVERDOSE {
  namespace SERIAL { 
    static std::string SEPARATOR = "<Right>";

    template <class T0, class T>
    std::string dump(const T* t, const T0* t0) {
      int64_t basePosition = reinterpret_cast<int64_t>(t0);
      int64_t memoryPosition = reinterpret_cast<int64_t>(t);
      auto p = reinterpret_cast<unsigned char const*>(t);
      std::ostringstream stream;
      if constexpr (std::is_same_v<T, std::string>) {
        stream << t->c_str();
      } else {
        for (size_t n = 0 ; n < sizeof(T) ; ++n) 
          stream << std::setw(2) << std::setfill('0') << int(p[n]) << " ";
      }
      std::string type;
      if constexpr (std::is_same_v<T, int>) {
        type = "int";
      }
      if constexpr (std::is_same_v<T, double>) {
        type = "double";
      }
      if constexpr (std::is_same_v<T, std::string>) {
        type = "std::string";
      }
      std::ostringstream result_stream;
      result_stream  << type << "<Left>" <<  memoryPosition-basePosition  << "<Left>" << stream.str() << "<Left>" << sizeof(T);
    
      return result_stream.str();
    }
  }; // end SERIAL
  namespace DESERIAL {
    namespace OD=OVERDOSE;
    //lexical recovery
    template<typename RETURN>
    RETURN recover(std::string input) {
      //std::string input = "int<Left>0<Left>01 00 00 00 <Left>4<Right>double<Left>8<Left>00 00 00 00 00 00 00 64 <Left>8<Right>std::string<Left>16<Left>Hoge<Left>32";
      std::vector<std::string> each = OD::Split(input, "<Right>");

      RETURN ret;
      for(auto e:each) {
        std::vector<std::string> fields = OD::Split(e,"<Left>");

        auto type  = fields[0];
        auto posi  = int64_t(stoi(fields[1]));
        auto rawVec = OD::Split(fields[2], " ");
        auto size = std::stoi(fields[3]);
        
        if ( type == "int" ) {
          char charStack[rawVec.size()-2];
          int cur = 0;
          for( auto raw: rawVec) {
            charStack[cur] = char(stoi(raw));
            cur ++;
          }
          int rec = *(int*)(&charStack);   
          *(int*)(int64_t(&ret) + posi) = rec; // recover here
        }
        if ( type == "double" ) {
          char charStack[rawVec.size()-1];
          int cur = 0;
          for( auto raw: rawVec) {
            charStack[cur] = char(stoi(raw));
            cur ++;
          }
          double rec = *(double*)(&charStack);   
          *(double*)(int64_t(&ret) + posi) = rec; // recover here
        }
        if ( type == "std::string" ) {
          std::string rec = std::string(fields[2]);
          *(std::string*)(int64_t(&ret) + posi) = rec; // recover here
        }

      }
      return ret;
    }
  }; // end DESERIAL

};// OVERDOSE
