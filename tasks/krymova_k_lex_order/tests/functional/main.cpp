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

#include "krymova_k_lex_order/common/include/common.hpp"
#include "krymova_k_lex_order/mpi/include/ops_mpi.hpp"
#include "krymova_k_lex_order/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace krymova_k_lex_order {

class KrymovaKLexOrderFuncTestsProcesses : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<0>(test_param) + "_vs_" + std::get<1>(test_param) + "_exp_equal";
  }

 protected:
  void SetUp() override {
    TestType test_params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = {std::get<0>(test_params), std::get<1>(test_params)};
    expected_result_ = std::get<2>(test_params);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return (expected_result_ == output_data);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_ ;
  OutType expected_result_;
};

namespace {

TEST_P(KrymovaKLexOrderFuncTestsProcesses, MatmulFromPic) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 8> kTestParam = {
    std::make_tuple("hello", "hello", 0),
    std::make_tuple("a", "a", 0),
    
    std::make_tuple("apple", "banana", -1),
    std::make_tuple("abc", "abd", -1),
    std::make_tuple("a", "b", -1),
      
    std::make_tuple("banana", "apple", 1),
    std::make_tuple("abd", "abc", 1),
};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<KrymovaKLexOrderMPI, InType>(kTestParam, PPC_SETTINGS_krymova_k_lex_order),
                   ppc::util::AddFuncTask<KrymovaKLexSEQ, InType>(kTestParam, PPC_SETTINGS_krymova_k_lex_order));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = KrymovaKLexOrderFuncTestsProcesses::PrintFuncTestName<KrymovaKLexOrderFuncTestsProcesses>;

INSTANTIATE_TEST_SUITE_P(StringComparisonTests, KrymovaKLexOrderFuncTestsProcesses, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace krymova_k_lex_order
