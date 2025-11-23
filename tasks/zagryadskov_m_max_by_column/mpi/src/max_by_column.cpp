#include "zagryadskov_m_max_by_column/mpi/include/max_by_column.hpp"

#include <mpi.h>

#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "zagryadskov_m_max_by_column/common/include/common.hpp"

namespace zagryadskov_m_max_by_column {

ZagryadskovMMaxByColumnMPI::ZagryadskovMMaxByColumnMPI(const InType &in) {
  std::cout << "HEREConstr1" << std::endl;
  SetTypeOfTask(GetStaticTypeOfTask());
  int world_rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  std::cout << "HEREConstr2" << std::endl;
  if (world_rank == 0) {
    GetInput() = in;
  }
}

bool ZagryadskovMMaxByColumnMPI::ValidationImpl() {
  std::cout << "HEREvalid1" << std::endl;
  bool res = false;
  int world_rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  std::cout << "HEREvalid2" << std::endl;
  if (world_rank == 0) {
    size_t mat_size = std::get<1>(GetInput()).size();
    size_t n = std::get<0>(GetInput());
    bool if_dividable = mat_size % n == 0;
    bool if_suits_int = mat_size <= static_cast<size_t>(INT_MAX);
    res = (n > 0) && (mat_size > 0) && (GetOutput().empty()) && if_dividable && if_suits_int;
  } else {
    res = true;
  }
  return res;
}

bool ZagryadskovMMaxByColumnMPI::PreProcessingImpl() {
  return true;
}

bool ZagryadskovMMaxByColumnMPI::RunImpl() {
  int world_size = 0;
  int world_rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int n = 0;
  const void *mat_data = nullptr;
  int m = 0;
  OutType &res = GetOutput();
  OutType local_res;
  OutType columns;
  std::vector<int> sendcounts(world_size);
  std::vector<int> displs(world_size);
  if (!displs.empty()) {
    displs[0] = 0;
  }

  if (world_rank == 0) {
    n = static_cast<int>(std::get<0>(GetInput()));
    const auto &mat = std::get<1>(GetInput());
    m = static_cast<int>(mat.size()) / n;
    mat_data = reinterpret_cast<const void *>(mat.data());
  }
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int columns_count = n / world_size;
  using T = double;  // datatype cannot be sent to other processes
  MPI_Datatype datatype = MPI_DOUBLE;

  int i = 0;
  int j = 0;
  int r = 0;
  T tmp = std::numeric_limits<T>::lowest();
  bool tmp_flag = false;

  if (world_rank == 0) {
    res.assign(n, std::numeric_limits<T>::lowest());
  }
  for (r = 0; r < world_size; ++r) {
    sendcounts[r] = (columns_count + static_cast<int>(r < (n % world_size))) * m;
    if (r > 0) {
      displs[r] = displs[r - 1] + sendcounts[r - 1];
    }
  }

  local_res.assign(static_cast<size_t>(sendcounts[world_rank] / m), std::numeric_limits<T>::lowest());
  columns.resize(sendcounts[world_rank]);
  MPI_Scatterv(mat_data, sendcounts.data(), displs.data(), datatype, columns.data(), sendcounts[world_rank], datatype,
               0, MPI_COMM_WORLD);
  for (j = 0; std::cmp_less(j, local_res.size()); ++j) {
    for (i = 0; i < m; ++i) {
      tmp = columns[(j * m) + i];
      tmp_flag = tmp > local_res[j];
      local_res[j] = (static_cast<T>(tmp_flag) * tmp) + (static_cast<T>(!tmp_flag) * local_res[j]);
    }
  }
  for (r = 0; r < world_size; ++r) {
    sendcounts[r] /= m;
    if (r > 0) {
      displs[r] = displs[r - 1] + sendcounts[r - 1];
    }
  }

  MPI_Gatherv(local_res.data(), static_cast<int>(local_res.size()), datatype, res.data(), sendcounts.data(),
              displs.data(), datatype, 0, MPI_COMM_WORLD);

  bool result = false;
  if (world_rank == 0) {
    result = !GetOutput().empty();
  } else {
    result = true;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return result;
}

bool ZagryadskovMMaxByColumnMPI::PostProcessingImpl() {
  bool result = false;
  int world_rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  if (world_rank == 0) {
    result = !GetOutput().empty();
  } else {
    result = true;
  }
  return result;
}

}  // namespace zagryadskov_m_max_by_column
