#include <gtest/gtest.h>
#include <mpi.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "../../modules/util/include/func_test_util.hpp"
#include "../../modules/util/include/util.hpp"
#include "moskaev_v_max_value_elem_matrix/common/include/common.hpp"
#include "moskaev_v_max_value_elem_matrix/mpi/include/ops_mpi.hpp"
#include "moskaev_v_max_value_elem_matrix/seq/include/ops_seq.hpp"

namespace moskaev_v_max_value_elem_matrix {

static InType GenerateTestMatrix(int size) {
  if (size == 0) {
    return std::vector<std::vector<int>>{};
  }
  InType matrix(size, std::vector<int>(size));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 50000);

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      matrix[i][j] = dist(gen);
    }
  }

  matrix[size / 2][size / 2] = 99999;

  return matrix;
}

class MoskaevVMaxValueElemMatrixFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return "Size_" + std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int size = std::get<0>(params);

    input_data_ = GenerateTestMatrix(size);
    reference_max_ = CalculateReferenceMax(input_data_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    bool result_correct = (output_data == reference_max_);
    if (!result_correct) {
      std::cout << "Expected " << reference_max_ << ", got " << output_data << "\n";
    }
    return result_correct;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType reference_max_ = 0;

  static OutType CalculateReferenceMax(const InType &matrix) {
    if (matrix.empty()) {
      return 0;
    }

    OutType max_val = matrix[0][0];
    for (const auto &row : matrix) {
      for (int val : row) {
        max_val = std::max(val, max_val);
      }
    }
    return max_val;
  }
};

namespace {

TEST_P(MoskaevVMaxValueElemMatrixFuncTests, TestFindMaxElement) {
  ExecuteTest(GetParam());
}

// Тестовые случаи
const std::array<TestType, 5> kTestParam = {std::make_tuple(7, "7x7"), std::make_tuple(20, "20x20"),
                                            std::make_tuple(50, "50x50"), std::make_tuple(100, "100x100"),
                                            std::make_tuple(11, "11x11")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<MoskaevVMaxValueElemMatrixMPI, InType>(
                                               kTestParam, PPC_SETTINGS_moskaev_v_max_value_elem_matrix),
                                           ppc::util::AddFuncTask<MoskaevVMaxValueElemMatrixSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_moskaev_v_max_value_elem_matrix));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kFuncTestName = MoskaevVMaxValueElemMatrixFuncTests::PrintFuncTestName<MoskaevVMaxValueElemMatrixFuncTests>;

INSTANTIATE_TEST_SUITE_P(FuncTests, MoskaevVMaxValueElemMatrixFuncTests, kGtestValues, kFuncTestName);

// Индивидуальные тест кейсы

TEST(MoskaevVMaxValueElemMatrixMpi, testZeroMatrix) {
  int initialized = 0;
  MPI_Initialized(&initialized);
  if (initialized == 0) {
    MPI_Init(nullptr, nullptr);
  }
  auto matrix = GenerateTestMatrix(0);
  MoskaevVMaxValueElemMatrixMPI task(matrix);

  EXPECT_FALSE(task.Validation());

  EXPECT_FALSE(task.PreProcessing());
  EXPECT_FALSE(task.Run());
  EXPECT_FALSE(task.PostProcessing());
}

TEST(MoskaevVMaxValueElemMatrixSeq, testZeroMatrix) {
  auto matrix = GenerateTestMatrix(0);
  MoskaevVMaxValueElemMatrixSEQ task(matrix);

  EXPECT_FALSE(task.Validation());

  EXPECT_FALSE(task.PreProcessing());
  EXPECT_FALSE(task.Run());
  EXPECT_FALSE(task.PostProcessing());
}

TEST(MoskaevVMaxValueElemMatrixMpi, testSmallMatrix) {
  int initialized = 0;
  MPI_Initialized(&initialized);
  if (initialized == 0) {
    MPI_Init(nullptr, nullptr);
  }
  auto matrix = GenerateTestMatrix(30);
  MoskaevVMaxValueElemMatrixMPI task(matrix);
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_GT(task.GetOutput(), 0);
}

TEST(MoskaevVMaxValueElemMatrixSeq, testSmallMatrix) {
  auto matrix = GenerateTestMatrix(30);
  MoskaevVMaxValueElemMatrixSEQ task(matrix);
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_GT(task.GetOutput(), 0);
}

TEST(MoskaevVMaxValueElemMatrixSeq, testNegativeNumbers) {
  InType matrix = {{-5, -2, -10}, {-1, -8, -3}, {-7, -4, -6}};
  MoskaevVMaxValueElemMatrixSEQ task(matrix);
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), -1);
}

TEST(MoskaevVMaxValueElemMatrixSeq, testSingleElement) {
  InType matrix = {{42}};
  MoskaevVMaxValueElemMatrixSEQ task(matrix);
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), 42);
}

TEST(MoskaevVMaxValueElemMatrixMpi, testSingleElement) {
  int initialized = 0;
  MPI_Initialized(&initialized);
  if (!initialized) {
    MPI_Init(nullptr, nullptr);
  }

  InType matrix = {{42}};
  MoskaevVMaxValueElemMatrixMPI task(matrix);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), 42);
}

TEST(MoskaevVMaxValueElemMatrixSeq, testNonSquareMatrix) {
  InType matrix = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
  MoskaevVMaxValueElemMatrixSEQ task(matrix);
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), 12);
}

TEST(MoskaevVMaxValueElemMatrixSeq, testMultipleMaxElements) {
  InType matrix = {{1, 2, 3}, {4, 5, 5}, {5, 1, 2}};
  MoskaevVMaxValueElemMatrixSEQ task(matrix);
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), 5);
}

TEST(MoskaevVMaxValueElemMatrixMpi, testUnevenRowDistribution) {
  int initialized = 0;
  MPI_Initialized(&initialized);
  if (!initialized) {
    MPI_Init(nullptr, nullptr);
  }

  InType matrix(7, std::vector<int>(3, 1));
  matrix[3][1] = 999;

  MoskaevVMaxValueElemMatrixMPI task(matrix);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  EXPECT_EQ(task.GetOutput(), 999);
}

TEST(MoskaevVMaxValueElemMatrixSeq, testExtremeValues) {
  InType matrix = {{INT_MIN, 0, INT_MAX}, {-100, 100, 0}, {INT_MAX, INT_MIN, 500}};
  MoskaevVMaxValueElemMatrixSEQ task(matrix);
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_EQ(task.GetOutput(), INT_MAX);
}

TEST(MoskaevVMaxValueElemMatrixSeq, testIndividualMethodFailures) {
  InType empty_matrix{};
  MoskaevVMaxValueElemMatrixSEQ task(empty_matrix);

  EXPECT_FALSE(task.Validation());
  EXPECT_FALSE(task.PreProcessing());
  EXPECT_FALSE(task.Run());
  EXPECT_FALSE(task.PostProcessing());
}

TEST(MoskaevVMaxValueElemMatrixSeq, testStateAfterFailedOperations) {
  InType empty_matrix{};
  MoskaevVMaxValueElemMatrixSEQ task(empty_matrix);

  EXPECT_FALSE(task.Validation());
  EXPECT_FALSE(task.PreProcessing());
  EXPECT_FALSE(task.Run());
  EXPECT_FALSE(task.PostProcessing());

  EXPECT_EQ(task.GetOutput(), 0);
}

}  // namespace

}  // namespace moskaev_v_max_value_elem_matrix
