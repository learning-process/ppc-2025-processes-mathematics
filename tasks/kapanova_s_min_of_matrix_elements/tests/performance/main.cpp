#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "kapanova_s_min_of_matrix_elements/mpi/include/ops_mpi.hpp"
#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"

namespace {

using kapanova_s_min_of_matrix_elements::InType;
using kapanova_s_min_of_matrix_elements::KapanovaSMinOfMatrixElementsSEQ;
using kapanova_s_min_of_matrix_elements::OutType;

struct PerfTestData {
  std::string test_name;
  std::string description;

  PerfTestData(std::string name, std::string desc) : test_name(std::move(name)), description(std::move(desc)) {}
};

class KapanovaSMinOfMatrixElementsPerfTests : public testing::TestWithParam<PerfTestData> {
 private:
  std::vector<std::vector<int>> input_matrix_;

 public:
  void SetUp() override {
    input_matrix_ = CreateTestMatrix();
  }

  static bool CheckTestOutputData(OutType output_data) {
    return output_data >= 0;
  }

  static std::vector<std::vector<int>> LoadMatrixFromFile() {
    std::vector<std::vector<int>> matrix;
    std::string abs_path = "tasks/kapanova_s_min_of_matrix_elements/data/matrix_3x3.txt";
    std::ifstream file(abs_path);

    if (file.is_open()) {
      std::string line;
      while (std::getline(file, line)) {
        std::vector<int> row;
        std::istringstream iss(line);
        int value = 0;
        while (iss >> value) {
          row.push_back(value);
        }
        if (!row.empty()) {
          matrix.push_back(row);
        }
      }
    }
    return matrix;
  }

  static std::vector<std::vector<int>> CreateTestMatrix() {
    const int rows = 100;
    const int cols = 100;
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols));

    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        matrix[i][j] = (i * cols) + j;
      }
    }
    return matrix;
  }

  static int FindExpectedMin(const std::vector<std::vector<int>> &matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return 0;
    }

    int min_val = matrix[0][0];
    for (const auto &row : matrix) {
      for (const int val : row) {
        min_val = std::min(val, min_val);
      }
    }
    return min_val;
  }

  std::vector<std::vector<int>> GetTestInputData() {
    return input_matrix_;
  }
};

TEST_P(KapanovaSMinOfMatrixElementsPerfTests, RunPerfModes) {
  const auto test_input_data = GetTestInputData();

  EXPECT_FALSE(test_input_data.empty());
  EXPECT_FALSE(test_input_data[0].empty());

  auto task = std::make_shared<KapanovaSMinOfMatrixElementsSEQ>(test_input_data);

  task->Validation();
  task->PreProcessing();
  task->Run();
  task->PostProcessing();

  OutType output_data = task->GetOutput();
  bool result_correct = CheckTestOutputData(output_data);
  EXPECT_TRUE(result_correct);

  int expected_min = FindExpectedMin(test_input_data);
  EXPECT_EQ(expected_min, output_data);
}

TEST_P(KapanovaSMinOfMatrixElementsPerfTests, TestDescription) {
  const auto &test_param = GetParam();
  std::cout << "Running test: " << test_param.test_name << " - " << test_param.description << '\n';

  const auto test_input_data = GetTestInputData();
  EXPECT_FALSE(test_input_data.empty());

  auto task = std::make_shared<KapanovaSMinOfMatrixElementsSEQ>(test_input_data);
  task->Validation();
  task->PreProcessing();
  task->Run();
  task->PostProcessing();
}

const auto kGtestValues = testing::Values(PerfTestData("small_matrix", "Small matrix test"),
                                          PerfTestData("medium_matrix", "Medium matrix test"),
                                          PerfTestData("large_matrix", "Large matrix test"));

INSTANTIATE_TEST_SUITE_P(RunModeTests, KapanovaSMinOfMatrixElementsPerfTests, kGtestValues,
                         [](const testing::TestParamInfo<PerfTestData> &info) { return info.param.test_name; });

}  // namespace
