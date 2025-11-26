#include "barkalova_m_min_val_matr/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
#include <cstddef>
#include <utility>
#include <vector>

#include "barkalova_m_min_val_matr/common/include/common.hpp"

namespace barkalova_m_min_val_matr {

BarkalovaMMinValMatrMPI::BarkalovaMMinValMatrMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput().resize(in.size());
  for (size_t i = 0; i < in.size(); ++i) {
    GetInput()[i] = in[i];
  }
  GetOutput().clear();
}

bool BarkalovaMMinValMatrMPI::ValidationImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  bool is_valid = true;

  if (rank == 0) {
    const auto &matrix = GetInput();
    if (!matrix.empty()) {
      size_t stolb = matrix[0].size();
      is_valid = std::ranges::all_of(matrix, [stolb](const auto &row) { return row.size() == stolb; });
    }
  }

  MPI_Bcast(&is_valid, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
  return is_valid;
}

bool BarkalovaMMinValMatrMPI::PreProcessingImpl() {
  if (!GetInput().empty()) {
    size_t stolb = GetInput()[0].size();
    GetOutput().resize(stolb, INT_MAX);
  } else {
    GetOutput().clear();
  }
  return true;
}

namespace {

std::pair<size_t, size_t> GetMatrixDimensions(int rank, const std::vector<std::vector<int>> &matrix) {
  size_t all_rows = 0;
  size_t cols = 0;
  if (rank == 0) {
    all_rows = matrix.size();
    cols = matrix.empty() ? 0 : matrix[0].size();
  }
  return {all_rows, cols};
}

bool CheckGlobalSize(int rank, size_t all_rows, size_t cols) {
  bool size_valid = true;
  if (rank == 0) {
    if (cols > INT_MAX || all_rows > INT_MAX) {
      size_valid = false;
    } else {
      if (all_rows > 0 && cols > INT_MAX / all_rows) {
        size_valid = false;
      }
    }
  }
  MPI_Bcast(&size_valid, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
  return size_valid;
}

bool CheckChunksSize(int rank, int size, size_t all_rows, size_t cols) {
  bool chunks_valid = true;
  if (rank == 0) {
    size_t base_rows_proc = all_rows / static_cast<size_t>(size);
    size_t ostatok = all_rows % static_cast<size_t>(size);

    for (size_t i = 0; i < static_cast<size_t>(size); ++i) {
      size_t i_rows = base_rows_proc + (i < ostatok ? 1 : 0);
      if (i_rows > 0 && cols > INT_MAX / i_rows) {
        chunks_valid = false;
        break;
      }
    }
  }
  MPI_Bcast(&chunks_valid, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
  return chunks_valid;
}

bool CheckLocalSize(size_t loc_rows, size_t cols) {
  return !(loc_rows > 0 && cols > INT_MAX / loc_rows);
}

std::tuple<std::vector<int>, std::vector<int>, std::vector<int>> PrepareScatterData(
    int rank, int size, size_t all_rows, size_t cols, size_t base_rows_proc, size_t ostatok,
    const std::vector<std::vector<int>> &matrix) {
  std::vector<int> send_counts(size);
  std::vector<int> displacements(size);
  std::vector<int> flat_matrix;

  if (rank == 0) {
    size_t curr_displacement = 0;
    for (size_t i = 0; i < static_cast<size_t>(size); ++i) {
      size_t i_rows = base_rows_proc + (i < ostatok ? 1 : 0);
      send_counts[i] = static_cast<int>(i_rows * cols);
      displacements[i] = static_cast<int>(curr_displacement);
      curr_displacement += i_rows * cols;
    }

    flat_matrix.reserve(all_rows * cols);
    for (const auto &row : matrix) {
      flat_matrix.insert(flat_matrix.end(), row.begin(), row.end());
    }
  }

  return {send_counts, displacements, flat_matrix};
}

std::vector<int> ProcessLocalData(size_t loc_rows, size_t cols, const std::vector<int> &local_data) {
  std::vector<int> local_min(cols, INT_MAX);

  if (loc_rows > 0 && cols > 0) {
    for (size_t i = 0; i < loc_rows; ++i) {
      for (size_t j = 0; j < cols; ++j) {
        int value = local_data[(i * cols) + j];
        local_min[j] = std::min(value, local_min[j]);
      }
    }
  }

  return local_min;
}

}  // namespace

bool BarkalovaMMinValMatrMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  auto [all_rows, cols] = GetMatrixDimensions(rank, GetInput());

  MPI_Bcast(&all_rows, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&cols, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

  if (all_rows == 0 || cols == 0) {
    GetOutput().clear();
    return true;
  }

  if (!CheckGlobalSize(rank, all_rows, cols)) {
    GetOutput().clear();
    return false;
  }

  size_t base_rows_proc = all_rows / static_cast<size_t>(size);
  size_t ostatok = all_rows % static_cast<size_t>(size);
  size_t loc_rows = base_rows_proc + (static_cast<size_t>(rank) < ostatok ? 1 : 0);

  if (!CheckChunksSize(rank, size, all_rows, cols)) {
    GetOutput().clear();
    return false;
  }

  if (!CheckLocalSize(loc_rows, cols)) {
    GetOutput().clear();
    return false;
  }

  auto [send_counts, displacements, flat_matrix] =
      PrepareScatterData(rank, size, all_rows, cols, base_rows_proc, ostatok, GetInput());

  int recv_count = (loc_rows > 0 && cols > 0) ? static_cast<int>(loc_rows * cols) : 0;
  std::vector<int> local_data(recv_count > 0 ? recv_count : 0);

  MPI_Scatterv(rank == 0 ? flat_matrix.data() : nullptr, send_counts.data(), displacements.data(), MPI_INT,
               recv_count > 0 ? local_data.data() : nullptr, recv_count, MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<int> local_min = ProcessLocalData(loc_rows, cols, local_data);

  int cols_int = static_cast<int>(cols);
  std::vector<int> global_min(cols);
  MPI_Allreduce(local_min.data(), global_min.data(), cols_int, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

  GetOutput() = global_min;
  return true;
}

bool BarkalovaMMinValMatrMPI::PostProcessingImpl() {
  return true;
}
}  // namespace barkalova_m_min_val_matr
