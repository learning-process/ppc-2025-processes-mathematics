#include <gtest/gtest.h>

#include "lopatin_a_scalar_mult/common/include/common.hpp"
#include "lopatin_a_scalar_mult/mpi/include/ops_mpi.hpp"
#include "lopatin_a_scalar_mult/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace lopatin_a_scalar_mult {

class LopatinAScalarMultPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
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

TEST_P(LopatinAScalarMultPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, LopatinAScalarMultMPI, LopatinAScalarMultSEQ>(
    PPC_SETTINGS_lopatin_a_scalar_mult);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = LopatinAScalarMultPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, LopatinAScalarMultPerfTests, kGtestValues, kPerfTestName);

}  // namespace lopatin_a_scalar_mult
