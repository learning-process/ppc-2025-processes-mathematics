#include <gtest/gtest.h>
#include <stb/stb_image.h>

// #include <algorithm>
#include <array>
#include <cstddef>
// #include <cstdint>
// #include <stdexcept>
#include <string>
#include <tuple>
// #include <utility>
#include <vector>

#include "barkalova_m_min_val_matr/common/include/common.hpp"
#include "barkalova_m_min_val_matr/mpi/include/ops_mpi.hpp"
#include "barkalova_m_min_val_matr/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

/*namespace barkalova_m_min_val_matr {

class BarkalovaMMinValMatrFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    int width = -1;
    int height = -1;
    int channels = -1;
    std::vector<uint8_t> img;
    // Read image
    {
      std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_barkalova_m_min_val_matr, "pic.jpg");
      auto *data = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);
      if (data == nullptr) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
      }
      img = std::vector<uint8_t>(data, data + (static_cast<ptrdiff_t>(width * height * channels)));
      stbi_image_free(data);
      if (std::cmp_not_equal(width, height)) {
        throw std::runtime_error("width != height: ");
      }
    }

    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = width - height + std::min(std::accumulate(img.begin(), img.end(), 0), channels);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return (input_data_ == output_data);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_ = 0;
};

namespace {

TEST_P(BarkalovaMMinValMatrFuncTests, MatmulFromPic) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {std::make_tuple(3, "3"), std::make_tuple(5, "5"), std::make_tuple(7, "7")};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<BarkalovaMMinValMatrMPI, InType>(kTestParam,
PPC_SETTINGS_barkalova_m_min_val_matr), ppc::util::AddFuncTask<BarkalovaMMinValMatrSEQ, InType>(kTestParam,
PPC_SETTINGS_barkalova_m_min_val_matr));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = BarkalovaMMinValMatrFuncTests::PrintFuncTestName<BarkalovaMMinValMatrFuncTests>;

INSTANTIATE_TEST_SUITE_P(PicMatrixTests, BarkalovaMMinValMatrFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace barkalova_m_min_val_matr*/

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
const std::vector<std::vector<int>> matrix1 = {{3, 7, 2, 8}, {1, 9, 6, 4}, {5, 2, 8, 3}, {7, 1, 4, 9}};
const std::vector<int> expected1 = {1, 1, 2, 3};

const std::vector<std::vector<int>> matrix2 = {{5, 8, 3}, {2, 6, 7}, {4, 3, 8}};
const std::vector<int> expected2 = {2, 3, 3};

const std::vector<std::vector<int>> matrix3 = {{10, 20}, {15, 5}, {8, 12}};
const std::vector<int> expected3 = {8, 5};

TEST_P(BarkalovaMMinValMatrFuncTests, MinValuesInColumns) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {std::make_tuple(matrix1, expected1), std::make_tuple(matrix2, expected2),
                                            std::make_tuple(matrix3, expected3)};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<BarkalovaMMinValMatrMPI, InType>(kTestParam, PPC_SETTINGS_barkalova_m_min_val_matr),
    ppc::util::AddFuncTask<BarkalovaMMinValMatrSEQ, InType>(kTestParam, PPC_SETTINGS_barkalova_m_min_val_matr));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = BarkalovaMMinValMatrFuncTests::PrintFuncTestName<BarkalovaMMinValMatrFuncTests>;

INSTANTIATE_TEST_SUITE_P(MinValuesTests, BarkalovaMMinValMatrFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace barkalova_m_min_val_matr
