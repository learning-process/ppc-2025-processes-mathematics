#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"

#include <climits>
#include <cstddef>
#include <vector>

namespace kapanova_s_min_of_matrix_elements {

KapanovaSMinOfMatrixElementsSEQ::KapanovaSMinOfMatrixElementsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

ppc::task::TypeOfTask KapanovaSMinOfMatrixElementsSEQ::GetStaticTypeOfTask() {
  return ppc::task::TypeOfTask::kSEQ;
}

bool KapanovaSMinOfMatrixElementsSEQ::ValidationImpl() {
  const auto &matrix = GetInput();
  if (matrix.empty()) {
    return false;
  }
  
  const std::size_t cols = matrix[0].size();
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
  int min_val = INT_MAX;

  for (const auto &row : matrix) {
    for (const int value : row) {
      min_val = std::min(value, min_val);
    }
  }

  GetOutput() = min_val;
  return true;
}

bool KapanovaSMinOfMatrixElementsSEQ::PostProcessingImpl() {
  return GetOutput() != INT_MAX;
}

}  // namespace kapanova_s_min_of_matrix_elements
