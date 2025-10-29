#include "barkalova_m_min_val_matr/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>
#include <climits>

#include "barkalova_m_min_val_matr/common/include/common.hpp"
#include "util/include/util.hpp"

namespace barkalova_m_min_val_matr {

BarkalovaMMinValMatrMPI::BarkalovaMMinValMatrMPI(const InType &in) 
{
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<int>();
}

bool BarkalovaMMinValMatrMPI::ValidationImpl() 
{
  const auto& matrix = GetInput();
  if(matrix.empty())
  {
    return false;
  }
  size_t stolb = matrix[0].size();
  for(const auto& row : matrix)
  {
    if(row.size() != stolb)
    {
      return false;
    }
  }
  return true;
}

bool BarkalovaMMinValMatrMPI::PreProcessingImpl() 
{
  if(GetInput().empty())
  {
    size_t stolb = GetInput()[0].size();
    GetOutput() = std::vector<int>(stolb,INT_MAX);
  }
  return true;
}

bool BarkalovaMMinValMatrMPI::RunImpl() 
{
  const auto& matrix = GetInput();
  auto& res = GetOutput();

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  size_t rows = matrix.size();
  size_t stolb = matrix[0].size();
  //распр строк между процессами
  size_t loc_rows = rows/size;
  size_t ostatok = rows%size;
  //опр диапозон строк для текущ процесса
  size_t start_row = rank*loc_rows+std::min(rank,(int)ostatok);
  size_t end_row = start_row +loc_rows+(rank < (int)ostatok ? 1 : 0);

  std::vector<int> loc_min(stolb,INT_MAX);
  for(int i = start_row; i < end_row; ++i)
  {
    for (int j = 0; j < stolb; ++j)
    {
      if(matrix[i][j]<loc_min[j])
      {
        loc_min[j] = matrix[i][j];
      }
    }
  }
//перед сбором результатов
  MPI_Barrier(MPI_COMM_WORLD);

  if(rank == 0)
  {
    res = loc_min;
    std::vector<int> other_min(stolb);
    for(int source = 1; source < size; ++source)
    {
      MPI_Recv(other_min.data(), stolb, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for(int j = 0; j < stolb; ++j)
      {
        if(other_min[j]<res[j])
        {
          res[j] = other_min[j];
        }
      }
    }
  }
  else
  {
    MPI_Send(loc_min.data(), stolb, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool BarkalovaMMinValMatrMPI::PostProcessingImpl() {
  return true;
}

}  // namespace barkalova_m_min_val_matr