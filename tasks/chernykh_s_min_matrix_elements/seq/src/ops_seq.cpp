#include "chernykh_s_min_matrix_elements/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "chernykh_s_min_matrix_elements/common/include/common.hpp"
#include "util/include/util.hpp"

namespace chernykh_s_min_matrix_elements {

ChernykhSMinMatrixElementsSEQ::ChernykhSMinMatrixElementsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0.0;
}

bool ChernykhSMinMatrixElementsSEQ::ValidationImpl() {
  const size_t stroki = std::get<0>(GetInput());
  const size_t stolbci = std::get<1>(GetInput());
  const std::vector<double> &matrica = std::get<2>(GetInput());
  if (matrica.size() != stroki * stolbci) {
    return false;
  }
  if (stroki > 0 && stolbci > 0 && matrica.empty()) {
    return false;
  }
  return true;
}

bool ChernykhSMinMatrixElementsSEQ::PreProcessingImpl() {
  return true;
}

bool ChernykhSMinMatrixElementsSEQ::RunImpl() {
  const size_t &stroki = std::get<0>(GetInput());
  const size_t &stolbci = std::get<1>(GetInput());
  const std::vector<double> &matrica = std::get<2>(GetInput());
  double global_min = std::numeric_limits<double>::max();
  if (matrica.empty() || stroki == 0 || stolbci == 0) {
    GetOutput() = std::numeric_limits<double>::max();
    return true;
  }

  for (size_t i = 0; i < stroki * stolbci; i++) {
    if (matrica[i] < global_min) {
      global_min = matrica[i];
    }
  }
  GetOutput() = global_min;
  return true;
}

bool ChernykhSMinMatrixElementsSEQ::PostProcessingImpl() {
  return GetOutput() != std::numeric_limits<double>::max();
}

}  // namespace chernykh_s_min_matrix_elements
