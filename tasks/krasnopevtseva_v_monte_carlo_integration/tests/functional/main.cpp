#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <string>
#include <tuple>

#include "krasnopevtseva_v_monte_carlo_integration/common/include/common.hpp"
#include "krasnopevtseva_v_monte_carlo_integration/mpi/include/ops_mpi.hpp"
#include "krasnopevtseva_v_monte_carlo_integration/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace krasnopevtseva_v_monte_carlo_integration {

class KrasnopevtsevaVMCIntegrationFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    const auto &params = std::get<0>(test_param);
    double a = std::get<0>(params);
    double b = std::get<1>(params);
    int points = std::get<2>(params);

    std::string result = "a" + std::to_string(a) + "_b" + std::to_string(b) + "_points" + std::to_string(points) + "_" +
                         std::get<1>(test_param);

    std::ranges::replace(result, '-', 'n');
    std::ranges::replace(result, '.', '_');

    return result;
  }

 private:
  InType input_data_;
  double expected_integral_{};

 protected:
  void SetUp() override {
    auto test_param = std::get<static_cast<size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = std::get<0>(test_param);

    double a = std::get<0>(input_data_);
    double b = std::get<1>(input_data_);

    expected_integral_ = ((b * b * b - 6 * b) * std::sin(b)) + ((3 * b * b - 6) * std::cos(b)) -
                         ((a * a * a - 6 * a) * std::sin(a)) - ((3 * a * a - 6) * std::cos(a));
  }

  bool CheckTestOutputData(OutType &output_data) final {
    double a = std::get<0>(input_data_);
    double b = std::get<1>(input_data_);
    int points = std::get<2>(input_data_);
    double tolerance = (b - a) / std::sqrt(points) * 10;
    if ((a <= -3.0) || (b >= 3.0)) {
      tolerance *= 10;
    }
    bool result = std::abs(output_data - expected_integral_) <= tolerance;
    return result;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

namespace {

TEST_P(KrasnopevtsevaVMCIntegrationFuncTests, MatmulFromPic) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 6> kTestParam = {
    std::make_tuple(std::make_tuple(0.0, 1.0, 1000), "small_interval_few_points"),
    std::make_tuple(std::make_tuple(0.0, 1.0, 3333), "odd_numper_of_points"),
    std::make_tuple(std::make_tuple(0.0, 2.3, 10000), "medium_interval_medium_points"),
    std::make_tuple(std::make_tuple(0.0, 3.0, 400000), "large_interval_many_points"),
    std::make_tuple(std::make_tuple(-1.0, 1.0, 50000), "simmerty_interval"),
    std::make_tuple(std::make_tuple(0.0, 1.0, 20000), "small_range_high_precision"),
};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<KrasnopevtsevaVMCIntegrationMPI, InType>(
                                               kTestParam, PPC_SETTINGS_krasnopevtseva_v_monte_carlo_integration),
                                           ppc::util::AddFuncTask<KrasnopevtsevaVMCIntegrationSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_krasnopevtseva_v_monte_carlo_integration));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    KrasnopevtsevaVMCIntegrationFuncTests::PrintFuncTestName<KrasnopevtsevaVMCIntegrationFuncTests>;

INSTANTIATE_TEST_SUITE_P(MCIntegrationTests, KrasnopevtsevaVMCIntegrationFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace krasnopevtseva_v_monte_carlo_integration
