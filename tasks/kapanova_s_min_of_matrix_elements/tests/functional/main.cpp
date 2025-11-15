#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <random>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"
#include "kapanova_s_min_of_matrix_elements/mpi/include/ops_mpi.hpp"
#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace kapanova_s_min_of_matrix_elements {

class KapanovaSMinOfMatrixElementsFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int matrix_size = std::get<0>(params);
    
    input_matrix_ = GenerateTestMatrix(matrix_size);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    int expected_min = FindExpectedMin(input_matrix_);
    return (expected_min == output_data);
  }

  InType GetTestInputData() final {
    return input_matrix_;
  }

 private:
  InType input_matrix_;
  
  std::vector<std::vector<int>> GenerateTestMatrix(int size) {
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100);
    
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        matrix[i][j] = dist(gen);
      }
    }
    
    int min_row = size / 2;
    int min_col = size / 2;
    matrix[min_row][min_col] = -10;  
    
    return matrix;
  }
  
  int FindExpectedMin(const std::vector<std::vector<int>>& matrix) {
    int min_val = matrix[0][0];
    for (const auto& row : matrix) {
      for (int val : row) {
        if (val < min_val) {
          min_val = val;
        }
      }
    }
    return min_val;
  }
};

namespace {

TEST_P(KapanovaSMinOfMatrixElementsFuncTests, FindMinInMatrix) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {
  std::make_tuple(3, "3x3_matrix"),
  std::make_tuple(5, "5x5_matrix"), 
  std::make_tuple(10, "10x10_matrix")
};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<KapanovaSMinOfMatrixElementsMPI, InType>(kTestParam, PPC_SETTINGS_kapanova_s_min_of_matrix_elements),
                   ppc::util::AddFuncTask<KapanovaSMinOfMatrixElementsSEQ, InType>(kTestParam, PPC_SETTINGS_kapanova_s_min_of_matrix_elements));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = KapanovaSMinOfMatrixElementsFuncTests::PrintFuncTestName<KapanovaSMinOfMatrixElementsFuncTests>;

INSTANTIATE_TEST_SUITE_P(MatrixMinTests, KapanovaSMinOfMatrixElementsFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace kapanova_s_min_of_matrix_elements
