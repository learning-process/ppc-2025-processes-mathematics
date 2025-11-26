#include "kulik_a_the_most_different_adjacent/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

#include "kulik_a_the_most_different_adjacent/common/include/common.hpp"

namespace kulik_a_the_most_different_adjacent {

KulikATheMostDifferentAdjacentMPI::KulikATheMostDifferentAdjacentMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  int proc_rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
  if (proc_rank == 0) {
    GetInput() = in;
  } else {
    GetInput() = InType{};
  }
}

bool KulikATheMostDifferentAdjacentMPI::ValidationImpl() {
  int proc_rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
  if (proc_rank == 0) {
    return (GetInput().size() >= 2);
  }
  return true;
}

bool KulikATheMostDifferentAdjacentMPI::PreProcessingImpl() {
  return true;
}

bool KulikATheMostDifferentAdjacentMPI::RunImpl() {
  int proc_num = 0;
  int proc_rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
  const auto &input = GetInput();
  auto n = static_cast<uint64_t>(input.size());
  MPI_Bcast(&n, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
  MPI_Status status;
  uint64_t active_procs = std::min(n, static_cast<uint64_t>(proc_num));
  uint64_t size = (active_procs > 0) ? n / active_procs : 0;
  uint64_t r = (active_procs > 0) ? n % active_procs : 0;
  std::vector<int> elemcnt(proc_num, 0);   // необходимы быть типа int из-за сигнатуры функции MPI_Scatterv
  std::vector<int> startpos(proc_num, 0);  //
  if (proc_rank == 0) {
    uint64_t offset = 0;
    for (uint64_t i = 0; i < static_cast<uint64_t>(proc_num); ++i) {
      if (i < active_procs) {
        elemcnt[i] = (i < r) ? static_cast<int>(size + 1) : static_cast<int>(size);
        startpos[i] = offset;
        offset += elemcnt[i];
      } else {
        elemcnt[i] = 0;
        startpos[i] = 0;
      }
    }
  }
  MPI_Bcast(elemcnt.data(), proc_num, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(startpos.data(), proc_num, MPI_INT, 0, MPI_COMM_WORLD);
  std::vector<double> buf(elemcnt[proc_rank]);
  MPI_Scatterv(input.data(), elemcnt.data(), startpos.data(), MPI_DOUBLE, buf.data(), elemcnt[proc_rank], MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
  double max_diff_val = 0.0;
  uint64_t max_diff_ind = 0;
  double max_diffall_val = 0.0;
  uint64_t max_diffall_ind = 0;
  if (elemcnt[proc_rank] >= 2) {
    for (int i = 0; i < elemcnt[proc_rank] - 1; ++i) {
      if (std::abs(buf[i + 1] - buf[i]) > max_diff_val) {
        max_diff_val = std::abs(buf[i + 1] - buf[i]);
        max_diff_ind = startpos[proc_rank] + i;
      }
    }
  }
  if (elemcnt[proc_rank] > 0) {
    double temp = 0.;
    if (proc_rank > 0 && elemcnt[proc_rank - 1] > 0) {
      MPI_Recv(&temp, 1, MPI_DOUBLE, proc_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      if (std::abs(buf[0] - temp) > max_diff_val) {
        max_diff_val = std::abs(buf[0] - temp);
        max_diff_ind = startpos[proc_rank] - 1;
      }
    }
    if (proc_rank < proc_num - 1 && elemcnt[proc_rank + 1] > 0) {
      MPI_Send(&buf.back(), 1, MPI_DOUBLE, proc_rank + 1, 0, MPI_COMM_WORLD);
    }
  }
  MPI_Allreduce(&max_diff_val, &max_diffall_val, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  uint64_t poss_index = (std::abs(max_diff_val - max_diffall_val) < 1e-12) ? max_diff_ind : UINT64_MAX;
  MPI_Allreduce(&poss_index, &max_diffall_ind, 1, MPI_UINT64_T, MPI_MIN, MPI_COMM_WORLD);
  OutType &ans = GetOutput();
  ans.first = max_diffall_ind;
  ans.second = max_diffall_ind + 1;

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool KulikATheMostDifferentAdjacentMPI::PostProcessingImpl() {
  return (GetOutput().first + 1 == GetOutput().second);
}

}  // namespace kulik_a_the_most_different_adjacent
