#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "task/include/task.hpp"

namespace yusupkina_m_elem_vec_sum {

using InType = std::vector<int>;
using OutType = long long;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace yusupkina_m_elem_vec_sum
