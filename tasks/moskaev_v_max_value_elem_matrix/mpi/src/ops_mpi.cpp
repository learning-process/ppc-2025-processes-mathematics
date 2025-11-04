#include "moskaev_v_max_value_elem_matrix/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <vector>

#include "moskaev_v_max_value_elem_matrix/common/include/common.hpp"

namespace moskaev_v_max_value_elem_matrix {

MoskaevVMaxValueElemMatrixMPI::MoskaevVMaxValueElemMatrixMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool MoskaevVMaxValueElemMatrixMPI::ValidationImpl() {
  return !GetInput().empty() && (GetOutput() == 0);
}

bool MoskaevVMaxValueElemMatrixMPI::PreProcessingImpl() {
  // Каждый процесс имеет всю матрицу, ничего не распределяем
  return true;
}

bool MoskaevVMaxValueElemMatrixMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &matrix = GetInput();
  auto total_rows = matrix.size();

  if (total_rows == 0) {
    return false;
  }

  // Рассчитываем диапазон строк для текущего процесса
  auto rows_per_process = total_rows / size;
  auto remainder = total_rows % size;

  auto start_row = (rank * rows_per_process) + std::min(static_cast<size_t>(rank), remainder);
  auto end_row = start_row + rows_per_process + ((rank < static_cast<int>(remainder)) ? 1 : 0);

  // Поиск локального максимума в своей части матрицы
  int local_max = matrix[start_row][0];
  for (size_t i = start_row; i < end_row; ++i) {
    for (int element : matrix[i]) {
      local_max = std::max(element, local_max);
    }
  }

  // Находим глобальный максимум
  int global_max = 0;
  MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    GetOutput() = global_max;
  } else {
    GetOutput() = local_max;  // или 0, в зависимости от требований
  }

  return true;
}

bool MoskaevVMaxValueElemMatrixMPI::PostProcessingImpl() {
  // Ничего не нужно очищать, так как не выделяли дополнительной памяти
  return true;
}

}  // namespace moskaev_v_max_value_elem_matrix
