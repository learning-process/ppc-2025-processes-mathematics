#include "barkalova_m_min_val_matr/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
#include <cstddef>
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

bool BarkalovaMMinValMatrMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  size_t all_rows = 0;
  size_t cols = 0;

  if (rank == 0) {
    const auto &matrix = GetInput();
    if (!matrix.empty()) {
      all_rows = matrix.size();
      cols = matrix[0].size();
    }
  }

  MPI_Bcast(&all_rows, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&cols, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

  if (all_rows == 0 || cols == 0) {
    GetOutput().clear();
    return true;
  }

  size_t base_rows_proc = all_rows / size;
  size_t ostatok = all_rows % size;
  bool rank_less_than_ostatok = (rank < static_cast<int>(ostatok));
  size_t loc_rows = base_rows_proc + (rank_less_than_ostatok ? 1 : 0);

  std::vector<int> send_counts(size);
  std::vector<int> displacements(size);

  if (rank == 0) {
    size_t curr_displacement = 0;
    for (int i = 0; i < size; ++i) {
      bool i_less_than_ostatok = (i < static_cast<int>(ostatok));
      size_t i_rows = base_rows_proc + (i_less_than_ostatok ? 1 : 0);
      send_counts[i] = i_rows * cols;
      displacements[i] = curr_displacement;
      curr_displacement += i_rows * cols;
    }
  }

  std::vector<int> flat_matrix;
  if (rank == 0) {
    const auto &matrix = GetInput();
    flat_matrix.reserve(all_rows * cols);
    for (const auto &row : matrix) {
      flat_matrix.insert(flat_matrix.end(), row.begin(), row.end());
    }
  }

  std::vector<int> local_data(loc_rows * cols);
  int recv_count = loc_rows * cols;

  const void *sendbuf = (rank == 0) ? flat_matrix.data() : nullptr;
  void *recvbuf = (recv_count > 0) ? local_data.data() : nullptr;

  MPI_Scatterv(sendbuf, send_counts.data(), displacements.data(), MPI_INT, recvbuf, recv_count, MPI_INT, 0,
               MPI_COMM_WORLD);

  std::vector<int> local_min(cols, INT_MAX);
  for (size_t i = 0; i < loc_rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      int value = local_data[i * cols + j];
      local_min[j] = std::min(value, local_min[j]);
    }
  }

  std::vector<int> global_min(cols);
  MPI_Allreduce(local_min.data(), global_min.data(), cols, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

  GetOutput() = global_min;
  return true;
}

bool BarkalovaMMinValMatrMPI::PostProcessingImpl() {
  return true;
}
}  // namespace barkalova_m_min_val_matr
