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

bool ValidateMatrixSize(int rank, size_t rows, size_t cols) {
  bool size_valid = true;
  if (rank == 0) {
    if (rows > static_cast<size_t>(INT_MAX) || cols > static_cast<size_t>(INT_MAX)) {
      size_valid = false;
    }
    if (size_valid && rows > 0 && cols > 0 && rows > SIZE_MAX / cols) {
      size_valid = false;
    }
  }
  MPI_Bcast(&size_valid, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
  return size_valid;
}

bool CheckLocalBufferOverflow(size_t rows, size_t local_cols) {
  if (rows > 0 && local_cols > 0) {
    if (rows > SIZE_MAX / local_cols) {
      return false;
    }
    size_t total = rows * local_cols;
    if (total > static_cast<size_t>(INT_MAX)) {
      return false;
    }
  }
  return true;
}

std::pair<size_t, size_t> GetMatrixDimensions(int rank, const std::vector<std::vector<int>> &matrix) {
  size_t rows = 0;
  size_t cols = 0;

  if (rank == 0) {
    rows = matrix.size();
    cols = matrix.empty() ? 0 : matrix[0].size();
  }

  std::array<uint64_t, 2> dims = {rows, cols};
  MPI_Bcast(dims.data(), 2, MPI_UINT64_T, 0, MPI_COMM_WORLD);
  return {dims[0], dims[1]};
}

std::pair<size_t, size_t> GetColumnRange(int rank, int size, size_t cols) {
  size_t base_cols = cols / static_cast<size_t>(size);
  size_t extra = cols % static_cast<size_t>(size);

  size_t start_col = 0;
  for (int i = 0; i < rank; ++i) {
    size_t i_cols = base_cols + (static_cast<size_t>(i) < extra ? 1 : 0);
    start_col += i_cols;
  }
  size_t my_cols = base_cols + (static_cast<size_t>(rank) < extra ? 1 : 0);
  return {start_col, my_cols};
}

std::vector<int> CopyDataForRank0(int size, size_t rows, size_t cols, const std::vector<std::vector<int>> &matrix) {
  std::vector<int> my_data;
  auto [start, my_cols] = GetColumnRange(0, size, cols);

  if (my_cols > 0) {
    my_data.resize(rows * my_cols);
    for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < my_cols; ++j) {
        my_data[i * my_cols + j] = matrix[i][start + j];
      }
    }
  }
  return my_data;
}

void SendDataToOtherProcesses(int size, size_t rows, size_t cols, const std::vector<std::vector<int>> &matrix) {
  for (int proc = 1; proc < size; ++proc) {
    auto [proc_start, proc_cols] = GetColumnRange(proc, size, cols);

    if (proc_cols > 0) {
      std::vector<int> buffer(rows * proc_cols);
      for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < proc_cols; ++j) {
          buffer[i * proc_cols + j] = matrix[i][proc_start + j];
        }
      }
      MPI_Send(buffer.data(), static_cast<int>(buffer.size()), MPI_INT, proc, 0, MPI_COMM_WORLD);
    }
  }
}

std::vector<int> SendColumnsToOthers(int size, size_t rows, size_t cols, const std::vector<std::vector<int>> &matrix) {
  auto my_data = CopyDataForRank0(size, rows, cols, matrix);
  SendDataToOtherProcesses(size, rows, cols, matrix);
  return my_data;
}

std::vector<int> ReceiveColumnsFromRank0(int rank, int size, size_t rows, size_t cols) {
  std::vector<int> my_data;
  auto [start, my_cols] = GetColumnRange(rank, size, cols);

  if (my_cols > 0) {
    my_data.resize(rows * my_cols);
    MPI_Recv(my_data.data(), static_cast<int>(my_data.size()), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  return my_data;
}

std::vector<int> ScatterColumns(int rank, int size, size_t rows, size_t cols,
                                const std::vector<std::vector<int>> &matrix) {
  if (rank == 0) {
    return SendColumnsToOthers(size, rows, cols, matrix);
  } else {
    return ReceiveColumnsFromRank0(rank, size, rows, cols);
  }
}

std::vector<int> CalculateLocalMins(const std::vector<int> &local_data, size_t rows, size_t my_cols) {
  std::vector<int> mins(my_cols, INT_MAX);

  for (size_t col = 0; col < my_cols; ++col) {
    for (size_t row = 0; row < rows; ++row) {
      int value = local_data[row * my_cols + col];
      mins[col] = std::min(value, mins[col]);
    }
  }

  return mins;
}

void PrepareGathervData(int size, size_t cols, std::vector<int> &recv_counts, std::vector<int> &displacements) {
  size_t base_cols = cols / static_cast<size_t>(size);
  size_t extra = cols % static_cast<size_t>(size);

  recv_counts.resize(size);
  displacements.resize(size);

  int offset = 0;
  for (int i = 0; i < size; ++i) {
    size_t i_cols = base_cols + (static_cast<size_t>(i) < extra ? 1 : 0);
    recv_counts[i] = static_cast<int>(i_cols);
    displacements[i] = offset;
    offset += recv_counts[i];
  }
}

void GatherAndBroadcastResults(const std::vector<int> &local_mins, int size, size_t cols, size_t my_cols,
                               std::vector<int> &result) {
  result.resize(cols, INT_MAX);

  std::vector<int> recv_counts;
  std::vector<int> displacements;
  PrepareGathervData(size, cols, recv_counts, displacements);

  MPI_Gatherv(local_mins.data(), static_cast<int>(my_cols), MPI_INT, result.data(), recv_counts.data(),
              displacements.data(), MPI_INT, 0, MPI_COMM_WORLD);

  if (size > 1) {
    MPI_Bcast(result.data(), static_cast<int>(cols), MPI_INT, 0, MPI_COMM_WORLD);
  }
}

}  // namespace

bool BarkalovaMMinValMatrMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &matrix = GetInput();
  auto [rows, cols] = GetMatrixDimensions(rank, matrix);

  if (rows == 0 || cols == 0) {
    GetOutput().clear();
    return true;
  }

  if (!ValidateMatrixSize(rank, rows, cols)) {
    GetOutput().clear();
    return false;
  }

  auto [start, my_cols] = GetColumnRange(rank, size, cols);

  if (my_cols > static_cast<size_t>(INT_MAX)) {
    GetOutput().clear();
    return false;
  }

  if (my_cols > 0 && !CheckLocalBufferOverflow(rows, my_cols)) {
    GetOutput().clear();
    return false;
  }

  std::vector<int> local_data = ScatterColumns(rank, size, rows, cols, matrix);

  if (my_cols > 0 && local_data.size() != rows * my_cols) {
    GetOutput().clear();
    return false;
  }

  std::vector<int> local_mins;
  if (my_cols > 0) {
    local_mins = CalculateLocalMins(local_data, rows, my_cols);
  } else {
    local_mins.resize(0);
  }

  GatherAndBroadcastResults(local_mins, size, cols, my_cols, GetOutput());
  return true;
}

bool BarkalovaMMinValMatrMPI::PostProcessingImpl() {
  return true;
}
}  // namespace barkalova_m_min_val_matr
