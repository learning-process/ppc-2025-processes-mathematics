#pragma once

// LCOV_EXCL_START
#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"
#include "task/include/task.hpp"

namespace kapanova_s_min_of_matrix_elements {

class KapanovaSMinOfMatrixElementsSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit KapanovaSMinOfMatrixElementsSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};
// LCOV_EXCL_STOP

}  // namespace kapanova_s_min_of_matrix_elements
