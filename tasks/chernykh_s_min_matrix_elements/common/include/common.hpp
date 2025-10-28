#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace chernykh_s_min_matrix_elements {

using InType = std::tuple<size_t,size_t,std::vector<double>>;
using OutType = double;
using TestType = std::string;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace chernykh_s_min_matrix_elements
