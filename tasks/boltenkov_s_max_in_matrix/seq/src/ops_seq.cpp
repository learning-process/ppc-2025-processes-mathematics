#include "boltenkov_s_max_in_matrix/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "boltenkov_s_max_in_matrix/common/include/common.hpp"
#include "util/include/util.hpp"

namespace boltenkov_s_max_in_matrix {

BoltenkovSMaxInMatrixkSEQ::BoltenkovSMaxInMatrixkSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::numeric_limits<double>::lowest();
}

bool BoltenkovSMaxInMatrixkSEQ::ValidationImpl() {
  return std::abs(GetOutput() + 1.7e+308) < 1e-14 &&
         std::get<0>(GetInput()) > 0 && !std::get<1>(GetInput()).empty() &&
         std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
}

bool BoltenkovSMaxInMatrixkSEQ::PreProcessingImpl() {
  return std::get<0>(GetInput()) > 0 && !std::get<1>(GetInput()).empty() &&
         std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
}

bool BoltenkovSMaxInMatrixkSEQ::RunImpl() {
  if (!(std::get<0>(GetInput()) > 0 && !std::get<1>(GetInput()).empty() &&
        std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0)) {
    return false;
  }

  OutType &mx = GetOutput();
  std::vector<double> &v = std::get<1>(GetInput());

  int n = v.size();
  bool flag;

  for (int i = 0; i < n; ++i) {
    flag = v[i] > mx;
    mx = static_cast<double>(flag) * v[i] +
         (1. - static_cast<double>(flag)) * mx;
  }

  return std::abs(GetOutput() + std::numeric_limits<double>::lowest()) > 1e-14;
}

bool BoltenkovSMaxInMatrixkSEQ::PostProcessingImpl() {
  return std::abs(GetOutput() + std::numeric_limits<double>::lowest()) > 1e-14;
}

} // namespace boltenkov_s_max_in_matrix
