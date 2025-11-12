#include <gtest/gtest.h>

#include "kulik_a_the_most_different_adjacent/common/include/common.hpp"
#include "kulik_a_the_most_different_adjacent/mpi/include/ops_mpi.hpp"
#include "kulik_a_the_most_different_adjacent/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace kulik_a_the_most_different_adjacent {

class KulikATheMostDifferentAdjacentPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
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

TEST_P(KulikATheMostDifferentAdjacentPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KulikATheMostDifferentAdjacentMPI, KulikATheMostDifferentAdjacentSEQ>(PPC_SETTINGS_kulik_a_the_most_different_adjacent);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = KulikATheMostDifferentAdjacentPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KulikATheMostDifferentAdjacentPerfTests, kGtestValues, kPerfTestName);

}  // namespace kulik_a_the_most_different_adjacent
