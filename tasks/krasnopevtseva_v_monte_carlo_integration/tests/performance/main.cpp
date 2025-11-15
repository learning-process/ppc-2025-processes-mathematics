#include <gtest/gtest.h>
#include <cmath>
#include <tuple>

#include "krasnopevtseva_v_monte_carlo_integration/common/include/common.hpp"
#include "krasnopevtseva_v_monte_carlo_integration/mpi/include/ops_mpi.hpp"
#include "krasnopevtseva_v_monte_carlo_integration/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace krasnopevtseva_v_monte_carlo_integration {

class KrasnopevtsevaV_MCIntegrationPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_{};
  double expected_integral;
  double tolerance;
  
  void SetUp() override {
    double a = 0.0;
    double b = 2.0;
    int points = 500000;
    input_data_ = std::make_tuple(a, b, points);
    tolerance = (b - a) / std::sqrt(points)*10;
    expected_integral = (b*b*b - 6*b)*std::sin(b) + (3*b*b - 6)*std::cos(b) - (a*a*a - 6*a)*std::sin(a) - (3*a*a - 6)*std::cos(a);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return std::abs(output_data - expected_integral) < tolerance;
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
