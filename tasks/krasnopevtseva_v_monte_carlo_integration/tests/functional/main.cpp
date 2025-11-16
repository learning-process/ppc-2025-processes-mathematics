#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "krasnopevtseva_v_monte_carlo_integration/common/include/common.hpp"
#include "krasnopevtseva_v_monte_carlo_integration/mpi/include/ops_mpi.hpp"
#include "krasnopevtseva_v_monte_carlo_integration/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace krasnopevtseva_v_monte_carlo_integration {

class KrasnopevtsevaV_MCIntegrationFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    const auto &params = std::get<0>(test_param);
    double a = std::get<0>(params);
    double b = std::get<1>(params);
    int points = std::get<2>(params);

    std::string result = "a" + std::to_string(a) + "_b" + std::to_string(b) + "_points" + std::to_string(points) + "_" +
                         std::get<1>(test_param);

    std::replace(result.begin(), result.end(), '-', 'n');
    std::replace(result.begin(), result.end(), '.', '_');

    return result;
  }

 private:
  InType input_data_;
  double expected_integral;

 protected:
  void SetUp() override {
    auto test_param = std::get<static_cast<size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = std::get<0>(test_param);

    double a = std::get<0>(input_data_);
    double b = std::get<1>(input_data_);

    expected_integral = (b * b * b - 6 * b) * std::sin(b) + (3 * b * b - 6) * std::cos(b) -
                        (a * a * a - 6 * a) * std::sin(a) - (3 * a * a - 6) * std::cos(a);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    double a = std::get<0>(input_data_);
    double b = std::get<1>(input_data_);
    int points = std::get<2>(input_data_);
#ifdef BUILD_MPI
    // MPI проверяем только на процессе 0
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank != 0) {
      return true;
    }
#endif
    double tolerance = (b - a) / std::sqrt(points) * 10;
    bool result = std::abs(output_data - expected_integral) <= tolerance;
    std::cout << "Result out of tolerance: diff=" << std::abs(output_data - expected_integral)
              << ", tolerance=" << tolerance << std::endl;
    return result;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

namespace {

TEST_P(KrasnopevtsevaV_MCIntegrationFuncTests, MatmulFromPic) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 5> kTestParam = {
    std::make_tuple(std::make_tuple(0.0, 1.0, 1000), "small_interval_few_points"),
    std::make_tuple(std::make_tuple(0.0, 2.3, 10000), "medium_interval_medium_points"),
    std::make_tuple(std::make_tuple(-1.5, 1.7, 400000), "large_interval_many_points"),
    std::make_tuple(std::make_tuple(-1.0, 1.0, 50000), "simmerty_interval"),
    std::make_tuple(std::make_tuple(0.0, 1.0, 20000), "small_range_high_precision"),
};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<KrasnopevtsevaV_MCIntegrationMPI, InType>(
                                               kTestParam, PPC_SETTINGS_krasnopevtseva_v_monte_carlo_integration),
                                           ppc::util::AddFuncTask<KrasnopevtsevaV_MCIntegrationSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_krasnopevtseva_v_monte_carlo_integration));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    KrasnopevtsevaV_MCIntegrationFuncTests::PrintFuncTestName<KrasnopevtsevaV_MCIntegrationFuncTests>;

INSTANTIATE_TEST_SUITE_P(MCIntegrationTests, KrasnopevtsevaV_MCIntegrationFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace krasnopevtseva_v_monte_carlo_integration
