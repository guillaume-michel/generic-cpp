#include <iostream>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <tuple>
#include <cstddef>
#include <stdexcept>
#include <any>
#include <cmath>

template<typename T>
struct typeOf;

template <typename F>
struct fInfo : fInfo<decltype(&F::operator())> { };

template <typename F, typename Ret, typename... Args>
struct fInfo<Ret(F::*)(Args...)const> {
    static const size_t arity = sizeof...(Args);
    using return_type = Ret;
    using argument_types = std::tuple<Args...>;
};

template<typename F>
constexpr size_t arity_v = fInfo<F>::arity;

template<typename F>
using return_type = typename fInfo<F>::return_type;

template<size_t N, typename F>
using argument_type = typename std::decay<decltype(std::get<N>(std::declval<typename fInfo<F>::argument_types>()))>::type;

template<size_t N, typename R>
struct Generic;

template<typename R>
struct Generic<1, R>{

  R operator()(std::any arg1) {
    const std::string tname = arg1.type().name();

    if (vtable.find(tname) != std::end(vtable)) {
      return vtable[tname](arg1);
    } else {
      // did not find method for current type
      throw std::runtime_error(std::string("Could not find method for type: ") + tname);
    }
  }

  std::unordered_map<std::string, std::function<R(std::any)>> vtable;
};

template<typename R>
struct Generic<2, R>{

  R operator()(std::any arg1, std::any arg2) {
    const std::string tname1 = arg1.type().name();
    const std::string tname2 = arg2.type().name();

    // dispatch on first argument then second
    if (vtable.find(tname1) != std::end(vtable) &&
        vtable[tname1].find(tname2) != std::end(vtable[tname1])) {
        return vtable[tname1][tname2](arg1, arg2);
    } else {
      // did not find method for current type
      throw std::runtime_error(std::string("Could not find method for types: ") + tname1 + " - " + tname2);
    }
  }

  std::unordered_map<std::string, std::unordered_map<std::string, std::function<R(std::any, std::any)>>> vtable;
};

template<size_t N, typename R>
auto defgeneric() {
  return Generic<N, R>{};
}

template<size_t N, typename R, typename Implementation>
void defmethod(Generic<N, R>& generic, Implementation&& implementation) {
  static_assert(N == arity_v<Implementation>, "Given implementation has wrong arity");
  static_assert(std::is_same<R, return_type<Implementation>>::value, "Given implementation has wrong return type");

  if constexpr (N == 1) {
      using Arg1 = argument_type<0, Implementation>;
      generic.vtable.insert(
          std::make_pair<std::string,
          std::function<R(std::any)>>(typeid(std::declval<Arg1>()).name(),
                                      [implementation = std::forward<Implementation>(implementation)](std::any value) {
                                        return implementation(std::any_cast<Arg1>(value));
                                      }));
  } else if constexpr (N == 2) {
      using Arg1 = argument_type<0, Implementation>;
      using Arg2 = argument_type<1, Implementation>;
      generic.vtable[typeid(std::declval<Arg1>()).name()].insert(
          std::make_pair<std::string,
          std::function<R(std::any, std::any)>>(typeid(std::declval<Arg2>()).name(),
                                                [implementation = std::forward<Implementation>(implementation)](std::any value1,
                                                                                                                std::any value2) {
                                                  return implementation(std::any_cast<Arg1>(value1),
                                                                        std::any_cast<Arg2>(value2));
                                                }));
  } else {
    static_assert(N>2, "defmethod do not support arity > 2");
  }
}

//----------------------------------------------------------------------
struct Circle {
  float radius;
};

struct Square {
  float length;
};

// with language support:
//
// generic void draw(auto shape);
//
// generic float perimeter(auto shape);
//
// method void draw(Circle c) {
//   std::cout << "Draw Circle" << std::endl;
// }
//
// method void draw(Square c) {
//   std::cout << "Draw Square" << std::endl;
// }
//
// method float perimeter(Circle c) {
//   return 2 * (float)M_PI * c.radius;
// }
//
// method float perimeter(Square c) {
//   return 4 * c.length;
// }

// defines a generic method with 1 argument called "draw" that returns void
auto draw = defgeneric<1, void>();

// defines a generic method with 1 argument called "perimeter" that return a float
auto perimeter = defgeneric<1, float>();

// defines a generic method with 2 arguments called "intersection" that returns void
auto intersection = defgeneric<2, void>();


int main(int argc, char* argv[]) {

  // method draw for Circle
  defmethod(draw,
            [](Circle c) {
              std::cout << "Draw Circle" << std::endl;
            });

  // method draw for Square
  defmethod(draw,
            [](Square c) {
              std::cout << "Draw Square" << std::endl;
            });

  // method perimeter for Circle
  defmethod(perimeter,
            [](Circle c) {
              return 2 * (float)M_PI * c.radius;
            });

  // method perimeter for Square
  defmethod(perimeter,
            [](Square c) {
              return 4 * c.length;
            });

  // method intersection for Circle - Circle
  defmethod(intersection,
            [](Circle c1, Circle c2) {
              std::cout << "Intersection of Circle with Circle" << std::endl;
            });

  // method intersection for Square - Circle
  defmethod(intersection,
            [](Square s, Circle c) {
              std::cout << "Intersection of Square with Circle" << std::endl;
            });

  // method intersection for Circle - Square
  defmethod(intersection,
            [](Circle c, Square s) {
              std::cout << "Intersection of Circle with Square" << std::endl;
              //intersection(s, c);
            });

  // method intersection for Square - Square
  defmethod(intersection,
            [](Square s1, Square s2) {
              std::cout << "Intersection of Square with Square" << std::endl;
            });

  std::vector<std::any> shapes = {
    Circle{1.0f},
    Square{1.0f},
  };

  for (auto shape : shapes) {
    draw(shape);
    std::cout << "perimeter: " << perimeter(shape) << std::endl;
    intersection(shape, Circle{2.0f});
    intersection(shape, Square{2.0f});
  }

  return 0;
}
