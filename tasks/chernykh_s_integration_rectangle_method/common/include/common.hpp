#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace chernykh_s_integration_rectangle_method {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace chernykh_s_integration_rectangle_method
