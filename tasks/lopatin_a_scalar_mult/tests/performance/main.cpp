#include <gtest/gtest.h>

#include "lopatin_a_scalar_mult/common/include/common.hpp"
#include "lopatin_a_scalar_mult/mpi/include/ops_mpi.hpp"
#include "lopatin_a_scalar_mult/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace lopatin_a_scalar_mult {

class LopatinAScalarMultPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_{};

  void SetUp() override {
    std::string filename = "test_vectors_func.txt";
    std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_lopatin_a_scalar_mult, filename);
    std::ifstream infile(abs_path, std::ios::in);
    if (!infile.is_open()) {
      throw std::runtime_error("Failed to open file: " + filename);
    }

    int count = 0;
    std::string line;
    while (std::getline(infile, line)) {
      if (line.empty()) {
        continue;
      }

      std::istringstream iss(line);
      double value;

      if (!count) {
        while (iss >> value) {
          input_data_.first.push_back(value);
        }
        ++count;
      } else if (count) {
        if (count > 1) {
          throw std::runtime_error("Too much data in file: " + filename);
        }
        while (iss >> value) {
          input_data_.second.push_back(value);
        }
        ++count;
      }
    }

    infile.close();
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == 3;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(LopatinAScalarMultPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, LopatinAScalarMultMPI, LopatinAScalarMultSEQ>(
    PPC_SETTINGS_lopatin_a_scalar_mult);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = LopatinAScalarMultPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(ScalarMultPerfTests, LopatinAScalarMultPerfTests, kGtestValues, kPerfTestName);

}  // namespace lopatin_a_scalar_mult
