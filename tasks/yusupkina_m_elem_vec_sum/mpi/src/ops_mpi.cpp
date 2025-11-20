#include "yusupkina_m_elem_vec_sum/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "util/include/util.hpp"
#include "yusupkina_m_elem_vec_sum/common/include/common.hpp"

namespace yusupkina_m_elem_vec_sum {

YusupkinaMElemVecSumMPI::YusupkinaMElemVecSumMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool YusupkinaMElemVecSumMPI::ValidationImpl() {
  return true;
}

bool YusupkinaMElemVecSumMPI::PreProcessingImpl() {
  GetOutput() = 0;
  return true;
}

bool YusupkinaMElemVecSumMPI::RunImpl() {
  int rank = 0;
  int count = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &count);

  const auto &input_vec = GetInput();
  int vec_size = static_cast<int>(input_vec.size());

  if (vec_size == 0) {
    GetOutput() = 0;
    return true;
  }

  if (count > vec_size) {
    if (rank == 0) {
      GetOutput() = std::accumulate(input_vec.begin(), input_vec.end(), 0LL);
    }
    OutType output = GetOutput();
    MPI_Bcast(&output, 1, MPI_INT64_T, 0, MPI_COMM_WORLD);
    GetOutput() = output;
    return true;
  }

  int base_size = vec_size / count;
  int remainder = vec_size % count;
  int cur_size = base_size + (rank < remainder ? 1 : 0);

  int local_start_ind = 0;
  for (int i = 0; i < rank; i++) {
    local_start_ind += base_size + (i < remainder ? 1 : 0);
  }

  std::vector<int> local_vec_part(cur_size);
  for (int i = 0; i < cur_size; i++) {
    local_vec_part[i] = input_vec[local_start_ind + i];
  }

  OutType local_sum = std::accumulate(local_vec_part.begin(), local_vec_part.end(), 0LL);
  OutType global_sum = 0LL;

  MPI_Allreduce(&local_sum, &global_sum, 1, MPI_INT64_T, MPI_SUM, MPI_COMM_WORLD);
  GetOutput() = global_sum;
  return true;
}

bool YusupkinaMElemVecSumMPI::PostProcessingImpl() {
  return true;
}

}  // namespace yusupkina_m_elem_vec_sum
