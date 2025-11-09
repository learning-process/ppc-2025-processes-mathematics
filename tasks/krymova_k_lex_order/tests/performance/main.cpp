#include <gtest/gtest.h>

#include "krymova_k_lex_order/common/include/common.hpp"
#include "krymova_k_lex_order/mpi/include/ops_mpi.hpp"
#include "krymova_k_lex_order/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace krymova_k_lex_order {

class KrymovaKLexOrderPerfTestProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
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

TEST_P(KrymovaKLexOrderPerfTestProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KrymovaKLexOrderMPI, KrymovaKLexSEQ>(PPC_SETTINGS_krymova_k_lex_order);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = KrymovaKLexOrderPerfTestProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KrymovaKLexOrderPerfTestProcesses, kGtestValues, kPerfTestName);

}  // namespace krymova_k_lex_order
