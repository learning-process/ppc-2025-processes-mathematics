#include "polukhin_v_string_diff/mpi/include/ops_mpi.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>

#include "mpi.h"
#include "polukhin_v_string_diff/common/include/common.hpp"

namespace polukhin_v_string_diff {

StringDiffTaskMPI::StringDiffTaskMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool StringDiffTaskMPI::ValidationImpl() {
  return true;
}

bool StringDiffTaskMPI::PreProcessingImpl() {
  return true;
}

bool StringDiffTaskMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &input = GetInput();
  const std::string &str1 = input.first;
  const std::string &str2 = input.second;

  int min_len = static_cast<int>(std::min(str1.size(), str2.size()));
  int length_diff = std::abs(static_cast<int>(str1.size()) - static_cast<int>(str2.size()));

  int local_count = 0;

  if (min_len > 0) {
    int calc_num = (min_len + size - 1) / size;
    int start = rank * calc_num;
    int end = std::min(start + calc_num, min_len);

    for (int i = start; i < end; ++i) {
      if (str1[i] != str2[i]) {
        ++local_count;
      }
    }
  }

  int total_count = 0;
  MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    total_count += length_diff;
    GetOutput() = static_cast<size_t>(total_count);
  }

  size_t result = (rank == 0) ? GetOutput() : 0;
  MPI_Bcast(&result, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

  if (rank != 0) {
    GetOutput() = result;
  }

  return true;
}

bool StringDiffTaskMPI::PostProcessingImpl() {
  return true;
}

}  // namespace polukhin_v_string_diff
