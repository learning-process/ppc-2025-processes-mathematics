#include <gtest/gtest.h>

#include "boltenkov_s_max_in_matrix/common/include/common.hpp"
#include "boltenkov_s_max_in_matrix/mpi/include/ops_mpi.hpp"
#include "boltenkov_s_max_in_matrix/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace boltenkov_s_max_in_matrix {

class ExampleRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 0;
  InType input_data_{};

  void SetUp() override {
    //input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    output_data = 0.0;
    return false;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ExampleRunPerfTestProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, BoltenkovSMaxInMatrixkMPI, BoltenkovSMaxInMatrixkSEQ>(PPC_ID_boltenkov_s_max_in_matrix);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ExampleRunPerfTestProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ExampleRunPerfTestProcesses, kGtestValues, kPerfTestName);

}  // namespace boltenkov_s_max_in_matrix
