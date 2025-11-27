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
  size_t len1 = str1.size();
  size_t len2 = str2.size();
  size_t length_diff = std::max<size_t>(len1, len2) - std::min<size_t>(len1, len2);

  size_t local_count = 0;

  if (min_len > 0) {
    size_t els_per_process = (min_len + size - 1) / size;
    size_t start = rank * els_per_process;
    size_t end = std::min<size_t>(start + els_per_process, min_len);

    for (size_t i = start; i < end; ++i) {
      if (str1[i] != str2[i]) {
        ++local_count;
      }
    }
  }

  size_t total_count = 0;
  MPI_Reduce(&local_count, &total_count, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    total_count += length_diff;
    GetOutput() = total_count;
  }

  size_t result = (rank == 0) ? GetOutput() : 0;
  MPI_Bcast(&result, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

  if (rank != 0) {
    GetOutput() = result;
  }

  return true;
}

bool StringDiffTaskMPI::PostProcessingImpl() {
  return true;
}

}  // namespace polukhin_v_string_diff
