#include <gtest/gtest.h>

#include "chernykh_s_integration_rectangle_method/common/include/common.hpp"
#include "chernykh_s_integration_rectangle_method/mpi/include/ops_mpi.hpp"
#include "chernykh_s_integration_rectangle_method/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace chernykh_s_integration_rectangle_method {

class ChernykhSRunFuncTestsIntegrationRectangleMethod : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 100;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ChernykhSRunFuncTestsIntegrationRectangleMethod, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ChernykhSIntegrationRectangleMethodMPI, ChernykhSIntegrationRectangleMethodSEQ>(PPC_SETTINGS_chernykh_s_integration_rectangle_method);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ChernykhSRunFuncTestsIntegrationRectangleMethod::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ChernykhSRunFuncTestsIntegrationRectangleMethod, kGtestValues, kPerfTestName);

}  // namespace chernykh_s_integration_rectangle_method
