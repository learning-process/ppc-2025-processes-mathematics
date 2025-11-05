#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace lopatin_a_scalar_mult {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace lopatin_a_scalar_mult
