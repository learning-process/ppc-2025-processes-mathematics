#include "kulik_a_the_most_different_adjacent/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <utility>
#include <vector>

#include "kulik_a_the_most_different_adjacent/common/include/common.hpp"

namespace kulik_a_the_most_different_adjacent {

KulikATheMostDifferentAdjacentMPI::KulikATheMostDifferentAdjacentMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool KulikATheMostDifferentAdjacentMPI::ValidationImpl() {
  return (GetInput().size() >= 2);
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
  const auto n = input.size();
  MPI_Status status;
  const int r = static_cast<int>(n % proc_num);
  int update_n = static_cast<int>(n);
  if (r != 0) {
    update_n += (proc_num - r);
  }
  int size = update_n / proc_num;
  std::vector<int> elemcnt(proc_num, size);
  elemcnt[proc_num - 1] = static_cast<int>(n) - (size * (proc_num - 1));
  std::vector<int> startpos(proc_num);
  for (int i = 0; i < proc_num; ++i) {
    startpos[i] = i * size;
  }
  std::vector<double> buf(elemcnt[proc_rank]);
  MPI_Scatterv(input.data(), elemcnt.data(), startpos.data(), MPI_DOUBLE, buf.data(), elemcnt[proc_rank], MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
  if (proc_rank == proc_num - 1 && r != 0) {
    for (int i = 0; i < proc_num - r; ++i) {
      buf.push_back(input.back());
    }
  }
  struct {
    double val = 0.0;
    int ind = 0;
  } max_diff, max_diffall;

  for (int i = 0; i < size - 1; ++i) {
    if (std::abs(buf[i + 1] - buf[i]) > max_diff.val) {
      max_diff.val = std::abs(buf[i + 1] - buf[i]);
      max_diff.ind = (proc_rank * size) + i;
    }
  }
  double temp = 0.;
  if (proc_rank != proc_num - 1) {
    MPI_Send(&buf[size - 1], 1, MPI_DOUBLE, proc_rank + 1, 0, MPI_COMM_WORLD);
  }
  if (proc_rank != 0) {
    MPI_Recv(&temp, 1, MPI_DOUBLE, proc_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }
  if (proc_rank != 0 && (std::abs(temp - buf[0]) > max_diff.val)) {
    max_diff.val = std::abs(temp - buf[0]);
    max_diff.ind = (proc_rank * size) - 1;
  }
  MPI_Allreduce(&max_diff, &max_diffall, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
  OutType &ans = GetOutput();
  ans.first = max_diffall.ind;
  ans.second = max_diffall.ind + 1;
  MPI_Barrier(MPI_COMM_WORLD);

  return true;
}

bool KulikATheMostDifferentAdjacentMPI::PostProcessingImpl() {
  return (GetOutput().first >= 0 && GetOutput().second > GetOutput().first);
}

}  // namespace kulik_a_the_most_different_adjacent
