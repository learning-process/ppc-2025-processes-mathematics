#pragma once

#include "task/include/task.hpp"
#include "zagryadskov_m_radix_sort_double_simple_merge/common/include/common.hpp"

namespace zagryadskov_m_radix_sort_double_simple_merge {

class ZagryadskovMRadixSortDoubleSimpleMergeSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ZagryadskovMRadixSortDoubleSimpleMergeSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 public:
  static void foffset(uint8_t *mas, size_t size, size_t offset, uint64_t count[255ull + 1ull]);
  static void radix_sort_LSD(double *mas, size_t size);
};

}  // namespace zagryadskov_m_radix_sort_double_simple_merge
