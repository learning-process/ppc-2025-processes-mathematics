#include "zagryadskov_m_max_by_column/mpi/include/max_by_column.hpp"

#include <mpi.h>

#include <algorithm>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>

#include "util/include/util.hpp"
#include "zagryadskov_m_max_by_column/common/include/common.hpp"

namespace zagryadskov_m_max_by_column {

ZagryadskovMMaxByColumnMPI::ZagryadskovMMaxByColumnMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ZagryadskovMMaxByColumnMPI::ValidationImpl() {
  bool ifDividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  bool res =
      (std::get<0>(GetInput()) > 0) && (std::get<1>(GetInput()).size() > 0) && (GetOutput().size() == 0) && ifDividable;
  return res;
}

bool ZagryadskovMMaxByColumnMPI::PreProcessingImpl() {
  bool ifDividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  bool res = (std::get<0>(GetInput()) > 0) && (std::get<1>(GetInput()).size() > 0) && ifDividable;
  return res;
}

bool ZagryadskovMMaxByColumnMPI::RunImpl() {
  bool ifDividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  bool testData = (std::get<0>(GetInput()) > 0) && (std::get<1>(GetInput()).size() > 0) && ifDividable;
  if (!testData) {
    return false;
  }

  int world_size = 0, world_rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  const auto &n = std::get<0>(GetInput());
  const auto &mat = std::get<1>(GetInput());
  size_t m = mat.size() / n;
  OutType &res = GetOutput();
  OutType local_res;
  OutType columns;
  int columns_count = int(n) / world_size;
  int columns_size = columns_count * int(m);
  using T = std::decay_t<decltype(*mat.begin())>;
  MPI_Datatype datatype;
  if (std::is_same<T, char>::value) {
    datatype = MPI_CHAR;
  } else if (std::is_same<T, unsigned char>::value) {
    datatype = MPI_UNSIGNED_CHAR;
  } else if (std::is_same<T, short>::value) {
    datatype = MPI_SHORT;
  } else if (std::is_same<T, unsigned short>::value) {
    datatype = MPI_UNSIGNED_SHORT;
  } else if (std::is_same<T, int>::value) {
    datatype = MPI_INT;
  } else if (std::is_same<T, unsigned>::value) {
    datatype = MPI_UNSIGNED;
  } else if (std::is_same<T, long>::value) {
    datatype = MPI_LONG;
  } else if (std::is_same<T, unsigned long>::value) {
    datatype = MPI_UNSIGNED_LONG;
  } else if (std::is_same<T, long long>::value) {
    datatype = MPI_LONG_LONG;
  } else if (std::is_same<T, float>::value) {
    datatype = MPI_FLOAT;
  } else if (std::is_same<T, double>::value) {
    datatype = MPI_DOUBLE;
  } else {
    return false;
  }

  columns.resize(columns_size);

  res.resize(n, std::numeric_limits<T>::lowest());
  local_res.resize(columns_count, std::numeric_limits<T>::lowest());
  MPI_Scatter(mat.data(), columns_size, datatype, columns.data(), columns_size, datatype, 0, MPI_COMM_WORLD);

  size_t i, j;
  T tmp;
  int tmpFlag;
  for (j = 0; j < size_t(columns_count); ++j) {
    for (i = 0; i < m; ++i) {
      tmp = columns[j * m + i];
      tmpFlag = tmp > local_res[j];
      local_res[j] = tmpFlag * tmp + (!tmpFlag) * local_res[j];
    }
  }
  MPI_Gather(local_res.data(), columns_count, datatype, res.data(), columns_count, datatype, 0, MPI_COMM_WORLD);
  if (world_rank == 0) {
    for (j = size_t(columns_count * world_size); j < n; ++j) {
      for (i = 0; i < m; ++i) {
        tmp = mat[j * m + i];
        tmpFlag = tmp > res[j];
        res[j] = tmpFlag * tmp + (!tmpFlag) * res[j];
      }
    }
  }

  MPI_Bcast(res.data(), res.size(), datatype, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
  return GetOutput().size() > 0;
}

bool ZagryadskovMMaxByColumnMPI::PostProcessingImpl() {
  return GetOutput().size() > 0;
}

}  // namespace zagryadskov_m_max_by_column
