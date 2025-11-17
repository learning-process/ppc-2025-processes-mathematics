#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <vector>

#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"
#include "task/include/task.hpp"

namespace kapanova_s_min_of_matrix_elements {

ppc::task::TypeOfTask KapanovaSMinOfMatrixElementsSEQ::GetStaticTypeOfTask() {
  return ppc::task::TypeOfTask::kSEQ;
}

KapanovaSMinOfMatrixElementsSEQ::KapanovaSMinOfMatrixElementsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KapanovaSMinOfMatrixElementsSEQ::ValidationImpl() {
  const auto &matrix = GetInput();
  if (matrix.empty()) {
    return false;
  }

  // Проверяем, что все строки имеют одинаковый размер
  const size_t cols = matrix[0].size();
  for (const auto &row : matrix) {
    if (row.size() != cols) {
      return false;
    }
  }

  return true;
}

bool KapanovaSMinOfMatrixElementsSEQ::PreProcessingImpl() {
  GetOutput() = INT_MAX;
  return true;
}

bool KapanovaSMinOfMatrixElementsSEQ::RunImpl() {
  const auto &matrix = GetInput();

  if (matrix.empty() || matrix[0].empty()) {
    return false;
  }

  int min_val = matrix[0][0];
  for (const auto &row : matrix) {
    for (int value : row) {
      if (value < min_val) {
        min_val = value;
      }
    }
  }

  GetOutput() = min_val;
  return true;
}

bool KapanovaSMinOfMatrixElementsSEQ::PostProcessingImpl() {
  return GetOutput() != INT_MAX;
}

}  // namespace kapanova_s_min_of_matrix_elements
