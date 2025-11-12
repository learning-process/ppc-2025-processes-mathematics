#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace kulik_a_the_most_different_adjacent {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace kulik_a_the_most_different_adjacent
