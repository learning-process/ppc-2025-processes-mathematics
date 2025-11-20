#include <gtest/gtest.h>

#include <fstream>
#include <ios>

#include "boltenkov_s_max_in_matrix/common/include/common.hpp"
#include "boltenkov_s_max_in_matrix/mpi/include/ops_mpi.hpp"
#include "boltenkov_s_max_in_matrix/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace boltenkov_s_max_in_matrix {

class ExampleRunPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_{};

  void SetUp() override {
    TestType params = "matrix2";
    std::string file_name = params + ".bin";
    std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_boltenkov_s_max_in_matrix, file_name);
    std::ifstream file_stream(abs_path, std::ios::binary);
    if (!file_stream.is_open()) {
      throw std::runtime_error("Error opening file!\n");
    }
    int m = -1, n = -1;
    file_stream.read(reinterpret_cast<char *>(&m), sizeof(int));
    file_stream.read(reinterpret_cast<char *>(&n), sizeof(int));
    if (m <= 0 || n <= 0) {
      throw std::runtime_error("invalid input data!\n");
    }
    std::get<0>(input_data_) = n;
    std::vector<double> &v = std::get<1>(input_data_);
    v.resize(m * n);
    file_stream.read(reinterpret_cast<char *>(v.data()), static_cast<std::streamsize>(sizeof(double) * m * n));
    file_stream.close();
  }

  bool CheckTestOutputData(OutType &output_data) final {
    int n = std::get<1>(input_data_).size();
    std::vector<double> &v = std::get<1>(input_data_);
    for (int i = 0; i < n; ++i) {
      if (v[i] > output_data) {
        return false;
      }
    }
    return true;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ExampleRunPerfTestProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, BoltenkovSMaxInMatrixkMPI, BoltenkovSMaxInMatrixkSEQ>(
    PPC_ID_boltenkov_s_max_in_matrix);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ExampleRunPerfTestProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ExampleRunPerfTestProcesses, kGtestValues, kPerfTestName);

}  // namespace boltenkov_s_max_in_matrix
