#pragma once

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"

namespace kapanova_s_min_of_matrix_elements {

class KapanovaSMinOfMatrixElementsSEQ : public BaseTask {
 public:
  explicit KapanovaSMinOfMatrixElementsSEQ(const InType &in);

  static ppc::task::TypeOfTask GetStaticTypeOfTask();

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace kapanova_s_min_of_matrix_elements
