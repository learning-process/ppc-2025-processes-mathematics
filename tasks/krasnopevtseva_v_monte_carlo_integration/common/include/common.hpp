#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace krasnopevtseva_v_monte_carlo_integration {

using InType = std::tuple<double, double, int>;
using OutType = double;
using TestType = std::tuple<std::tuple<double, double, int>, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace krasnopevtseva_v_monte_carlo_integration
