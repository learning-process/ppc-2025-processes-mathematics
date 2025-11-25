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
  int rank, size;
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
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  size_t all_rows = 0, cols = 0;

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
  if (!size_valid) {
    GetOutput().clear();
    return false;
  }

  size_t base_rows_proc = all_rows / size;
  size_t ostatok = all_rows % size;
  size_t loc_rows = base_rows_proc + (static_cast<size_t>(rank) < ostatok ? 1 : 0);
  bool chunks_valid = true;
  if (rank == 0) {
    for (size_t i = 0; i < static_cast<size_t>(size); ++i) {
      size_t i_rows = base_rows_proc + (i < ostatok ? 1 : 0);
      if (i_rows > 0 && cols > INT_MAX / i_rows) {
        chunks_valid = false;
        break;
      }
    }
  }
  MPI_Bcast(&chunks_valid, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
  if (!chunks_valid) {
    GetOutput().clear();
    return false;
  }

  std::vector<int> send_counts(size);
  std::vector<int> displacements(size);

  if (rank == 0) {
    size_t curr_displacement = 0;
    for (size_t i = 0; i < static_cast<size_t>(size); ++i) {
      size_t i_rows = base_rows_proc + (i < ostatok ? 1 : 0);
      send_counts[i] = static_cast<int>(i_rows * cols);
      displacements[i] = static_cast<int>(curr_displacement);
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

  if (loc_rows > 0 && cols > INT_MAX / loc_rows) {
    GetOutput().clear();
    return false;
  }

  std::vector<int> local_data(loc_rows * cols);
  int recv_count = static_cast<int>(loc_rows * cols);

  MPI_Scatterv(rank == 0 ? flat_matrix.data() : nullptr, send_counts.data(), displacements.data(), MPI_INT,
               local_data.data(), recv_count, MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<int> local_min(cols, INT_MAX);

  for (size_t i = 0; i < loc_rows; ++i) {
    for (size_t j = 0; j < cols; ++j) {
      int value = local_data[i * cols + j];
      if (value < local_min[j]) {
        local_min[j] = value;
      }
    }
  }

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
