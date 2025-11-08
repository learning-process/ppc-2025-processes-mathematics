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
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ZagryadskovMMaxByColumnMPI::ValidationImpl() {
  bool if_dividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  bool res =
      (std::get<0>(GetInput()) > 0) && (!std::get<1>(GetInput()).empty()) && (GetOutput().empty()) && if_dividable;
  return res;
}

bool ZagryadskovMMaxByColumnMPI::PreProcessingImpl() {
  bool if_dividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  bool res = (std::get<0>(GetInput()) > 0) && (!std::get<1>(GetInput()).empty()) && if_dividable;
  return res;
}

bool ZagryadskovMMaxByColumnMPI::RunImpl() {
  bool if_dividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  bool test_data = (std::get<0>(GetInput()) > 0) && (!std::get<1>(GetInput()).empty()) && if_dividable;
  if (!test_data) {
    return false;
  }

  int world_size = 0;
  int world_rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  const auto &n = std::get<0>(GetInput());
  const auto &mat = std::get<1>(GetInput());
  size_t m = mat.size() / n;
  OutType &res = GetOutput();
  OutType local_res;
  OutType columns;
  int columns_count = static_cast<int>(n) / world_size;
  int columns_size = columns_count * static_cast<int>(m);
  using T = std::decay_t<decltype(*mat.begin())>;
  MPI_Datatype datatype = GetMpiType<T>();
  if (datatype == MPI_DATATYPE_NULL) {
    return false;
  }

  columns.resize(columns_size);
  size_t i = 0;
  size_t j = 0;
  T tmp = std::numeric_limits<T>::lowest();
  bool tmp_flag = false;

  res.resize(n, std::numeric_limits<T>::lowest());
  if (columns_size > 0) {
    local_res.resize(columns_count, std::numeric_limits<T>::lowest());
    MPI_Scatter(mat.data(), columns_size, datatype, columns.data(), columns_size, datatype, 0, MPI_COMM_WORLD);

    for (j = 0; std::cmp_less(j, columns_count); ++j) {
      for (i = 0; i < m; ++i) {
        tmp = columns[(j * m) + i];
        tmp_flag = tmp > local_res[j];
        local_res[j] = (static_cast<T>(tmp_flag) * tmp) + (static_cast<T>(!tmp_flag) * local_res[j]);
      }
    }
    MPI_Gather(local_res.data(), columns_count, datatype, res.data(), columns_count, datatype, 0, MPI_COMM_WORLD);
  }
  if (world_rank == 0) {
    for (j = static_cast<size_t>(columns_count) * static_cast<size_t>(world_size); std::cmp_less(j, n); ++j) {
      for (i = 0; i < m; ++i) {
        tmp = mat[(j * m) + i];
        tmp_flag = tmp > res[j];
        res[j] = (static_cast<T>(tmp_flag) * tmp) + (static_cast<T>(!tmp_flag) * res[j]);
      }
    }
  }

  MPI_Bcast(res.data(), static_cast<int>(res.size()), datatype, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
  return !GetOutput().empty();
}

bool ZagryadskovMMaxByColumnMPI::PostProcessingImpl() {
  return !GetOutput().empty();
}

}  // namespace zagryadskov_m_max_by_column
