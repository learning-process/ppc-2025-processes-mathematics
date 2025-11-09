#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace krymova_k_lex_order {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace krymova_k_lex_order
