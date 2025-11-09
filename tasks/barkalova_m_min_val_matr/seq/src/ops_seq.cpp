#include "barkalova_m_min_val_matr/seq/include/ops_seq.hpp"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <vector>

#include "barkalova_m_min_val_matr/common/include/common.hpp"

namespace barkalova_m_min_val_matr {

BarkalovaMMinValMatrSEQ::BarkalovaMMinValMatrSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  // GetInput() = in;
  //  GetOutput() = std::vector<int>();
  //  Вместо присваивания - копируем элементы
  GetInput().resize(in.size());
  for (size_t i = 0; i < in.size(); ++i) {
    GetInput()[i] = in[i];
  }
  GetOutput().clear();
}
bool BarkalovaMMinValMatrSEQ::ValidationImpl() {
  const auto &matrix = GetInput();
  if (matrix.empty()) {
    return false;
  }
  size_t stolb = matrix[0].size();
  // все строки должны иметь одинак разм
  return std::ranges::all_of(matrix, [stolb](const auto &row) { return row.size() == stolb; });
}
// было изначально
/*bool BarkalovaMMinValMatrSEQ::PreProcessingImpl() {
  if (!GetInput().empty()) {
    size_t stolb = GetInput()[0].size();
    GetOutput() = std::vector<int>(stolb, INT_MAX);
  }
  }
  return true;
}*/
bool BarkalovaMMinValMatrSEQ::PreProcessingImpl() {
  if (!GetInput().empty()) {
    size_t stolb = GetInput()[0].size();
    GetOutput().resize(stolb, INT_MAX);  // ← использовать resize вместо присваивания
  } else {
    GetOutput().clear();
  }
  return true;
}
/*bool BarkalovaMMinValMatrSEQ::RunImpl() {
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
}*/
bool BarkalovaMMinValMatrSEQ::RunImpl() {
  const auto &matrix = GetInput();
  auto &res = GetOutput();
  for (auto &elem : res) {
    elem = INT_MAX;
  }

  for (size_t j = 0; j < res.size(); ++j) {
    for (const auto &row : matrix) {
      res[j] = std::min(row[j], res[j]);
    }
  }
  return true;
}

/*bool BarkalovaMMinValMatrSEQ::PostProcessingImpl() {
  return true;
}*/
bool BarkalovaMMinValMatrSEQ::PostProcessingImpl() {
  return GetInput().empty() || !GetOutput().empty();
}
}  // namespace barkalova_m_min_val_matr
