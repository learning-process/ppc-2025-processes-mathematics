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

bool ValidateMatrixSize(int rank, size_t rows, size_t stolb) {
  bool size_valid = true;
  if (rank == 0) {
    if (rows > static_cast<size_t>(INT_MAX) || stolb > static_cast<size_t>(INT_MAX)) {
      size_valid = false;
    }
    if (size_valid && rows > 0 && stolb > 0 && rows > SIZE_MAX / stolb) {
      size_valid = false;
    }
  }
  MPI_Bcast(&size_valid, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
  return size_valid;
}

bool ValidateLocalSize(size_t col_stolb) {
  return col_stolb <= static_cast<size_t>(INT_MAX);
}

std::pair<size_t, size_t> GetMatrixDimensions(int rank, const std::vector<std::vector<int>> &matrix) {
  size_t rows = 0;
  size_t stolb = 0;

  if (rank == 0) {
    rows = matrix.size();
    stolb = matrix.empty() ? 0 : matrix[0].size();
  }

  std::array<uint64_t, 2> dims = {rows, stolb};
  MPI_Bcast(dims.data(), 2, MPI_UINT64_T, 0, MPI_COMM_WORLD);
  return {dims[0], dims[1]};
}

std::vector<int> GetMatrixData(int rank, size_t rows, size_t stolb, const std::vector<std::vector<int>> &matrix) {
  std::vector<int> flat_matrix(rows * stolb);

  if (rank == 0) {
    for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < stolb; ++j) {
        flat_matrix[(i * stolb) + j] = matrix[i][j];
      }
    }
  }

  MPI_Bcast(flat_matrix.data(), static_cast<int>(rows * stolb), MPI_INT, 0, MPI_COMM_WORLD);
  return flat_matrix;
}

std::pair<size_t, size_t> GetColumnRange(int rank, int size, size_t stolb) {
  size_t loc_stolb = stolb / static_cast<size_t>(size);
  size_t ostatok = stolb % static_cast<size_t>(size);

  size_t start_stolb = 0;
  for (int i = 0; i < rank; ++i) {
    size_t i_cols = loc_stolb + (i < static_cast<int>(ostatok) ? 1 : 0);
    start_stolb += i_cols;
  }

  size_t col_stolb = loc_stolb + (rank < static_cast<int>(ostatok) ? 1 : 0);

  return {start_stolb, col_stolb};
}

std::vector<int> CalculateLocalMins(const std::vector<int> &flat_matrix, size_t rows, size_t stolb, size_t start_stolb,
                                    size_t col_stolb) {
  std::vector<int> loc_min(col_stolb, INT_MAX);
  for (size_t k = 0; k < col_stolb; ++k) {
    size_t stolb_index = start_stolb + k;
    for (size_t i = 0; i < rows; ++i) {
      int value = flat_matrix[(i * stolb) + stolb_index];
      if (value < loc_min[k]) {
        loc_min[k] = value;
      }
    }
  }
  return loc_min;
}

void PrepareGathervData(int size, size_t stolb, std::vector<int> &recv_counts, std::vector<int> &displacements) {
  size_t loc_stolb = stolb / static_cast<size_t>(size);
  size_t ostatok = stolb % static_cast<size_t>(size);

  recv_counts.resize(size);
  displacements.resize(size);

  size_t current_displacement = 0;
  for (int i = 0; i < size; i++) {
    size_t i_cols = loc_stolb + (i < static_cast<int>(ostatok) ? 1 : 0);
    recv_counts[i] = static_cast<int>(i_cols);
    displacements[i] = static_cast<int>(current_displacement);
    current_displacement += i_cols;
  }
}

void GatherAndBroadcastResults(const std::vector<int> &loc_min, int size, size_t stolb, size_t col_stolb,
                               std::vector<int> &res) {
  res.resize(stolb, INT_MAX);

  std::vector<int> recv_counts;
  std::vector<int> displacements;
  PrepareGathervData(size, stolb, recv_counts, displacements);

  int send_count = static_cast<int>(col_stolb);
  MPI_Gatherv(loc_min.data(), send_count, MPI_INT, res.data(), recv_counts.data(), displacements.data(), MPI_INT, 0,
              MPI_COMM_WORLD);

  if (size > 1) {
    MPI_Bcast(res.data(), static_cast<int>(stolb), MPI_INT, 0, MPI_COMM_WORLD);
  }
}

}  // namespace

bool BarkalovaMMinValMatrMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &matrix = GetInput();
  auto [rows, stolb] = GetMatrixDimensions(rank, matrix);

  if (rows == 0 || stolb == 0) {
    GetOutput().clear();
    return true;
  }

  if (!ValidateMatrixSize(rank, rows, stolb)) {
    GetOutput().clear();
    return false;
  }

  std::vector<int> flat_matrix = GetMatrixData(rank, rows, stolb, matrix);
  auto [start_stolb, col_stolb] = GetColumnRange(rank, size, stolb);

  if (!ValidateLocalSize(col_stolb)) {
    GetOutput().clear();
    return false;
  }

  std::vector<int> loc_min = CalculateLocalMins(flat_matrix, rows, stolb, start_stolb, col_stolb);
  GatherAndBroadcastResults(loc_min, size, stolb, col_stolb, GetOutput());

  return true;
}

bool BarkalovaMMinValMatrMPI::PostProcessingImpl() {
  return true;
}
}  // namespace barkalova_m_min_val_matr
