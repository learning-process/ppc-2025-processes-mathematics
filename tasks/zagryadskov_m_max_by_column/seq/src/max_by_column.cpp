#include "zagryadskov_m_max_by_column/seq/include/max_by_column.hpp"

#include <algorithm>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>

#include "util/include/util.hpp"
#include "zagryadskov_m_max_by_column/common/include/common.hpp"

namespace zagryadskov_m_max_by_column {

ZagryadskovMMaxByColumnSEQ::ZagryadskovMMaxByColumnSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ZagryadskovMMaxByColumnSEQ::ValidationImpl() {
  bool ifDividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  return (std::get<0>(GetInput()) > 0) && (std::get<1>(GetInput()).size() > 0) && (GetOutput().size() == 0) &&
         ifDividable;
}

bool ZagryadskovMMaxByColumnSEQ::PreProcessingImpl() {
  bool ifDividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  return (std::get<0>(GetInput()) > 0) && (std::get<1>(GetInput()).size() > 0) && ifDividable;
}

bool ZagryadskovMMaxByColumnSEQ::RunImpl() {
  bool ifDividable = std::get<1>(GetInput()).size() % std::get<0>(GetInput()) == 0;
  if ((std::get<0>(GetInput()) == 0) || (std::get<1>(GetInput()).size() == 0) || !ifDividable) {
    return false;
  }

  const auto &n = std::get<0>(GetInput());
  const auto &mat = std::get<1>(GetInput());
  size_t m = mat.size() / n;
  OutType &res = GetOutput();
  OutType rows;
  using T = std::decay_t<decltype(*mat.begin())>;

  size_t j, i;
  res = std::vector<T>(n, std::numeric_limits<T>::lowest());
  T tmp;
  int tmpFlag;
  for (j = 0; j < n; ++j) {
    for (i = 0; i < m; ++i) {
      tmp = mat[j * m + i];
      tmpFlag = tmp > res[j];
      res[j] = tmpFlag * tmp + (!tmpFlag) * res[j];
    }
  }

  return GetOutput().size() > 0;
}

bool ZagryadskovMMaxByColumnSEQ::PostProcessingImpl() {
  return GetOutput().size() > 0;
}

}  // namespace zagryadskov_m_max_by_column
