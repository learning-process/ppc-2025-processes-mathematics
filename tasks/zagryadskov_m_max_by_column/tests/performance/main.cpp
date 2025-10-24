#include <gtest/gtest.h>

#include "util/include/perf_test_util.hpp"
#include "zagryadskov_m_max_by_column/common/include/common.hpp"
#include "zagryadskov_m_max_by_column/mpi/include/max_by_column.hpp"
#include "zagryadskov_m_max_by_column/seq/include/max_by_column.hpp"

namespace zagryadskov_m_max_by_column {

class ZagryadskovMRunPerfTestMaxByColumn : public ppc::util::BaseRunPerfTests<InType, OutType> {
  InType input_data_{};

  void SetUp() override {
    std::string inFileName = "mat1.bin";
    std::string abs_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_zagryadskov_m_max_by_column,
                                                          inFileName);  // std::string abs_path = "../../data/mat1.bin";
    std::ifstream inFileStream(abs_path, std::ios::in | std::ios::binary);
    if (!inFileStream.is_open()) {
      throw std::runtime_error("Error opening file!\n");
    }
    size_t m;
    size_t n;
    inFileStream.read(reinterpret_cast<char *>(&m), sizeof(size_t));
    inFileStream.read(reinterpret_cast<char *>(&n), sizeof(size_t));
    std::get<0>(input_data_) = n;
    auto &mat = std::get<1>(input_data_);
    mat.resize(m * n);
    using T = std::decay_t<decltype(*mat.begin())>;

    inFileStream.read(reinterpret_cast<char *>(mat.data()), sizeof(T) * m * n);

    inFileStream.close();
  }

  bool CheckTestOutputData(OutType &output_data) final {
    bool res = true;
    size_t n = std::get<0>(input_data_);
    size_t m = std::get<1>(input_data_).size() / n;
    auto &mat = std::get<1>(input_data_);
    if (output_data.size() != n) {
      res = false;
      return res;
    }

    for (size_t j = 0; j < n; ++j) {
      for (size_t i = 0; i < m; ++i) {
        if (output_data[j] < mat[j * m + i]) {
          res = false;
        }
      }
    }

    return res;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ZagryadskovMRunPerfTestMaxByColumn, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, ZagryadskovMMaxByColumnMPI, ZagryadskovMMaxByColumnSEQ>(
    PPC_SETTINGS_zagryadskov_m_max_by_column);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ZagryadskovMRunPerfTestMaxByColumn::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ZagryadskovMRunPerfTestMaxByColumn, kGtestValues, kPerfTestName);

}  // namespace zagryadskov_m_max_by_column
