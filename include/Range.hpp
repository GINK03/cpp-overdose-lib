#include <vector>
// Python2のRagneに該当する処理を実装

namespace OVERDOSE {
  template <typename U>
  std::vector<U> Range(const U& u1, const U& u2) {
    std::vector<U> v;
    for(U us = u1; us < u2; us++) {
      v.push_back(us);
    }
    return v;
  }
};
