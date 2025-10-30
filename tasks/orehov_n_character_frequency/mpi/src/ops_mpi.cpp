#include "orehov_n_character_frequency/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <string>
#include <tuple>

#include "orehov_n_character_frequency/common/include/common.hpp"

namespace orehov_n_character_frequency {

OrehovNCharacterFrequencyMPI::OrehovNCharacterFrequencyMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool OrehovNCharacterFrequencyMPI::ValidationImpl() {
  return (!std::get<0>(GetInput()).empty()) && (std::get<1>(GetInput()).length() == 1);
}

bool OrehovNCharacterFrequencyMPI::PreProcessingImpl() {
  return true;
}

bool OrehovNCharacterFrequencyMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::string str = std::get<0>(GetInput());
  std::string symbol = std::get<1>(GetInput());
  int length = static_cast<int>(str.length());

  int part_size = length / size;
  int remains = length % size;

  int start = (rank * part_size) + std::min(rank, remains);
  int end = ((rank + 1) * part_size) + std::min(rank + 1, remains);

  int local_result = 0;

  for (int i = start; i < end; i++) {
    if (str[i] == symbol[0]) {
      local_result++;
    }
  }

  int global_result = 0;

  MPI_Allreduce(&local_result, &global_result, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  GetOutput() = global_result;

  return true;
}

bool OrehovNCharacterFrequencyMPI::PostProcessingImpl() {
  return true;
}

}  // namespace orehov_n_character_frequency
