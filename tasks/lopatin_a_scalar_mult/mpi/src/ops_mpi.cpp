#include "lopatin_a_scalar_mult/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cstddef>
#include <utility>
#include <vector>

#include "lopatin_a_scalar_mult/common/include/common.hpp"

namespace lopatin_a_scalar_mult {

LopatinAScalarMultMPI::LopatinAScalarMultMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0.0;
}

bool LopatinAScalarMultMPI::ValidationImpl() {
  return (!GetInput().first.empty() && !GetInput().second.empty()) &&
         (GetInput().first.size() == GetInput().second.size());
}

bool LopatinAScalarMultMPI::PreProcessingImpl() {
  GetOutput() = 0.0;
  return (GetOutput() == 0.0);
}

bool LopatinAScalarMultMPI::RunImpl() {
  int proc_num = 0;
  int proc_rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

  const auto &input = GetInput();
  const auto n = input.first.size();
  OutType &total_res = GetOutput();

  int local_n = static_cast<int>(n / proc_num);

  if (local_n > 0) {
    InType local_data = std::make_pair(std::vector<double>(local_n), std::vector<double>(local_n));

    MPI_Scatter(input.first.data(), local_n, MPI_DOUBLE, local_data.first.data(), local_n, MPI_DOUBLE, 0,
                MPI_COMM_WORLD);
    MPI_Scatter(input.second.data(), local_n, MPI_DOUBLE, local_data.second.data(), local_n, MPI_DOUBLE, 0,
                MPI_COMM_WORLD);

    OutType proc_res{};
    for (int i = 0; i < local_n; ++i) {
      proc_res += local_data.first[i] * local_data.second[i];
    }

    MPI_Reduce(&proc_res, &total_res, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  }

  if (proc_rank == 0) {
    if (n % proc_num != 0) {
      size_t tail_index = n - (n % proc_num);
      for (size_t i = tail_index; i < n; ++i) {
        total_res += input.first[i] * input.second[i];
      }
    }
  }

  MPI_Bcast(&total_res, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  return true;
}

bool LopatinAScalarMultMPI::PostProcessingImpl() {
  return true;
}

}  // namespace lopatin_a_scalar_mult
