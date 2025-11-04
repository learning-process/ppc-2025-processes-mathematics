#include "barkalova_m_min_val_matr/seq/include/ops_seq.hpp"

#include <climits>
#include <vector>

#include "barkalova_m_min_val_matr/common/include/common.hpp"
#include "util/include/util.hpp"

namespace barkalova_m_min_val_matr {

BarkalovaMMinValMatrSEQ::BarkalovaMMinValMatrSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool BarkalovaMMinValMatrSEQ::ValidationImpl() {
  const auto &matrix = GetInput();
  if (matrix.empty()) {
    return false;
  }
  size_t stolb = matrix[0].size();
  // все строки должны иметь одинак разм
  for (const auto &row : matrix) {
    if (row.size() != stolb) {
      return false;
    }
  }
  return true;
}

bool BarkalovaMMinValMatrSEQ::PreProcessingImpl() {
  if (!GetInput().empty()) {
    size_t stolb = GetInput()[0].size();
    GetOutput() = std::vector<int>(stolb, INT_MAX);
  }
  return true;
}

bool BarkalovaMMinValMatrSEQ::RunImpl() {
  const auto &matrix = GetInput();
  auto &res = GetOutput();

  for (size_t j = 0; j < res.size(); ++j) {
    for (size_t i = 0; i < matrix.size(); ++i) {
      if (matrix[i][j] < res[j]) {
        res[j] = matrix[i][j];
      }
    }
  }
  return true;
}

/*bool BarkalovaMMinValMatrSEQ::PostProcessingImpl() {
  return true;
}*/
bool BarkalovaMMinValMatrSEQ::PostProcessingImpl() {
  if (!GetInput().empty() && GetOutput().empty()) {
    return false;
  }
  return true;
}
}  // namespace barkalova_m_min_val_matr
