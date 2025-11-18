#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "chernykh_s_min_matrix_elements/common/include/common.hpp"
#include "chernykh_s_min_matrix_elements/mpi/include/ops_mpi.hpp"
#include "chernykh_s_min_matrix_elements/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace chernykh_s_min_matrix_elements {

class ChernykhSRunFuncTestsMinMatrixElements : public ppc::util::BaseRunPerfTests<InType, OutType> {
 private:
  InType input_data_;

  void SetUp() override {
    TestType params = "create_data_2048x2048";
    std::string inFileName = params + ".txt";
    std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_chernykh_s_min_matrix_elements, inFileName);

    std::ifstream inFile(abs_path, std::ios::in);
    if (!inFile.is_open()) {
      throw std::runtime_error("Failed to open file: " + abs_path);
    }

    input_data_.clear();
    std::string line;

    while (std::getline(inFile, line)) {
      if (line.empty()) {
        continue;
      }

      std::istringstream iss(line);
      std::vector<double> row;
      double value;

      while (iss >> value) {
        row.push_back(value);
      }

      if (!row.empty()) {
        input_data_.push_back(row);
      }
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {  // такое же как в func тесте
    const auto &mat = input_data_;
    double expected_min = std::numeric_limits<double>::max();
    for (const auto &row : mat) {
      for (double v : row) {
        expected_min = std::min(expected_min, v);
      }
    }
    return std::fabs(output_data - expected_min) < 1e-6;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ChernykhSRunFuncTestsMinMatrixElements, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ChernykhSMinMatrixElementsMPI, ChernykhSMinMatrixElementsSEQ>(
        PPC_SETTINGS_chernykh_s_min_matrix_elements);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ChernykhSRunFuncTestsMinMatrixElements::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ChernykhSRunFuncTestsMinMatrixElements, kGtestValues, kPerfTestName);

};  // namespace chernykh_s_min_matrix_elements
