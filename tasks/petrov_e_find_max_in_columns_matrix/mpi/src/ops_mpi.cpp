#include "petrov_e_find_max_in_columns_matrix/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "petrov_e_find_max_in_columns_matrix/common/include/common.hpp"

namespace petrov_e_find_max_in_columns_matrix {

PetrovEFindMaxInColumnsMatrixMPI::PetrovEFindMaxInColumnsMatrixMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = {};
}

bool PetrovEFindMaxInColumnsMatrixMPI::ValidationImpl() {
  return (std::get<0>(GetInput()) * std::get<1>(GetInput()) == static_cast<int>(std::get<2>(GetInput()).size())) &&
         (GetOutput().empty());
}

bool PetrovEFindMaxInColumnsMatrixMPI::PreProcessingImpl() {
  return (std::get<0>(GetInput()) * std::get<1>(GetInput()) == static_cast<int>(std::get<2>(GetInput()).size()));
}

bool PetrovEFindMaxInColumnsMatrixMPI::RunImpl() {
  if ((std::get<0>(GetInput()) * std::get<1>(GetInput()) != static_cast<int>(std::get<2>(GetInput()).size()))) {
    return false;
  }

  auto &n = std::get<0>(GetInput());
  auto &m = std::get<1>(GetInput());
  auto &matrix = std::get<2>(GetInput());
  OutType &res = GetOutput();
  using MatrixElemType = std::remove_reference_t<decltype(matrix[0])>;
  MPI_Datatype mpi_matrix_elem_type = petrov_e_find_max_in_columns_matrix::GetMPIDatatype<MatrixElemType>();
  if (mpi_matrix_elem_type == MPI_DATATYPE_NULL) {
    return false;
  }
  int proc_num = 0;
  int proc_rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
  int i = 0;
  int j = 0;
  MatrixElemType max = NAN;
  res.resize(m);
  int col_num_per_proc = m / proc_num;
  int col_num_wo_proc = m % proc_num;
  int flag = 0;

  if (proc_rank < col_num_wo_proc) {
    flag = 1;
  } else {
    flag = 0;
  }

  int start = (proc_rank * col_num_per_proc) + std::min(proc_rank, col_num_wo_proc);
  int end = start + col_num_per_proc + flag;

  OutType proc_res(m, std::numeric_limits<MatrixElemType>::lowest());

  for (i = start; std::cmp_less(i, end); i++) {
    max = matrix[static_cast<int>(i * n)];
    for (j = 1; j < n; j++) {
      max = std::max(matrix[(i * n) + j], max);
    }
    proc_res[i] = max;
  }

  MPI_Allreduce(proc_res.data(), res.data(), m, mpi_matrix_elem_type, MPI_MAX, MPI_COMM_WORLD);

  return true;
}

bool PetrovEFindMaxInColumnsMatrixMPI::PostProcessingImpl() {
  return true;
}

}  // namespace petrov_e_find_max_in_columns_matrix
