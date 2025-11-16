#include <gtest/gtest.h>

#include <vector>

#include "kapanova_s_min_of_matrix_elements/seq/include/ops_seq.hpp"

namespace {

using kapanova_s_min_of_matrix_elements::InType;
using kapanova_s_min_of_matrix_elements::KapanovaSMinOfMatrixElementsSEQ;

class MatrixMinTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  static int FindMinSequential(const std::vector<std::vector<int>> &matrix) {
    int min_val = matrix[0][0];
    for (const auto &row : matrix) {
      for (const int value : row) {
        min_val = std::min(value, min_val);
      }
    }
    return min_val;
  }
};

TEST_F(MatrixMinTest, BasicTest) {
  std::vector<std::vector<int>> matrix = {{3, 1, 4}, {2, 5, 9}, {6, 7, 8}};
  int expected_min = 1;

  KapanovaSMinOfMatrixElementsSEQ task_seq(matrix);
  bool success = task_seq.Run();
  EXPECT_TRUE(success);
  EXPECT_EQ(expected_min, task_seq.GetOutput());

  int actual_min = FindMinSequential(matrix);
  EXPECT_EQ(expected_min, actual_min);
}

TEST_F(MatrixMinTest, NegativeNumbers) {
  std::vector<std::vector<int>> matrix = {{3, -1, 4}, {2, 5, 9}, {6, 7, 8}};
  int expected_min = -1;

  KapanovaSMinOfMatrixElementsSEQ task_seq(matrix);
  bool success = task_seq.Run();
  EXPECT_TRUE(success);
  EXPECT_EQ(expected_min, task_seq.GetOutput());

  int actual_min = FindMinSequential(matrix);
  EXPECT_EQ(expected_min, actual_min);
}

TEST_F(MatrixMinTest, SingleElement) {
  std::vector<std::vector<int>> matrix = {{5}};
  int expected_min = 5;

  KapanovaSMinOfMatrixElementsSEQ task_seq(matrix);
  bool success = task_seq.Run();
  EXPECT_TRUE(success);
  EXPECT_EQ(expected_min, task_seq.GetOutput());
}

TEST_F(MatrixMinTest, EmptyMatrix) {
  std::vector<std::vector<int>> matrix = {};

  KapanovaSMinOfMatrixElementsSEQ task_seq(matrix);
  bool success = task_seq.Run();
  EXPECT_FALSE(success);
}

TEST_F(MatrixMinTest, DifferentRowSizes) {
  std::vector<std::vector<int>> matrix = {{1, 2}, {3}};

  KapanovaSMinOfMatrixElementsSEQ task_seq(matrix);
  bool success = task_seq.Run();
  EXPECT_FALSE(success);
}

}  // namespace
