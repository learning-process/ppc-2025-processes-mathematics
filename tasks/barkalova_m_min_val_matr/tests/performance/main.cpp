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


