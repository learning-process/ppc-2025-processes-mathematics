#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "lopatin_a_scalar_mult/common/include/common.hpp"
#include "lopatin_a_scalar_mult/mpi/include/ops_mpi.hpp"
#include "lopatin_a_scalar_mult/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace lopatin_a_scalar_mult {

class LopatinAScalarMultFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return test_param;
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    std::string filename = params + ".txt";
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

 private:
  InType input_data_ = InType();
};

namespace {

TEST_P(LopatinAScalarMultFuncTests, ScalarMult) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 1> kTestParam = {std::string("test_vectors_func")};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<LopatinAScalarMultMPI, InType>(kTestParam, PPC_SETTINGS_lopatin_a_scalar_mult),
    ppc::util::AddFuncTask<LopatinAScalarMultSEQ, InType>(kTestParam, PPC_SETTINGS_lopatin_a_scalar_mult));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = LopatinAScalarMultFuncTests::PrintFuncTestName<LopatinAScalarMultFuncTests>;

INSTANTIATE_TEST_SUITE_P(ScalarMultFuncTests, LopatinAScalarMultFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace lopatin_a_scalar_mult
