#include "krymova_k_lex_order/seq/include/ops_seq.hpp"

#include <algorithm>

#include "krymova_k_lex_order/common/include/common.hpp"
#include "util/include/util.hpp"

namespace krymova_k_lex_order {
KrymovaKLexSEQ::KrymovaKLexSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KrymovaKLexSEQ::ValidationImpl() {
  return true;
}

bool KrymovaKLexSEQ::PreProcessingImpl() {
  return true;
}

bool KrymovaKLexSEQ::RunImpl() {
  const std::string &str1 = std::get<0>(GetInput());
  const std::string &str2 = std::get<1>(GetInput());

  size_t len1 = str1.length();
  size_t len2 = str2.length();
  size_t min_len = (len1 < len2) ? len1 : len2;
  for (size_t i = 0; i < min_len; ++i) {
    if (str1[i] != str2[i]) {
      GetOutput() = (str1[i] < str2[i]) ? -1 : 1;
      return true;
    }
  }
  if (len1 < len2) {
    GetOutput() = -1;
  } else if (len1 > len2) {
    GetOutput() = 1;
  } else {
    GetOutput() = 0;
  }
  return true;
}

bool KrymovaKLexSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace krymova_k_lex_order
