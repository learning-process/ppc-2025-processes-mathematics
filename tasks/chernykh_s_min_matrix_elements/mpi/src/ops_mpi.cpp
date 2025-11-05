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
  const size_t stroki = std::get<0>(GetInput());
  const size_t stolbci = std::get<1>(GetInput());

  const std::vector<double> &matrica = std::get<2>(GetInput());

  if (matrica.size() != stroki * stolbci) {
    return false;
  }

  if (stroki == 0 || stolbci == 0) {
    return false;
  }

  if (matrica.empty()) {
    return false;
  }
  return true;
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

  // std::cout<<"rank = "<<rank<<std::endl<<"size = "<<size<<std::endl;

  const size_t &stroki = std::get<0>(GetInput());
  const size_t &stolbci = std::get<1>(GetInput());
  const std::vector<double> &matrica = std::get<2>(GetInput());
  const size_t total_elements = stroki * stolbci;

  size_t kolvo_na_process = total_elements / size;
  size_t ostatok = total_elements % size;
  size_t count = kolvo_na_process;
  if (rank == 0) {
    count += ostatok;
  }

  // std::cout<<"kolvo_na_process = "<<kolvo_na_process<<std::endl<<"ostatok = "<<ostatok<<std::endl;

  size_t start = rank * kolvo_na_process;
  if (rank > 0) {
    start += ostatok;
  }
  size_t end = start + count;
  end += (rank == size) ? ostatok : 0;

  for (size_t i = start; i < end; ++i) {
    if (matrica[i] < local_min) {
      local_min = matrica[i];
    }
  }

  // std::cout<<"local_min = "<<local_min<<std::endl<<"global_min = "<<global_min<<std::endl;
  MPI_Reduce(&local_min, &global_min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    GetOutput() = global_min;
  }

  return true;
}

bool ChernykhSMinMatrixElementsMPI::PostProcessingImpl() {
  return true;
}
}  // namespace chernykh_s_min_matrix_elements
