#include "orehov_n_character_frequency/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <iostream>
#include <numeric>
#include <vector>

#include "orehov_n_character_frequency/common/include/common.hpp"
#include "util/include/util.hpp"

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
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::string str = std::get<0>(GetInput());
  std::string symbol = std::get<1>(GetInput());
  int length = str.length();

  int part_size = length / size;
  int remains = length % size;

  int start = rank * part_size + std::min(rank, remains);
  int end = start + part_size + (rank < remains ? 1 : 0);

  int local_result = 0;

  for (int i = start; i < end; i++) {
    if (str[i] == symbol[0]) {
      local_result++;
    }
  }

  int global_result = 0;

  MPI_Reduce(&local_result, &global_result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&global_result, 1, MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = global_result;

  return true;
}

bool OrehovNCharacterFrequencyMPI::PostProcessingImpl() {
  return true;
}

}  // namespace orehov_n_character_frequency
