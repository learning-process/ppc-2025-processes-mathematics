#include "barkalova_m_min_val_matr/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
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

std::pair<int, int> GetLocalColumns(int rank, int size, int total_cols) {
  int base = total_cols / size;
  int extra = total_cols % size;
  int start = (rank * base) + std::min(rank, extra);
  int count = base + (rank < extra ? 1 : 0);
  return {start, count};
}

std::vector<int> GetColumnData(const std::vector<std::vector<int>> &matrix, int start_col, int col_count) {
  if (col_count == 0) {
    return {};
  }

  int rows = static_cast<int>(matrix.size());
  size_t total_elements = static_cast<size_t>(rows) * static_cast<size_t>(col_count);
  std::vector<int> data(total_elements);

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < col_count; ++j) {
      data[(i * col_count) + j] = matrix[i][start_col + j];
    }
  }
  return data;
}

std::vector<int> FindColumnMins(const std::vector<int> &data, int rows, int col_count) {
  if (col_count == 0) {
    return {};
  }

  std::vector<int> mins(col_count, INT_MAX);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < col_count; ++j) {
      mins[j] = std::min(mins[j], data[(i * col_count) + j]);
    }
  }
  return mins;
}

}  // namespace

bool BarkalovaMMinValMatrMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int rows = 0;
  int cols = 0;
  if (rank == 0) {
    const auto &matrix = GetInput();
    rows = static_cast<int>(matrix.size());
    cols = matrix.empty() ? 0 : static_cast<int>(matrix[0].size());
  }

  std::array<int, 2> dims = {rows, cols};
  MPI_Bcast(dims.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);
  rows = dims[0];
  cols = dims[1];

  if (rows == 0 || cols == 0) {
    GetOutput().clear();
    return true;
  }

  if (rows > INT_MAX / cols) {
    GetOutput().clear();
    return false;
  }

  auto [start_col, col_count] = GetLocalColumns(rank, size, cols);

  std::vector<int> local_data;
  if (rank == 0) {
    local_data = GetColumnData(GetInput(), start_col, col_count);

    for (int proc = 1; proc < size; ++proc) {
      auto [proc_start, proc_count] = GetLocalColumns(proc, size, cols);
      if (proc_count > 0) {
        auto proc_data = GetColumnData(GetInput(), proc_start, proc_count);
        MPI_Send(proc_data.data(), rows * proc_count, MPI_INT, proc, 0, MPI_COMM_WORLD);
      }
    }
  } else if (col_count > 0) {
    size_t total_elements = static_cast<size_t>(rows) * static_cast<size_t>(col_count);
    local_data.resize(total_elements);
    MPI_Recv(local_data.data(), rows * col_count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  auto local_mins = FindColumnMins(local_data, rows, col_count);

  std::vector<int> recv_counts(size);
  std::vector<int> displs(size);
  int current_displacement = 0;
  for (int i = 0; i < size; ++i) {
    auto [i_start, i_count] = GetLocalColumns(i, size, cols);
    recv_counts[i] = i_count;
    displs[i] = current_displacement;
    current_displacement += i_count;
  }

  GetOutput().resize(cols, INT_MAX);
  MPI_Gatherv(local_mins.data(), col_count, MPI_INT, GetOutput().data(), recv_counts.data(), displs.data(), MPI_INT, 0,
              MPI_COMM_WORLD);

  MPI_Bcast(GetOutput().data(), cols, MPI_INT, 0, MPI_COMM_WORLD);

  return true;
}

bool BarkalovaMMinValMatrMPI::PostProcessingImpl() {
  return true;
}
}  // namespace barkalova_m_min_val_matr
