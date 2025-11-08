#include "moskaev_v_max_value_elem_matrix/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
#include <cstddef>
#include <utility>

#include "moskaev_v_max_value_elem_matrix/common/include/common.hpp"

namespace moskaev_v_max_value_elem_matrix {

MoskaevVMaxValueElemMatrixMPI::MoskaevVMaxValueElemMatrixMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = InType(in);
  GetOutput() = 0;
}

bool MoskaevVMaxValueElemMatrixMPI::ValidationImpl() {
  return !GetInput().empty() && (GetOutput() == 0);
}

bool MoskaevVMaxValueElemMatrixMPI::PreProcessingImpl() {
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

  // ИСПРАВЛЕНИЕ 1: Правильное распределение строк
  size_t start_row = (rank * rows_per_process + std::min(static_cast<size_t>(rank), remainder));
  size_t end_row = start_row + rows_per_process;
  if (static_cast<size_t>(rank) < remainder) {
    end_row += 1;
  }

  // Поиск локального максимума в своей части матрицы
  int local_max = INT_MIN;
  for (size_t i = start_row; i < end_row; ++i) {
    for (int element : matrix[i]) {
      local_max = std::max(element, local_max);
    }
  }

  // Находим глобальный максимумs
  int global_max = 0;

  // ИСПРАВЛЕНИЕ 2: Все процессы должны получать результат
  if (size == 1) {
    // Если только один процесс, просто копируем
    global_max = local_max;
  } else {
    // Используем MPI_Allreduce вместо MPI_Reduce, чтобы все процессы получили результат
    MPI_Allreduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  }

  // ИСПРАВЛЕНИЕ 3: Все процессы сохраняют результат
  GetOutput() = global_max;

  return true;
}

bool MoskaevVMaxValueElemMatrixMPI::PostProcessingImpl() {
  return true;
}

}  // namespace moskaev_v_max_value_elem_matrix
