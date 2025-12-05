#pragma once

#include <mpi.h>

#include <vector>

#include "task/include/task.hpp"
#include "zagryadskov_m_radix_sort_double_simple_merge/common/include/common.hpp"

namespace zagryadskov_m_radix_sort_double_simple_merge {

class ZagryadskovMRadixSortDoubleSimpleMergeMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ZagryadskovMRadixSortDoubleSimpleMergeMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
  static bool SecondPhase(int m, int n, int world_size, int world_rank, std::vector<int> &sendcounts,
                          std::vector<int> &displs, OutType &res, OutType &local_res, MPI_Datatype datatype);
};

}  // namespace zagryadskov_m_radix_sort_double_simple_merge
