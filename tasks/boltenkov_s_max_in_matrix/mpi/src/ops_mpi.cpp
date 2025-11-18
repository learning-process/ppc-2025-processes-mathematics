#include <mpi.h>
#include <numeric>
#include <vector>

#include "boltenkov_s_max_in_matrix/mpi/include/ops_mpi.hpp"
#include "boltenkov_s_max_in_matrix/common/include/common.hpp"
#include "util/include/util.hpp"

namespace boltenkov_s_max_in_matrix {

BoltenkovSMaxInMatrixkMPI::BoltenkovSMaxInMatrixkMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::numeric_limits<double>::lowest();
}

bool BoltenkovSMaxInMatrixkMPI::ValidationImpl() {
  return std::abs(GetOutput() + std::numeric_limits<double>::lowest()) < 1e-14 && 
         std::get<0>(GetInput()) > 0 && !std::get<1>(GetInput()).empty() &&
         std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
}

bool BoltenkovSMaxInMatrixkMPI::PreProcessingImpl() {
  return std::get<0>(GetInput()) > 0 && !std::get<1>(GetInput()).empty() &&
         std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
}

bool BoltenkovSMaxInMatrixkMPI::RunImpl() {
  if (!(std::get<0>(GetInput()) > 0 && !std::get<1>(GetInput()).empty() &&
      std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0)) {
    return false;
  }

  int size = 0;
  int rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  std::vector<int> sendcounts(size, 0);
  std::vector<int> displs(size, 0);
  std::vector<double> data;
  std::vector<double> all_maxs(size, std::numeric_limits<double>::lowest());

  OutType& mx = GetOutput();
  std::vector<double>& v = std::get<1>(GetInput());

  int len = v.size();
  int cnt_item = len / size;
  int r = len % size;

  int cur_disp = 0;
  for (int i = 0; i < size; ++i)
  {
    int cur_cnt = cnt_item + (i < r ? 1 : 0);
    sendcounts[r] = cur_cnt;
    displs[i] = cur_disp;
    cur_disp += sendcounts[i];
  }

  MPI_Datatype datatype = MPI_DOUBLE;

  std::vector<double> mx_elem(1);
  data.resize(sendcounts[rank]);
  if (rank == 0)
  {
    MPI_Scatterv(v.data(), sendcounts.data(), displs.data(), datatype, data.data(), sendcounts[rank],
                 datatype, 0, MPI_COMM_WORLD);
  }
  else
  {
    MPI_Scatterv(nullptr, sendcounts.data(), displs.data(), datatype, data.data(), sendcounts[rank],
                 datatype, 0, MPI_COMM_WORLD);
  }

  bool flag;
  OutType tmp_mx;
  for (int i = 0; i < sendcounts[rank]; ++i)
  {
    flag = data[i] > tmp_mx;
    tmp_mx = static_cast<double>(flag) * data[i] + (1. - static_cast<double>(flag)) * tmp_mx;
  }

  if (rank == 0)
  {
    for (int i = 0; i < size; ++i)
    {
      sendcounts[i] = 1;
      displs[i] = i;
    }
  }

  MPI_Gatherv(&tmp_mx, 1, datatype,
              all_maxs.data(), sendcounts.data(), displs.data(), datatype,
              0, MPI_COMM_WORLD);
  
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0)
  {
    for (int i = 0; i < size; ++i)
    {
      flag = all_maxs[i] > tmp_mx;
      mx = static_cast<double>(flag) * all_maxs[i] + (1. - static_cast<double>(flag)) * mx;
    }
  }

  return std::abs(GetOutput() + std::numeric_limits<double>::lowest()) > 1e-14;
}

bool BoltenkovSMaxInMatrixkMPI::PostProcessingImpl() {
  return std::abs(GetOutput() + std::numeric_limits<double>::lowest()) > 1e-14;
}

}  // namespace boltenkov_s_max_in_matrix
