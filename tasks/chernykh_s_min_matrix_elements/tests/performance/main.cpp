#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "chernykh_s_min_matrix_elements/common/include/common.hpp"
#include "chernykh_s_min_matrix_elements/mpi/include/ops_mpi.hpp"
#include "chernykh_s_min_matrix_elements/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace chernykh_s_min_matrix_elements {

class ChernykhSRunFuncTestsMinMatrixElements : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  InType input_data_{};

  void SetUp() override {
    // Файл с матрицей
    const std::string file_name = "data";
    std::string abs_path = "../../tasks/chernykh_s_min_matrix_elements/data/data";

    std::ifstream inFile(abs_path, std::ios::binary);
    if (!inFile.is_open()) {
      throw std::runtime_error("Failed to open file: " + abs_path);
    }

    size_t stroki = 0, stolbci = 0;
    inFile.read(reinterpret_cast<char *>(&stroki), sizeof(size_t));
    inFile.read(reinterpret_cast<char *>(&stolbci), sizeof(size_t));

    auto &matrix = std::get<2>(input_data_);
    matrix.resize(stroki * stolbci);
    inFile.read(reinterpret_cast<char *>(matrix.data()), sizeof(double) * stroki * stolbci);
    inFile.close();

    std::get<0>(input_data_) = stroki;
    std::get<1>(input_data_) = stolbci;
  }

  // bool CheckTestOutputData(OutType &output_data) final {
  //   const auto &matrix = std::get<2>(input_data_);
  //   if (matrix.empty()) {
  //     return output_data == std::numeric_limits<double>::max();
  //   }

  //   double expected = *std::min_element(matrix.begin(), matrix.end());

  //   return std::fabs(output_data - expected) < 1e-6;
  // }

  bool CheckTestOutputData(OutType &output_data) final {
    const auto &mat = std::get<2>(input_data_);
    double expected_min = *std::min_element(mat.begin(), mat.end());
    // std::cout<<"Perfomance" << std::endl;
    // std::cout<<"expected_min = " << expected_min <<std::endl;
    // std::cout<<"output_data - expected_min = " << output_data - expected_min <<std::endl;
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

}  // namespace chernykh_s_min_matrix_elements
