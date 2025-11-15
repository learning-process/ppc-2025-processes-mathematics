#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace kapanova_s_min_of_matrix_elements {

using InType = std::vector<std::vector<int>>;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace kapanova_s_min_of_matrix_elements
