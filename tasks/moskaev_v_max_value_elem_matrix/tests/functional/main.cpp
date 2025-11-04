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

// Функция для генерации тестовой матрицы (ТОЛЬКО в тестах)
static InType GenerateTestMatrix(int size) {
  InType matrix(size, std::vector<int>(size));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(1, 100000);

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      matrix[i][j] = dist(gen);
    }
  }

  // Устанавливаем известный максимальный элемент
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

    // Генерируем тестовую матрицу (ТОЛЬКО здесь)

    input_data_ = GenerateTestMatrix(size);
    reference_max_ = CalculateReferenceMax(input_data_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
      bool result_correct = (output_data == reference_max_);
      if (!result_correct) {
        std::cout << "Rank 0: Expected " << reference_max_ << ", got " << output_data << "\n";
      }
      return result_correct;
    }
    return output_data > 0;
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
const std::array<TestType, 4> kTestParam = {std::make_tuple(10, "10x10"), std::make_tuple(20, "20x20"),
                                            std::make_tuple(50, "50x50"), std::make_tuple(100, "100x100")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<MoskaevVMaxValueElemMatrixMPI, InType>(
                                               kTestParam, PPC_SETTINGS_moskaev_v_max_value_elem_matrix),
                                           ppc::util::AddFuncTask<MoskaevVMaxValueElemMatrixSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_moskaev_v_max_value_elem_matrix));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kFuncTestName = MoskaevVMaxValueElemMatrixFuncTests::PrintFuncTestName<MoskaevVMaxValueElemMatrixFuncTests>;

INSTANTIATE_TEST_SUITE_P(FuncTests, MoskaevVMaxValueElemMatrixFuncTests, kGtestValues, kFuncTestName);

// Индивидуальные тест кейсы
TEST(MoskaevVMaxValueElemMatrixMpi, testSmallMatrix) {
  auto matrix = GenerateTestMatrix(10);        // Генерируем в тесте
  MoskaevVMaxValueElemMatrixMPI task(matrix);  // Передаем в реализацию
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_GT(task.GetOutput(), 0);
}

TEST(MoskaevVMaxValueElemMatrixSeq, testSmallMatrix) {
  auto matrix = GenerateTestMatrix(10);        // Генерируем в тесте
  MoskaevVMaxValueElemMatrixSEQ task(matrix);  // Передаем в реализацию
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  EXPECT_GT(task.GetOutput(), 0);
}

}  // namespace

}  // namespace moskaev_v_max_value_elem_matrix
