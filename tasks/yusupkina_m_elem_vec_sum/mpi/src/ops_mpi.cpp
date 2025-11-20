#include "yusupkina_m_elem_vec_sum/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "yusupkina_m_elem_vec_sum/common/include/common.hpp"
#include "util/include/util.hpp"

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
  int rank=0;
  int count=0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &count);

  const auto& input_vec = GetInput(); 
  int vec_size=input_vec.size();

  if(vec_size==0){
    GetOutput()=0;
    return true;
  }

  if (count > vec_size) {
    if (rank == 0) {
      GetOutput() = std::accumulate(input_vec.begin(), input_vec.end(), 0LL);
    }
    MPI_Bcast(&GetOutput(), 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    return true;
  }

  int base_size = vec_size/count;  
  int remainder = vec_size%count; 
  int cur_size=base_size +(rank < remainder ? 1: 0);

  int local_start_ind=0;
  for (int i = 0; i < rank; i++) {
    local_start_ind += base_size + (i < remainder ? 1 : 0);
  }

  std::vector<int> local_vec_part(cur_size);
  for (int i = 0; i < cur_size; i++) {
    local_vec_part[i] = input_vec[local_start_ind + i];
  }

  OutType local_sum=std::accumulate(local_vec_part.begin(), local_vec_part.end(), 0LL);

  MPI_Allreduce(&local_sum, &GetOutput(), 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);
  return true;

}

bool YusupkinaMElemVecSumMPI::PostProcessingImpl() {
  return true;
}

}  // namespace yusupkina_m_elem_vec_sum
