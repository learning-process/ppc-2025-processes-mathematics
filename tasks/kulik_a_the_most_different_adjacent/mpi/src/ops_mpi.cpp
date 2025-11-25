#include "kulik_a_the_most_different_adjacent/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
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
  uint64_t r;
  uint64_t update_n;
  uint64_t size;
  if (proc_rank == 0) {
    r = n % static_cast<uint64_t>(proc_num);
    update_n = n;
    if (r != 0) {
      update_n += (static_cast<uint64_t>(proc_num) - r);
    }
    size = update_n / static_cast<uint64_t>(proc_num);
  }
  MPI_Bcast(&r, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
  MPI_Bcast(&update_n, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
  MPI_Bcast(&size, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
  std::vector<int> elemcnt(proc_num,
                           static_cast<int>(size));  // необходимы быть типа int из-за сигнатуры функции MPI_Scatterv
  std::vector<int> startpos(proc_num);               //
  int64_t last_size = static_cast<int>(n) - static_cast<int>(size * (proc_num - 1));
  if (last_size > 0) {
    elemcnt[proc_num - 1] = static_cast<int>(last_size);
  } else {
    elemcnt[proc_num - 1] = 0;
  }
  for (int i = 0; i < proc_num; ++i) {
    startpos[i] = static_cast<int>(i * size);
  }
  std::vector<double> buf;
  if (elemcnt[proc_rank] > 0) {
    buf.resize(elemcnt[proc_rank]);
  }
  MPI_Scatterv(input.data(), elemcnt.data(), startpos.data(), MPI_DOUBLE, buf.data(), elemcnt[proc_rank], MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
  if (proc_rank == proc_num - 1 && r != 0 && !buf.empty()) {
    for (uint64_t i = 0; i < static_cast<uint64_t>(proc_num) - r; ++i) {
      buf.push_back(buf.back());
    }
  }
  double max_diff_val = 0.0;
  uint64_t max_diff_ind = 0;
  double max_diffall_val = 0.0;
  uint64_t max_diffall_ind = 0;
  for (uint64_t i = 0; i < size - (uint64_t)1; ++i) {
    if (std::abs(buf[i + 1] - buf[i]) > max_diff_val) {
      max_diff_val = std::abs(buf[i + 1] - buf[i]);
      max_diff_ind = (proc_rank * size) + i;
    }
  }
  double temp = 0.;
  if (proc_rank != 0) {
    MPI_Recv(&temp, 1, MPI_DOUBLE, proc_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }
  if (proc_rank != proc_num - 1) {
    MPI_Send(&buf[size - 1], 1, MPI_DOUBLE, proc_rank + 1, 0, MPI_COMM_WORLD);
  }
  if (proc_rank != 0 && (std::abs(temp - buf[0]) > max_diff_val)) {
    max_diff_val = std::abs(temp - buf[0]);
    max_diff_ind = (proc_rank * size) - 1;
  }
  MPI_Allreduce(&max_diff_val, &max_diffall_val, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  int64_t poss_index;
  if (std::abs(max_diff_val - max_diffall_val) < 1e-12) {
    poss_index = max_diff_ind;
  } else {
    poss_index = -1;
  }
  MPI_Allreduce(&poss_index, &max_diffall_ind, 1, MPI_UINT64_T, MPI_MAX, MPI_COMM_WORLD);
  OutType &ans = GetOutput();
  ans.first = max_diffall_ind;
  ans.second = max_diffall_ind + 1;
  MPI_Barrier(MPI_COMM_WORLD);

  return true;
}

bool KulikATheMostDifferentAdjacentMPI::PostProcessingImpl() {
  return (GetOutput().second == (GetOutput().first + 1));
}

}  // namespace kulik_a_the_most_different_adjacent
