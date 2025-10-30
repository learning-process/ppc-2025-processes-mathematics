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

// ... (Ваш конструктор)
ChernykhSMinMatrixElementsMPI::ChernykhSMinMatrixElementsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0.0;
}

bool ChernykhSMinMatrixElementsMPI::ValidationImpl() {
  const size_t stroki = std::get<0>(this->GetInput());
  const size_t stolbci = std::get<1>(this->GetInput());

  const std::vector<double> &matrica = std::get<2>(this->GetInput());

  if (matrica.size() != stroki * stolbci) {
    return false;
  }

  return true;
}

bool ChernykhSMinMatrixElementsMPI::PreProcessingImpl() {
  return true;
}

bool ChernykhSMinMatrixElementsMPI::RunImpl() {
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  size_t stroki = 0;
  size_t stolbci = 0;
  const std::vector<double> *matrica = nullptr;

  if (rank == 0) {
    stroki = std::get<0>(this->GetInput());
    stolbci = std::get<1>(this->GetInput());
    matrica = &std::get<2>(this->GetInput());
  }

  MPI_Bcast(&stroki, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&stolbci, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

  size_t total_elements = stroki * stolbci;

  double local_minimum = std::numeric_limits<double>::max();
  double global_minimum = std::numeric_limits<double>::max();

  if (total_elements == 0) {
    if (rank == 0) {
      GetOutput() = global_minimum;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    return true;
  }

  int razmer_bloka = (int)(total_elements / size);
  int remainder = (int)(total_elements % size);  // остаток, который будет обрабатывать ТОЛЬКО Master.

  std::vector<double> local_data(razmer_bloka);

  MPI_Scatter((rank == 0) ? (void *)matrica->data() : nullptr, razmer_bloka, MPI_DOUBLE, local_data.data(),
              razmer_bloka, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (!local_data.empty()) {
    local_minimum = *std::min_element(local_data.begin(), local_data.end());
  }

  if (rank == 0 && remainder > 0) {
    const double *remainder_start = matrica->data() + (size * razmer_bloka);
    double remainder_min = *std::min_element(remainder_start, remainder_start + remainder);
    local_minimum = std::fmin(local_minimum, remainder_min);
  }

  MPI_Reduce(&local_minimum, &global_minimum, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    GetOutput() = global_minimum;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool ChernykhSMinMatrixElementsMPI::PostProcessingImpl() {
  return true;
}
}  // namespace chernykh_s_min_matrix_elements
