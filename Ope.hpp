#include <functional>

template <typename LT, typename RT, typename L, typename R>
std::function<RT(LT)> operator*(const L& l, const R& r) {
  auto functor = [l,r](LT lt){ 
    RT rt = r(l(lt));
    return rt;
  };
  return functor;
}

template <typename LT, typename RT, typename L, typename R>
std::function<RT(LT)> merge(const L& l, const R& r) {
  auto functor = [l,r](LT lt){ 
    RT rt = r(l(lt));
    return rt;
  };
  return functor;
}
