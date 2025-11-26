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

bool BarkalovaMMinValMatrMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  size_t all_rows = 0;
  size_t cols = 0;
  bool is_root = (rank == 0);

  if (is_root) {
    const auto &matrix = GetInput();
    all_rows = matrix.size();
    cols = matrix.empty() ? 0 : matrix[0].size();
  }

  MPI_Bcast(&all_rows, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&cols, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

  bool empty_matrix = (all_rows == 0) || (cols == 0);
  if (empty_matrix) {
    GetOutput().clear();
    return true;
  }

  size_t base_rows = all_rows / static_cast<size_t>(size);
  size_t remainder = all_rows % static_cast<size_t>(size);

  bool has_extra_row = std::cmp_less(rank, remainder);
  size_t local_rows = base_rows + (has_extra_row ? 1 : 0);
  int local_count = static_cast<int>(local_rows * cols);

  std::vector<int> scatter_counts;
  std::vector<int> scatter_displs;
  std::vector<int> flat_matrix;

  if (is_root) {
    scatter_counts.resize(size);
    scatter_displs.resize(size);

    size_t current_offset = 0;
    for (int i = 0; i < size; ++i) {
      bool process_has_extra = std::cmp_less(i, remainder);
      size_t process_rows = base_rows + (process_has_extra ? 1 : 0);
      scatter_counts[i] = static_cast<int>(process_rows * cols);
      scatter_displs[i] = static_cast<int>(current_offset);
      current_offset += process_rows * cols;
    }

    const auto &matrix = GetInput();
    flat_matrix.reserve(all_rows * cols);
    for (const auto &row : matrix) {
      flat_matrix.insert(flat_matrix.end(), row.begin(), row.end());
    }
  }

  std::vector<int> counts(size);
  std::vector<int> displs(size);

  if (is_root) {
    counts = scatter_counts;
    displs = scatter_displs;
  }

  MPI_Bcast(counts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(displs.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<int> local_data(local_count);

  MPI_Scatterv(is_root ? flat_matrix.data() : nullptr, counts.data(), displs.data(), MPI_INT, local_data.data(),
               local_count, MPI_INT, 0, MPI_COMM_WORLD);

  std::vector<int> local_min(cols, INT_MAX);

  bool has_local_data = (local_count > 0);
  if (has_local_data) {
    for (size_t i = 0; i < local_rows; ++i) {
      size_t row_offset = i * cols;
      for (size_t j = 0; j < cols; ++j) {
        int current_value = local_data[row_offset + j];
        local_min[j] = std::min(current_value, local_min[j]);
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
