#include "kapanova_s_min_of_matrix_elements/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
#include <vector>

namespace kapanova_s_min_of_matrix_elements {

KapanovaSMinOfMatrixElementsMPI::KapanovaSMinOfMatrixElementsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput().resize(in.size());
  for (size_t i = 0; i < in.size(); ++i) {
    GetInput()[i] = in[i];
  }
  GetOutput() = 0;
}

bool KapanovaSMinOfMatrixElementsMPI::ValidationImpl() {
  const auto &matrix = GetInput();
  if (matrix.empty()) {
    return true;
  }

  const size_t cols = matrix[0].size();
  return std::ranges::all_of(matrix, [cols](const auto &row) { return row.size() == cols; });
}

bool KapanovaSMinOfMatrixElementsMPI::PreProcessingImpl() {
  GetOutput() = INT_MAX;
  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::RunImpl() {
  const auto &matrix = GetInput();

  if (matrix.empty()) {
    GetOutput() = INT_MAX;
    return true;
  }

  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int total_rows = static_cast<int>(matrix.size());
  const int total_cols = static_cast<int>(matrix[0].size());
  const int total_elements = total_rows * total_cols;

  int elements_per_process = total_elements / size;
  int remainder = total_elements % size;

  int start_element = 0;
  int end_element = 0;

  if (rank < remainder) {
    start_element = rank * (elements_per_process + 1);
    end_element = start_element + elements_per_process + 1;
  } else {
    start_element = (rank * elements_per_process) + remainder;
    end_element = start_element + elements_per_process;
  }

  int local_min = INT_MAX;
  for (int elem_idx = start_element; elem_idx < end_element; ++elem_idx) {
    const int row = elem_idx / total_cols;
    const int col = elem_idx % total_cols;

    if (row < total_rows && col < total_cols) {
      local_min = std::min(matrix[row][col], local_min);
    }
  }

  int global_min = local_min;
  if (size > 1) {
    MPI_Allreduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
  }

  GetOutput() = global_min;
  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::PostProcessingImpl() {
  return true;
}

}  // namespace kapanova_s_min_of_matrix_elements