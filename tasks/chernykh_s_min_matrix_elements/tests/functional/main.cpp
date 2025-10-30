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

#include "chernykh_s_min_matrix_elements/common/include/common.hpp"
#include "chernykh_s_min_matrix_elements/mpi/include/ops_mpi.hpp"
#include "chernykh_s_min_matrix_elements/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace chernykh_s_min_matrix_elements {

class ChernykhSRunFuncTestsMinMatrixElements : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return test_param;
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    std::string inFileName = params;
    std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_chernykh_s_min_matrix_elements, inFileName);
    std::ifstream inFile(abs_path, std::ios::in | std::ios::binary);

    if (!inFile.is_open()) {
      throw std::runtime_error("Failed to open file: " + abs_path);
    }

    size_t stroki = 0, stolbci = 0;
    inFile.read(reinterpret_cast<char *>(&stroki), sizeof(size_t));
    inFile.read(reinterpret_cast<char *>(&stolbci), sizeof(size_t));

    std::get<2>(input_data_).resize(stroki * stolbci);
    inFile.read(reinterpret_cast<char *>(std::get<2>(input_data_).data()), sizeof(double) * stroki * stolbci);

    inFile.close();

    std::get<0>(input_data_) = stroki;
    std::get<1>(input_data_) = stolbci;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    const auto &mat = std::get<2>(input_data_);
    double expected_min = *std::min_element(mat.begin(), mat.end());
    return std::fabs(output_data - expected_min) < 1e-6;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(ChernykhSRunFuncTestsMinMatrixElements, FindMinInMatrix) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 1> kTestParam = {"data"};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<ChernykhSMinMatrixElementsMPI, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_min_matrix_elements),
                                           ppc::util::AddFuncTask<ChernykhSMinMatrixElementsSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_chernykh_s_min_matrix_elements));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName =
    ChernykhSRunFuncTestsMinMatrixElements::PrintFuncTestName<ChernykhSRunFuncTestsMinMatrixElements>;

INSTANTIATE_TEST_SUITE_P(MinMatrixTests, ChernykhSRunFuncTestsMinMatrixElements, kGtestValues, kPerfTestName);

}  // namespace
}  // namespace chernykh_s_min_matrix_elements
