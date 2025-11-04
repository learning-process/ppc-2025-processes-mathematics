#include <gtest/gtest.h>

#include "barkalova_m_min_val_matr/common/include/common.hpp"
#include "barkalova_m_min_val_matr/mpi/include/ops_mpi.hpp"
#include "barkalova_m_min_val_matr/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

/*namespace barkalova_m_min_val_matr {

class BarkalovaMMinValMatrPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 100;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(BarkalovaMMinValMatrPerfTest, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, BarkalovaMMinValMatrMPI, BarkalovaMMinValMatrSEQ>(PPC_SETTINGS_barkalova_m_min_val_matr);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = BarkalovaMMinValMatrPerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, BarkalovaMMinValMatrPerfTest, kGtestValues, kPerfTestName);

}  // namespace barkalova_m_min_val_matr*/



namespace barkalova_m_min_val_matr {

class BarkalovaMMinValMatrPerfTest : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_{};

  void SetUp() override {
    // Оставляем маленькую матрицу
    const size_t rows = 100;
    const size_t stolb = 100;
    
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(stolb));
    
    for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < stolb; ++j) {
        matrix[i][j] = static_cast<int>(i + j + 1);
      }
    }
    
    input_data_ = matrix;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    const auto& matrix = input_data_;
    
    if (output_data.empty()) {
      return false; 
    }
    
    if (output_data.size() != matrix[0].size()) {
      return false;
    }

    std::vector<int> correct_result(matrix[0].size(), INT_MAX);
    for (size_t j = 0; j < matrix[0].size(); ++j) {
      for (size_t i = 0; i < matrix.size(); ++i) {
        if (matrix[i][j] < correct_result[j]) {
          correct_result[j] = matrix[i][j];
        }
      }
    }
    
    return output_data == correct_result;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(BarkalovaMMinValMatrPerfTest, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, BarkalovaMMinValMatrMPI, BarkalovaMMinValMatrSEQ>(PPC_SETTINGS_barkalova_m_min_val_matr);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = BarkalovaMMinValMatrPerfTest::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, BarkalovaMMinValMatrPerfTest, kGtestValues, kPerfTestName);

}  // namespace barkalova_m_min_val_matr