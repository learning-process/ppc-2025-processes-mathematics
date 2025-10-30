#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "moskaev_v_max_value_elem_matrix/common/include/common.hpp"
#include "moskaev_v_max_value_elem_matrix/mpi/include/ops_mpi.hpp"
#include "moskaev_v_max_value_elem_matrix/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace moskaev_v_max_value_elem_matrix {

// Функция для генерации тестовой матрицы для perf тестов
InType GeneratePerfTestMatrix(int size) {
  InType matrix(size, std::vector<int>(size));
  std::mt19937 gen(42);
  std::uniform_int_distribution<int> dist(1, 5000);

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      matrix[i][j] = dist(gen);
    }
  }

  // Устанавливаем известный максимальный элемент
  matrix[size / 2][size / 2] = 99999;

  return matrix;
}

class MoskaevVMaxValueElemMatrixPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kMatrixSize_ = 5000;  // Увеличим размер матрицы
  InType input_data_{};

  void SetUp() override {
    // Генерируем матрицу для perf тестов
    input_data_ = GeneratePerfTestMatrix(kMatrixSize_);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data > 0;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(MoskaevVMaxValueElemMatrixPerfTests, test_pipeline_run) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, MoskaevVMaxValueElemMatrixMPI, MoskaevVMaxValueElemMatrixSEQ>(
        PPC_SETTINGS_moskaev_v_max_value_elem_matrix);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = MoskaevVMaxValueElemMatrixPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(PerfTests, MoskaevVMaxValueElemMatrixPerfTests, kGtestValues, kPerfTestName);

// Тесты на производительность (ровно 2 теста на технологию)
TEST(moskaev_v_max_value_elem_matrix_mpi, test_pipeline_run) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  auto matrix = GeneratePerfTestMatrix(5000);  // Генерируем в тесте
  MoskaevVMaxValueElemMatrixMPI task(matrix);  // Передаем в реализацию

  // Pipeline run: отдельно измеряем время выполнения Run()
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  auto start_time = std::chrono::high_resolution_clock::now();
  EXPECT_TRUE(task.Run());
  auto end_time = std::chrono::high_resolution_clock::now();

  EXPECT_TRUE(task.PostProcessing());
  EXPECT_GT(task.GetOutput(), 0);

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  // Выводим время только из процесса 0
  if (rank == 0) {
    std::cout << "MPI Pipeline time: " << duration.count() << "ms" << std::endl;
  }
}

TEST(moskaev_v_max_value_elem_matrix_mpi, test_task_run) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  auto matrix = GeneratePerfTestMatrix(5000);  // Генерируем в тесте
  MoskaevVMaxValueElemMatrixMPI task(matrix);  // Передаем в реализацию

  // Task run: измеряем полное время выполнения
  auto start_time = std::chrono::high_resolution_clock::now();
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  auto end_time = std::chrono::high_resolution_clock::now();

  EXPECT_GT(task.GetOutput(), 0);

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  // Выводим время только из процесса 0
  if (rank == 0) {
    std::cout << "MPI Task time: " << duration.count() << "ms" << std::endl;
  }
}

TEST(moskaev_v_max_value_elem_matrix_seq, test_pipeline_run) {
  auto matrix = GeneratePerfTestMatrix(5000);  // Генерируем в тесте
  MoskaevVMaxValueElemMatrixSEQ task(matrix);  // Передаем в реализацию

  // Pipeline run: отдельно измеряем время выполнения Run()
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  auto start_time = std::chrono::high_resolution_clock::now();
  EXPECT_TRUE(task.Run());
  auto end_time = std::chrono::high_resolution_clock::now();

  EXPECT_TRUE(task.PostProcessing());
  EXPECT_GT(task.GetOutput(), 0);

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "SEQ Pipeline time: " << duration.count() << "ms" << std::endl;
}

TEST(moskaev_v_max_value_elem_matrix_seq, test_task_run) {
  auto matrix = GeneratePerfTestMatrix(5000);  // Генерируем в тесте
  MoskaevVMaxValueElemMatrixSEQ task(matrix);  // Передаем в реализацию

  // Task run: измеряем полное время выполнения
  auto start_time = std::chrono::high_resolution_clock::now();
  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());
  auto end_time = std::chrono::high_resolution_clock::now();

  EXPECT_GT(task.GetOutput(), 0);

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
  std::cout << "SEQ Task time: " << duration.count() << "ms" << std::endl;
}

}  // namespace moskaev_v_max_value_elem_matrix
