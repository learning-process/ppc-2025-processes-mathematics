#include "chernykh_s_min_matrix_elements/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>

#include "chernykh_s_min_matrix_elements/common/include/common.hpp"
#include "util/include/util.hpp"

namespace chernykh_s_min_matrix_elements {

ChernykhSMinMatrixElementsMPI::ChernykhSMinMatrixElementsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::numeric_limits<double>::max();
}

bool ChernykhSMinMatrixElementsMPI::ValidationImpl() {
  return (GetOutput() == std::numeric_limits<double>::max());
}

bool ChernykhSMinMatrixElementsMPI::PreProcessingImpl() {
  return true;
}

bool ChernykhSMinMatrixElementsMPI::RunImpl() {
  int rank = 0;
  int size = 0;

  std::vector<double> local_data;

  double local_min = std::numeric_limits<double>::max();
  double global_min = std::numeric_limits<double>::max();

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const std::vector<std::vector<double>> &matrix = GetInput();
  size_t vsego_stolbcov = matrix.size();

  size_t kolvo_na_process = vsego_stolbcov / size;
  size_t ostatok = vsego_stolbcov % size;
  size_t count = kolvo_na_process;

  if (rank == size - 1) {  // распределяем остаток на последний процесс
    count += ostatok;
  }
  size_t start = static_cast<size_t>(rank) * kolvo_na_process;
  size_t end = start + count;

  for (size_t i = start; i < end; i++) {
    for (double element : matrix[i]) {
      local_min = std::min(element, local_min);
    }
  }

  MPI_Allreduce(&local_min, &global_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

  GetOutput() = global_min;

  return true;
}

bool ChernykhSMinMatrixElementsMPI::PostProcessingImpl() {
  return true;
}
}  // namespace chernykh_s_min_matrix_elements
