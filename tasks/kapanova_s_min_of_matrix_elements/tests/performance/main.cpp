#include <gtest/gtest.h>

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"
#include "kapanova_s_min_of_matrix_elements/mpi/include/ops_mpi.hpp"
#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace kapanova_s_min_of_matrix_elements {

class ExampleRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
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

TEST_P(ExampleRunPerfTestProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KapanovaSMinOfMatrixElementsMPI, KapanovaSMinOfMatrixElementsSEQ>(PPC_SETTINGS_kapanova_s_min_of_matrix_elements);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ExampleRunPerfTestProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ExampleRunPerfTestProcesses, kGtestValues, kPerfTestName);

}  // namespace kapanova_s_min_of_matrix_elements
