#include "kulik_a_the_most_different_adjacent/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

#include "kulik_a_the_most_different_adjacent/common/include/common.hpp"
#include "util/include/util.hpp"

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
  int ProcNum = 0;
  int ProcRank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
  MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
  const auto &input = GetInput();
  const auto n = input.size();
  MPI_Status status;
  const int r = n % ProcNum;
  int update_n = n;
  if (r) {
    update_n += (ProcNum - r);
  }
  int size = update_n / ProcNum;
  std::vector<int> elemcnt(ProcNum, size);
  elemcnt[ProcNum - 1] = n - size * (ProcNum - 1);
  std::vector<int> startpos(ProcNum);
  for (int i = 0; i < ProcNum; ++i) {
    startpos[i] = i * size;
  }
  std::vector<double> buf(elemcnt[ProcRank]);
  MPI_Scatterv(input.data(), elemcnt.data(), startpos.data(), MPI_DOUBLE, buf.data(), elemcnt[ProcRank], MPI_DOUBLE, 0,
               MPI_COMM_WORLD);
  if (ProcRank == ProcNum - 1 && r) {
    for (int i = 0; i < ProcNum - r; ++i) {
      buf.push_back(input.back());
    }
  }
  struct {
    double val;
    int ind;
  } max_diff, max_diffall;
  max_diff.val = max_diffall.val = 0.;
  max_diff.ind = max_diffall.ind = 0;
  for (int i = 0; i < size - 1; ++i) {
    if (std::abs(buf[i + 1] - buf[i]) > max_diff.val) {
      max_diff.val = std::abs(buf[i + 1] - buf[i]);
      max_diff.ind = ProcRank * size + i;
    }
  }
  double temp = 0.;
  if (ProcRank != ProcNum - 1) {
    MPI_Send(&buf[size - 1], 1, MPI_DOUBLE, ProcRank + 1, 0, MPI_COMM_WORLD);
  }
  if (ProcRank != 0) {
    MPI_Recv(&temp, 1, MPI_DOUBLE, ProcRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  }
  if (ProcRank != 0 && (std::abs(temp - buf[0]) > max_diff.val)) {
    max_diff.val = std::abs(temp - buf[0]);
    max_diff.ind = ProcRank * size - 1;
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
