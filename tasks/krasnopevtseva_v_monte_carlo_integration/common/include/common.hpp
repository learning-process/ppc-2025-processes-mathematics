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
  kCOS_X3 = 0,  // cos(x) * x^3
  kSIN_X2 = 1,  // sin(x) * x^2
  kEXP_X = 2,   // exp(-x) * x
  kPOLY_X = 3   // x^4 - 2*x^2 + 1
};

class FuncSystem {
 public:
  static double GetFunc(int index, double x) {
    switch (static_cast<FuncIndex>(index)) {
      case FuncIndex::kCOS_X3:
        return std::cos(x) * x * x * x;
      case FuncIndex::kSIN_X2:
        return std::sin(x) * x * x;
      case FuncIndex::kEXP_X:
        return std::exp(-x) * x;
      case FuncIndex::kPOLY_X:
        return (x * x * x * x) - (2 * x * x) + 1;
      default:
        return std::cos(x) * x * x * x;
    }
  }

  static double AnalyticIntegral(int index, double a, double b) {
    switch (static_cast<FuncIndex>(index)) {
      case FuncIndex::kCOS_X3:
        return AnalyticCosX3(a, b);
      case FuncIndex::kSIN_X2:
        return AnalyticSinX2(a, b);
      case FuncIndex::kEXP_X:
        return AnalyticExpX(a, b);
      case FuncIndex::kPOLY_X:
        return AnalyticPolyX(a, b);
      default:
        return AnalyticCosX3(a, b);
    }
  }

 private:
  static double FCos(double x) {
    return ((x * x * x - 6 * x) * std::sin(x)) + ((3 * x * x - 6) * std::cos(x));
  }
  static double AnalyticCosX3(double a, double b) {
    return FCos(b) - FCos(a);
  }

  static double FSin(double x) {
    return (2 * std::sin(x) * x) - ((x * x - 2) * std::cos(x));
  }

  static double AnalyticSinX2(double a, double b) {
    return FSin(b) - FSin(a);
  }

  static double FExp(double x) {
    return -(x + 1) * std::exp(-x);
  }
  static double AnalyticExpX(double a, double b) {
    return FExp(b) - FExp(a);
  }

  static double f_poly(double x) {
    return ((x * x * x * x * x) / 5.0) - ((2 * x * x * x) / 3.0) + x;
  }

  static double AnalyticPolyX(double a, double b) {
    return f_poly(b) - f_poly(a);
  }
};

}  // namespace krasnopevtseva_v_monte_carlo_integration
