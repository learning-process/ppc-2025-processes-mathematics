#include "kapanova_s_min_of_matrix_elements/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <climits>
#include <vector>

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"
#include "util/include/util.hpp"

namespace kapanova_s_min_of_matrix_elements {

KapanovaSMinOfMatrixElementsMPI::KapanovaSMinOfMatrixElementsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KapanovaSMinOfMatrixElementsMPI::ValidationImpl() {
  const auto &matrix = GetInput();
  return !matrix.empty() && !matrix[0].empty();
}

bool KapanovaSMinOfMatrixElementsMPI::PreProcessingImpl() {
  GetOutput() = INT_MAX;
  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::RunImpl() {
  const auto &matrix = GetInput();

  if (matrix.empty() || matrix[0].empty()) {
    return false;
  }

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const int total_rows = matrix.size();
  const int total_cols = matrix[0].size();
  const int total_elements = total_rows * total_cols;
  int elements_per_process = total_elements / size;
  int remainder = total_elements % size;

  int start_element, end_element;

  if (rank < remainder) {
    start_element = rank * (elements_per_process + 1);
    end_element = start_element + elements_per_process + 1;
  } else {
    start_element = rank * elements_per_process + remainder;
    end_element = start_element + elements_per_process;
  }
  int local_min = INT_MAX;

  for (int elem_idx = start_element; elem_idx < end_element; elem_idx++) {
    int row = elem_idx / total_cols;
    int col = elem_idx % total_cols;

    if (matrix[row][col] < local_min) {
      local_min = matrix[row][col];
    }
  }

  int global_min;
  MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

  MPI_Bcast(&global_min, 1, MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = global_min;

  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::PostProcessingImpl() {
  return GetOutput() != INT_MAX;
}

}  // namespace kapanova_s_min_of_matrix_elements
