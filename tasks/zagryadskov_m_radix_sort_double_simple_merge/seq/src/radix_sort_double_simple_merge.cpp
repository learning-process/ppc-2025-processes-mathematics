#include "zagryadskov_m_radix_sort_double_simple_merge/seq/include/radix_sort_double_simple_merge.hpp"

#include <cstddef>
#include <cstdlib>

#include "zagryadskov_m_radix_sort_double_simple_merge/common/include/common.hpp"

namespace zagryadskov_m_radix_sort_double_simple_merge {

ZagryadskovMRadixSortDoubleSimpleMergeSEQ::ZagryadskovMRadixSortDoubleSimpleMergeSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ZagryadskovMRadixSortDoubleSimpleMergeSEQ::ValidationImpl() {
  return !GetInput().empty();
}

bool ZagryadskovMRadixSortDoubleSimpleMergeSEQ::PreProcessingImpl() {
  return true;
}

void ZagryadskovMRadixSortDoubleSimpleMergeSEQ::foffset(uint8_t *mas, size_t size, size_t offset,
                                                        uint64_t count[255ull + 1ull]) {
  size_t i;
  uint64_t tmp;
  memset(count, 0, (255ull + 1ull) * sizeof(uint64_t));
  for (i = 0; i < size * sizeof(double); i += sizeof(double)) {
    count[mas[i + offset]]++;
  }
  tmp = count[0ull];
  count[0ull] = 0ull;
  for (i = 0ull; i < 255ull; i++) {
    std::swap(tmp, count[i + 1ull]);
    count[i + 1ull] += count[i];
  }
}

void ZagryadskovMRadixSortDoubleSimpleMergeSEQ::radix_sort_LSD(double *mas, size_t size) {
  size_t i, j, k;
  uint8_t *pm;
  uint64_t count[255ull + 1ull];
  std::vector<double> vec_buf(size);
  double *mas2 = vec_buf.data();
  pm = reinterpret_cast<uint8_t *>(mas);

  for (i = 0ull; i < sizeof(double); i++) {
    foffset(pm, size, i, count);
    for (j = 0ull; j < size; j++) {
      mas2[count[pm[(j * sizeof(double)) + i]]++] = mas[j];
    }
    std::swap(mas, mas2);
    pm = reinterpret_cast<uint8_t *>(mas);
  }

  k = 0ull;
  if (mas[size - 1ull] < 0.0) {
    for (i = size; i > 0ull; i--) {
      if (mas[i - 1] > 0.0) {
        break;
      }
      mas2[k++] = mas[i - 1ull];
    }

    for (i = 0ull; i < size; i++) {
      if (mas[i] < 0.0) {
        break;
      }
      mas2[k++] = mas[i];
    }

    memcpy(mas, mas2, size * sizeof(double));
  }
}

bool ZagryadskovMRadixSortDoubleSimpleMergeSEQ::RunImpl() {
  GetOutput() = GetInput();
  radix_sort_LSD(GetOutput().data(), GetOutput().size());

  return !GetOutput().empty();
}

bool ZagryadskovMRadixSortDoubleSimpleMergeSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace zagryadskov_m_radix_sort_double_simple_merge
