#include "krasnopevtseva_v_monte_carlo_integration/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>
#include <random>
#include <cmath>
#include <cstddef>
#include <tuple>
#include "krasnopevtseva_v_monte_carlo_integration/common/include/common.hpp"
#include "util/include/util.hpp"

namespace krasnopevtseva_v_monte_carlo_integration {

KrasnopevtsevaV_MCIntegrationSEQ::KrasnopevtsevaV_MCIntegrationSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KrasnopevtsevaV_MCIntegrationSEQ::ValidationImpl() {
  const auto &input = GetInput();
  double a = std::get<0>(input);
  double b = std::get<1>(input);
  int num_points = std::get<2>(input);
  
  return (a <= b) && (num_points > 0);
}

bool KrasnopevtsevaV_MCIntegrationSEQ::PreProcessingImpl() {
  GetOutput() = 0.0;
  return true;
}

bool KrasnopevtsevaV_MCIntegrationSEQ::RunImpl() {
  const auto &input = GetInput();
  double a = std::get<0>(input);
  double b = std::get<1>(input);
  int num_points = std::get<2>(input);

  if (a >= b || num_points <= 0) {
    return false;
  }

  double sum = 0.0;
  
  // генератор вихрь Мерсенна
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis(a, b);
  
  // cos(x)*x^3
  for (int i = 0; i < num_points; i++) {
    double x = dis(gen); 
    double fx = std::cos(x) * x * x * x;  
    sum += fx;
  }
  
  double integral = (b - a) * sum / num_points;
  GetOutput() = integral;
  
  return true;
}

bool KrasnopevtsevaV_MCIntegrationSEQ::PostProcessingImpl() {
   return true;
}

}  // namespace krasnopevtseva_v_monte_carlo_integration
