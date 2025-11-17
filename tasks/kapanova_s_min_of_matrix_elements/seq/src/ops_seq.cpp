#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"

#include <climits>
#include <cstddef>
#include <vector>

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"

namespace kapanova_s_min_of_matrix_elements {

KapanovaSMinOfMatrixElementsSEQ::KapanovaSMinOfMatrixElementsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;  // Используем оператор присваивания вместо ручного копирования
  GetOutput() = 0;
}

bool KapanovaSMinOfMatrixElementsSEQ::ValidationImpl() {
  const auto &matrix = GetInput();
  if (matrix.empty()) {
    return true;
  }

  const std::size_t cols = matrix[0].size();
  return std::ranges::all_of(matrix, [cols](const auto &row) { return row.size() == cols; });
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
      min_value = std::min(value, min_value);
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
