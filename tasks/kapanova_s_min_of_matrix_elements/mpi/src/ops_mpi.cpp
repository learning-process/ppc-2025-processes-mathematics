#include "kapanova_s_min_of_matrix_elements/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
#include <cstddef>
#include <vector>

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"
#include "task/include/task.hpp"

namespace kapanova_s_min_of_matrix_elements {

KapanovaSMinOfMatrixElementsMPI::KapanovaSMinOfMatrixElementsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KapanovaSMinOfMatrixElementsMPI::ValidationImpl() {
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

bool KapanovaSMinOfMatrixElementsMPI::PreProcessingImpl() {
  GetOutput() = INT_MAX;
  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::RunImpl() {
  const auto &matrix = GetInput();

  // Инициализация MPI
  int mpi_initialized = 0;
  MPI_Initialized(&mpi_initialized);
  if (!mpi_initialized) {
    MPI_Init(nullptr, nullptr);
  }

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (matrix.empty() || matrix[0].empty()) {
    if (!mpi_initialized) {
      MPI_Finalize();
    }
    return false;
  }

  const int total_rows = static_cast<int>(matrix.size());
  const int total_cols = static_cast<int>(matrix[0].size());
  const int total_elements = total_rows * total_cols;

  // Распределяем элементы между процессами
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

  // Поиск локального минимума
  int local_min = INT_MAX;
  for (int elem_idx = start_element; elem_idx < end_element; ++elem_idx) {
    int row = elem_idx / total_cols;
    int col = elem_idx % total_cols;

    if (row < total_rows && col < total_cols) {
      local_min = std::min(matrix[row][col], local_min);
    }
  }

  // Находим глобальный минимум
  int global_min = local_min;
  if (size > 1) {
    MPI_Allreduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
  }

  GetOutput() = global_min;

  // Финализируем MPI если мы ее инициализировали
  if (!mpi_initialized) {
    MPI_Finalize();
  }

  return true;
}

bool KapanovaSMinOfMatrixElementsMPI::PostProcessingImpl() {
  return GetOutput() != INT_MAX;
}

}  // namespace kapanova_s_min_of_matrix_elements
