#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <array>
#include <climits>
#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

#include "barkalova_m_min_val_matr/common/include/common.hpp"
#include "barkalova_m_min_val_matr/mpi/include/ops_mpi.hpp"
#include "barkalova_m_min_val_matr/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace barkalova_m_min_val_matr {

class BarkalovaMMinValMatrFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    const auto &matrix = std::get<0>(test_param);
    return "matrix_" + std::to_string(matrix.size()) + "x" + (matrix.empty() ? "0" : std::to_string(matrix[0].size()));
  }

 protected:
  void SetUp() override {
    test_params_ = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_matrix_ = std::get<0>(test_params_);
    expected_output_ = std::get<1>(test_params_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == expected_output_;
  }

  InType GetTestInputData() final {
    return input_matrix_;
  }

 private:
  TestType test_params_;
  InType input_matrix_;
  OutType expected_output_;
};

namespace {
// Тестовые данные
const std::vector<std::vector<int>> kMatrix1 = {{3, 7, 2, 8}, {1, 9, 6, 4}, {5, 2, 8, 3}, {7, 1, 4, 9}};
const std::vector<int> kExpected1 = {1, 1, 2, 3};

const std::vector<std::vector<int>> kMatrix2 = {{5, 8, 3}, {2, 6, 7}, {4, 3, 8}};
const std::vector<int> kExpected2 = {2, 3, 3};

const std::vector<std::vector<int>> kMatrix3 = {{10, 20}, {15, 5}, {8, 12}};
const std::vector<int> kExpected3 = {8, 5};

const std::vector<std::vector<int>> kEmptyMatrix = {};
const std::vector<int> kExpectedEmpty = {};

const std::vector<std::vector<int>> kSingleElement = {{5}};
const std::vector<int> kExpectedSingleElement = {5};

const std::vector<std::vector<int>> kSingleRow = {{1, 2, 3, 4, 5}};
const std::vector<int> kExpectedSingleRow = {1, 2, 3, 4, 5};

const std::vector<std::vector<int>> kSingleColumn = {{1}, {2}, {3}, {4}};
const std::vector<int> kExpectedSingleColumn = {1};

const std::vector<std::vector<int>> kWithIntMax = {{INT_MAX, 1, 5}, {2, INT_MAX, 3}};
const std::vector<int> kExpectedWithIntMax = {2, 1, 3};

const std::vector<std::vector<int>> kNegativeValues = {{-1, -5, -2, -6}, {-3, -2, -4, -7}};
const std::vector<int> kExpectedNegative = {-3, -5, -4, -7};

const std::vector<std::vector<int>> kAllSameValues = {{2, 2}, {2, 2}, {2, 2}, {2, 2}};
const std::vector<int> kExpectedAllSame = {2, 2};

TEST_P(BarkalovaMMinValMatrFuncTests, MinValuesInColumns) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 10> kTestParam = {
    std::make_tuple(kMatrix1, kExpected1),
    std::make_tuple(kMatrix2, kExpected2),
    std::make_tuple(kMatrix3, kExpected3),
    std::make_tuple(kEmptyMatrix, kExpectedEmpty),
    std::make_tuple(kSingleElement, kExpectedSingleElement),
    std::make_tuple(kSingleRow, kExpectedSingleRow),
    std::make_tuple(kSingleColumn, kExpectedSingleColumn),
    std::make_tuple(kWithIntMax, kExpectedWithIntMax),
    std::make_tuple(kNegativeValues, kExpectedNegative),
    std::make_tuple(kAllSameValues, kExpectedAllSame),
};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<BarkalovaMMinValMatrMPI, InType>(kTestParam, PPC_SETTINGS_barkalova_m_min_val_matr),
    ppc::util::AddFuncTask<BarkalovaMMinValMatrSEQ, InType>(kTestParam, PPC_SETTINGS_barkalova_m_min_val_matr));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = BarkalovaMMinValMatrFuncTests::PrintFuncTestName<BarkalovaMMinValMatrFuncTests>;

INSTANTIATE_TEST_SUITE_P(MinValuesTests, BarkalovaMMinValMatrFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace barkalova_m_min_val_matr
