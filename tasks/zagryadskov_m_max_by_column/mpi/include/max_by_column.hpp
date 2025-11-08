#pragma once

#include <mpi.h>

#include <cstdint>
#include <type_traits>

#include "task/include/task.hpp"
#include "zagryadskov_m_max_by_column/common/include/common.hpp"

namespace zagryadskov_m_max_by_column {

class ZagryadskovMMaxByColumnMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit ZagryadskovMMaxByColumnMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

  template <typename T>
  MPI_Datatype GetMpiType() {
    MPI_Datatype datatype = MPI_DATATYPE_NULL;
    if (std::is_same_v<T, int8_t>) {
      datatype = MPI_CHAR;
    } else if (std::is_same_v<T, uint8_t>) {
      datatype = MPI_UNSIGNED_CHAR;
    } else if (std::is_same_v<T, int16_t>) {
      datatype = MPI_SHORT;
    } else if (std::is_same_v<T, uint16_t>) {
      datatype = MPI_UNSIGNED_SHORT;
    } else if (std::is_same_v<T, int32_t>) {
      datatype = MPI_INT;
    } else if (std::is_same_v<T, uint32_t>) {
      datatype = MPI_UNSIGNED;
    } else if (std::is_same_v<T, int64_t>) {
      datatype = MPI_LONG;
    } else if (std::is_same_v<T, uint64_t>) {
      datatype = MPI_UNSIGNED_LONG;
    } else if (std::is_same_v<T, float>) {
      datatype = MPI_FLOAT;
    } else if (std::is_same_v<T, double>) {
      datatype = MPI_DOUBLE;
    }

    return datatype;
  }
};

}  // namespace zagryadskov_m_max_by_column
