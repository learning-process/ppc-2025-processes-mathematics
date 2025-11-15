#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"
#include "kapanova_s_min_of_matrix_elements/mpi/include/ops_mpi.hpp"
#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace kapanova_s_min_of_matrix_elements {

class KapanovaSMinOfMatrixElementsPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  void SetUp() override {
    input_matrix_ = LoadMatrixFromFile();
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
  
  std::vector<std::vector<int>> LoadMatrixFromFile() {
    std::string file_path = ppc::util::GetAbsoluteTaskPath(
        PPC_ID_kapanova_s_min_of_matrix_elements, 
        "data/matrix_3x3.txt"  
    );
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
      return CreateTestMatrix();
    }
    
    std::vector<std::vector<int>> matrix;
    std::string line;
    
    while (std::getline(file, line)) {
      std::vector<int> row;
      std::istringstream iss(line);
      int value;
      
      while (iss >> value) {
        row.push_back(value);
      }
      
      if (!row.empty()) {
        matrix.push_back(row);
      }
    }
    
    file.close();
    
    if (matrix.empty()) {
      return CreateTestMatrix();
    }
    
    return matrix;
  }
  
  std::vector<std::vector<int>> CreateTestMatrix() {
    const int size = 1000;  
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
    
    int counter = 1;
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        matrix[i][j] = counter++;
      }
    }
    
    matrix[size/2][size/2] = -100;
    
    return matrix;
  }
  
  int FindExpectedMin(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
      return 0;
    }
    
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

TEST_P(KapanovaSMinOfMatrixElementsPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KapanovaSMinOfMatrixElementsMPI, KapanovaSMinOfMatrixElementsSEQ>(PPC_SETTINGS_kapanova_s_min_of_matrix_elements);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = KapanovaSMinOfMatrixElementsPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KapanovaSMinOfMatrixElementsPerfTests, kGtestValues, kPerfTestName);

}  // namespace kapanova_s_min_of_matrix_elements