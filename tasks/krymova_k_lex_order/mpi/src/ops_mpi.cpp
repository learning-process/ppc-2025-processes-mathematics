#include "krymova_k_lex_order/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <climits>
#include <string>
#include <vector>

#include "krymova_k_lex_order/common/include/common.hpp"
#include "util/include/util.hpp"

namespace krymova_k_lex_order {

KrymovaKLexOrderMPI::KrymovaKLexOrderMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KrymovaKLexOrderMPI::ValidationImpl() {
  return GetInput().size() == 2;
}

bool KrymovaKLexOrderMPI::PreProcessingImpl() {
  return true;
}

bool KrymovaKLexOrderMPI::RunImpl() {
  const std::string &str1 = GetInput()[0];
  const std::string &str2 = GetInput()[1];

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  size_t len1 = str1.length();
  size_t len2 = str2.length();
  size_t min_len = std::min(len1, len2);

  size_t chunk_size = (min_len + size - 1) / size;
  size_t start = rank * chunk_size;
  size_t end = std::min(start + chunk_size, min_len);
  bool found_diff = false;
  int local_diff_pos = -1;

  for (size_t i = start; i < end; ++i) {
    if (str1[i] != str2[i]) {
      found_diff = true;
      local_diff_pos = static_cast<int>(i);
      break;
    }
  }
  int local_found = found_diff ? 1 : 0;
  int global_found;
  MPI_Allreduce(&local_found, &global_found, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

  int result = 0;

  if (global_found) {
    int pos_to_send = found_diff ? local_diff_pos : INT_MAX;
    int global_min_pos;
    MPI_Allreduce(&pos_to_send, &global_min_pos, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    result = (str1[global_min_pos] < str2[global_min_pos]) ? -1 : 1;
  } else {
    if (len1 < len2) {
      result = -1;
    } else if (len1 > len2) {
      result = 1;
    }
  }

  GetOutput() = result;
  return true;
}

bool KrymovaKLexOrderMPI::PostProcessingImpl() {
  return true;
}

}  // namespace krymova_k_lex_order
