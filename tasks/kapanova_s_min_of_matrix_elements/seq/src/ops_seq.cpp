#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>
#include <climits> 

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"
#include "util/include/util.hpp"

namespace kapanova_s_min_of_matrix_elements {

KapanovaSMinOfMatrixElementsSEQ::KapanovaSMinOfMatrixElementsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KapanovaSMinOfMatrixElementsSEQ::ValidationImpl() {
  const auto& matrix = GetInput();
  return !matrix.empty() && !matrix[0].empty();
}

bool KapanovaSMinOfMatrixElementsSEQ::PreProcessingImpl() {
  GetOutput() = INT_MAX;
  return true;
}

bool KapanovaSMinOfMatrixElementsSEQ::RunImpl() {
  const auto& matrix = GetInput();
  if (matrix.empty() || matrix[0].empty()) {
    return false;
  }
  int min_val = matrix[0][0];
  
  for (const auto& row : matrix) {
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
