#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace krasnopevtseva_v_monte_carlo_integration {

using InType = std::tuple<double, double, int, int>;
using OutType = double;
using TestType = std::tuple<std::tuple<double, double, int, int>, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

enum class FuncIndex : int {
  COS_X3 = 0,  // cos(x) * x^3
  SIN_X2 = 1,  // sin(x) * x^2
  EXP_X = 2,   // exp(-x) * x
  POLY_X = 3   // x^4 - 2*x^2 + 1
};

class FuncSystem {
 public:
  static double getFunc(int index, double x) {
    switch (static_cast<FuncIndex>(index)) {
      case FuncIndex::COS_X3:
        return std::cos(x) * x * x * x;
      case FuncIndex::SIN_X2:
        return std::sin(x) * x * x;
      case FuncIndex::EXP_X:
        return std::exp(-x) * x;
      case FuncIndex::POLY_X:
        return (x * x * x * x) - (2 * x * x) + 1;
      default:
        return std::cos(x) * x * x * x;
    }
  }

  static double analyticIntegral(int index, double a, double b) {
    switch (static_cast<FuncIndex>(index)) {
      case FuncIndex::COS_X3:
        return analyticCosX3(a, b);
      case FuncIndex::SIN_X2:
        return analyticSinX2(a, b);
      case FuncIndex::EXP_X:
        return analyticExpX(a, b);
      case FuncIndex::POLY_X:
        return analyticPolyX(a, b);
      default:
        return analyticCosX3(a, b);
    }
  }

  static std::string getName(int index) {
    switch (static_cast<FuncIndex>(index)) {
      case FuncIndex::COS_X3:
        return "cos(x)*x^3";
      case FuncIndex::SIN_X2:
        return "sin(x)*x^2";
      case FuncIndex::EXP_X:
        return "exp(-x)*x";
      case FuncIndex::POLY_X:
        return "x^4-2x^2+1";
      default:
        return "cos(x)*x^3";
    }
  }

 private:
  static double f_cos(double x) {
    return ((x * x * x - 6 * x) * std::sin(x)) + ((3 * x * x - 6) * std::cos(x));
  }
  static double analyticCosX3(double a, double b) {
    return f_cos(b) - f_cos(a);
  }

  static double f_sin(double x) {
    return (2 * std::sin(x) * x) - ((x * x - 2) * std::cos(x));
  }

  static double analyticSinX2(double a, double b) {
    return f_sin(b) - f_sin(a);
  }

  static double f_exp(double x) {
    return -(x + 1) * std::exp(-x);
  }
  static double analyticExpX(double a, double b) {
    return f_exp(b) - f_exp(a);
  }

  static double f_poly(double x) {
    return ((x * x * x * x * x) / 5.0) - ((2 * x * x * x) / 3.0) + x;
  }

  static double analyticPolyX(double a, double b) {
    return f_poly(b) - f_poly(a);
  }
};

}  // namespace krasnopevtseva_v_monte_carlo_integration
