#include "kapanova_s_min_of_matrix_elements/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
#include <vector>

namespace kapanova_s_min_of_matrix_elements {

KapanovaSMinOfMatrixElementsMPI::KapanovaSMinOfMatrixElementsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

ppc::task::TypeOfTask KapanovaSMinOfMatrixElementsMPI::GetStaticTypeOfTask() {
  return ppc::task::TypeOfTask::kMPI;
}

bool KapanovaSMinOfMatrixElementsMPI::ValidationImpl() {
  const auto &matrix = GetInput();
  if (matrix.empty()) {
    return false;
  }

  const size_t cols = matrix[0].size();
  for (const auto &row : matrix) {
    if (row.size() != cols) {
      return false;
    }
  }

  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::PreProcessingImpl() {
  GetOutput() = INT_MAX;
  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::RunImpl() {
  int mpi_initialized;
  MPI_Initialized(&mpi_initialized);
  if (!mpi_initialized) {
    return false;
  }

  const auto &matrix = GetInput();

  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Если матрица пустая
  if (matrix.empty() || matrix[0].empty()) {
    int local_min = INT_MAX;
    int global_min = INT_MAX;
    if (size > 1) {
      MPI_Allreduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    } else {
      global_min = local_min;
    }
    GetOutput() = global_min;
    return true;
  }

  const int total_rows = static_cast<int>(matrix.size());
  const int total_cols = static_cast<int>(matrix[0].size());
  const int total_elements = total_rows * total_cols;

  // Распределение элементов
  int elements_per_process = total_elements / size;
  int remainder = total_elements % size;

  int start_element = rank * elements_per_process + std::min(rank, remainder);
  int end_element = start_element + elements_per_process + (rank < remainder ? 1 : 0);

  int local_min = INT_MAX;

  // Поиск локального минимума
  for (int elem_idx = start_element; elem_idx < end_element; ++elem_idx) {
    int row = elem_idx / total_cols;
    int col = elem_idx % total_cols;

    if (row < total_rows && col < total_cols) {
      if (matrix[row][col] < local_min) {
        local_min = matrix[row][col];
      }
    }
  }

  // Сбор всех минимумов
  int global_min = INT_MAX;
  if (size > 1) {
    MPI_Allreduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
  } else {
    global_min = local_min;
  }

  GetOutput() = global_min;
  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::PostProcessingImpl() {
  return true;
}

}  // namespace kapanova_s_min_of_matrix_elements
