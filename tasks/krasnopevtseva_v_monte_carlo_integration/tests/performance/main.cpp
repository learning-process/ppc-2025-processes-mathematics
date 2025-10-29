#include <gtest/gtest.h>

#include "krasnopevtseva_v_monte_carlo_integration/common/include/common.hpp"
#include "krasnopevtseva_v_monte_carlo_integration/mpi/include/ops_mpi.hpp"
#include "krasnopevtseva_v_monte_carlo_integration/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace krasnopevtseva_v_monte_carlo_integration {

class KrasnopevtsevaV_MCIntegrationPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
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

TEST_P(KrasnopevtsevaV_MCIntegrationPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KrasnopevtsevaV_MCIntegrationMPI, KrasnopevtsevaV_MCIntegrationSEQ>(PPC_SETTINGS_krasnopevtseva_v_monte_carlo_integration);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = KrasnopevtsevaV_MCIntegrationPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KrasnopevtsevaV_MCIntegrationPerfTests, kGtestValues, kPerfTestName);

}  // namespace krasnopevtseva_v_monte_carlo_integration
