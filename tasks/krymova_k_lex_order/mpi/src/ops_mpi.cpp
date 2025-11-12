#include "krymova_k_lex_order/mpi/include/ops_mpi.hpp"

#include <mpi.h>
#include <algorithm>
#include <string>
#include<vector>

#include "krymova_k_lex_order/common/include/common.hpp"
#include "util/include/util.hpp"

namespace krymova_k_lex_order {

KrymovaKLexOrderMPI::KrymovaKLexOrderMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KrymovaKLexOrderMPI::ValidationImpl() {
  return GetInput().size() == 2 && !GetInput()[0].empty() && !GetInput()[1].empty();
}

bool KrymovaKLexOrderMPI::PreProcessingImpl() {
  return true;
}

bool KrymovaKLexOrderMPI::RunImpl() {
  const std::string& str1 = GetInput()[0];
  const std::string& str2 = GetInput()[1];

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  size_t len1 = str1.length();
  size_t len2 = str2.length();
  size_t min_len = std::min(len1, len2);  
  
  size_t chunk_size = (min_len + size - 1) / size;
  size_t start = rank * chunk_size;
  size_t end = std::min(start + chunk_size, min_len);

  int local_difference_pos = -1;
  for (size_t i = start; i < end; ++i) {
    if (str1[i] != str2[i]) {
      local_difference_pos = static_cast<int>(i);
      break;
    }
  }

  int global_difference_pos;
  MPI_Allreduce(&local_difference_pos, &global_difference_pos, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
  if (global_difference_pos != -1) {
    if (rank == 0) {
      char char1 = str1[global_difference_pos];
      char char2 = str2[global_difference_pos];
      GetOutput() = (char1 < char2) ? -1 : 1;
    }
    MPI_Bcast(&GetOutput(), 1, MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    if (len1 < len2) {
      GetOutput() = -1;
    } else if (len1 > len2) {
      GetOutput() = 1;
    } else {
      GetOutput() = 0;
    }
  }

  return true;
}

bool KrymovaKLexOrderMPI::PostProcessingImpl() {
  return true;
}

}  // namespace krymova_k_lex_order
