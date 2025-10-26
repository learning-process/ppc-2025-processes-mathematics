#pragma once

#include "chernykh_s_integration_rectangle_method/common/include/common.hpp"
//#include "task/include/task.hpp" // мб не надо?

namespace chernykh_s_integration_rectangle_method {

class ChernykhSIntegrationRectangleMethodSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit ChernykhSIntegrationRectangleMethodSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace chernykh_s_integration_rectangle_method
