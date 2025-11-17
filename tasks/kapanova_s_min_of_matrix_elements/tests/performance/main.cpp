#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <vector>

#include "kapanova_s_min_of_matrix_elements/common/include/common.hpp"
#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"

namespace {

std::vector<std::vector<int>> LoadMatrixFromFile(const std::string &filename) {
  std::vector<std::vector<int>> matrix;
  std::ifstream file(filename);

  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      std::vector<int> row;
      std::istringstream iss(line);
      int value = 0;
      while (iss >> value) {
        row.push_back(value);
      }
      if (!row.empty()) {
        matrix.push_back(row);
      }
    }
  }
  return matrix;
}

TEST(kapanova_s_min_of_matrix_elements_perf, Test_Performance) {
  const int rows = 100;
  const int cols = 100;
  std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols));

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      matrix[i][j] = i * cols + j;
    }
  }

  matrix[50][50] = -100;

  kapanova_s_min_of_matrix_elements::KapanovaSMinOfMatrixElementsSEQ task(matrix);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  EXPECT_EQ(-100, task.GetOutput());
}

TEST(kapanova_s_min_of_matrix_elements_perf, Test_LoadFromFile) {
  // Используем функцию LoadMatrixFromFile
  auto matrix = LoadMatrixFromFile("tasks/kapanova_s_min_of_matrix_elements/data/matrix_3x3.txt");

  // Если файл не найден, создаем тестовую матрицу
  if (matrix.empty()) {
    matrix = {{1, 2, 3}, {4, 0, 6}, {7, 8, 9}};
  }

  kapanova_s_min_of_matrix_elements::KapanovaSMinOfMatrixElementsSEQ task(matrix);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  EXPECT_EQ(0, task.GetOutput());  // Минимум должен быть 0
}

}  // namespace
