#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"

#include <algorithm>
#include <climits>
#include <vector>

namespace kapanova_s_min_of_matrix_elements {

KapanovaSMinOfMatrixElementsSEQ::KapanovaSMinOfMatrixElementsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput().resize(in.size());
  for (size_t i = 0; i < in.size(); ++i) {
    GetInput()[i] = in[i];
  }
  GetOutput() = 0;
}

bool KapanovaSMinOfMatrixElementsSEQ::ValidationImpl() {
  const auto &matrix = GetInput();
  if (matrix.empty()) {
    return true;
  }

  const size_t first_row_size = matrix[0].size();
  for (const auto &row : matrix) {
    if (row.size() != first_row_size) {
      return false;
    }
  }

  return true;
}

bool KapanovaSMinOfMatrixElementsSEQ::PreProcessingImpl() {
  // ВСЕГДА устанавливаем INT_MAX, даже для пустой матрицы
  GetOutput() = INT_MAX;
  return true;
}

bool KapanovaSMinOfMatrixElementsSEQ::RunImpl() {
  const auto &matrix = GetInput();

  if (matrix.empty()) {
    GetOutput() = INT_MAX;
    return true;
  }

  int min_value = INT_MAX;
  for (const auto &row : matrix) {
    for (const int value : row) {
      if (value < min_value) {
        min_value = value;
      }
    }
  }

  GetOutput() = min_value;
  return true;
}

bool KapanovaSMinOfMatrixElementsSEQ::PostProcessingImpl() {
  // ВСЕГДА возвращаем true
  return true;
}

}  // namespace kapanova_s_min_of_matrix_elements
