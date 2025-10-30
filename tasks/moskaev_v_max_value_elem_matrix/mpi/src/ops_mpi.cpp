#include "moskaev_v_max_value_elem_matrix/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <vector>
#include <algorithm> 

#include "moskaev_v_max_value_elem_matrix/common/include/common.hpp"

namespace moskaev_v_max_value_elem_matrix {


std::vector<std::vector<int>> g_local_matrix;

void DistributeMatrix(const std::vector<std::vector<int>> &full_matrix, int rank, int size) {
  int matrix_dim = full_matrix.size();

  if (rank == 0) {
  // Процесс 0 распределяет данные (матрица пришла из тестов)
    int rows_per_process = matrix_dim / size;
    int remainder = matrix_dim % size;

    // Обрабатываем свою часть сначала
    int start_row = 0;
    int end_row = rows_per_process + (0 < remainder ? 1 : 0);
    g_local_matrix.assign(full_matrix.begin() + start_row, full_matrix.begin() + end_row);

    // Отправляем данные другим процессам
    for (int proc = 1; proc < size; ++proc) {
      start_row = end_row;
      end_row = start_row + rows_per_process + (proc < remainder ? 1 : 0);
      int rows_to_send = end_row - start_row;

      MPI_Send(&rows_to_send, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);

      for (int i = start_row; i < end_row; ++i) {
        MPI_Send(full_matrix[i].data(), matrix_dim, MPI_INT, proc, 0, MPI_COMM_WORLD);
      }
    }
  } else {
    // Получаем данные от процесса 0
    int rows_to_receive;
    MPI_Recv(&rows_to_receive, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    g_local_matrix.resize(rows_to_receive, std::vector<int>(matrix_dim));
    for (int i = 0; i < rows_to_receive; ++i) {
      MPI_Recv(g_local_matrix[i].data(), matrix_dim, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }
}


MoskaevVMaxValueElemMatrixMPI::MoskaevVMaxValueElemMatrixMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;  // Получаем готовую матрицу из тестов
  GetOutput() = 0;
}

bool MoskaevVMaxValueElemMatrixMPI::ValidationImpl() {
  return !GetInput().empty() && (GetOutput() == 0);
}

bool MoskaevVMaxValueElemMatrixMPI::PreProcessingImpl() {
  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Распределяем матрицу (пришедшую из тестов) по процессам
  DistributeMatrix(GetInput(), rank, size);

  return true;
}

bool MoskaevVMaxValueElemMatrixMPI::RunImpl() {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (g_local_matrix.empty()) {
    return false;
  }

  // Поиск локального максимума
  int local_max = g_local_matrix[0][0];
  for (const auto &row : g_local_matrix) {
    for (int element : row) {
      local_max = std::max(element, local_max);
    }
  }

  // Находим глобальный максимум
  int global_max = 0;
  MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    GetOutput() = global_max;
  } else {
    GetOutput() = local_max;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool MoskaevVMaxValueElemMatrixMPI::PostProcessingImpl() {
  g_local_matrix.clear();
  return true;
}

}  // namespace moskaev_v_max_value_elem_matrix
