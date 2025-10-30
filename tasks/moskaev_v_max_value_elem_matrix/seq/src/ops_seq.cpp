#include "moskaev_v_max_value_elem_matrix/seq/include/ops_seq.hpp"

#include <algorithm>

#include "moskaev_v_max_value_elem_matrix/common/include/common.hpp"
#include "util/include/util.hpp"

namespace moskaev_v_max_value_elem_matrix {

MoskaevVMaxValueElemMatrixSEQ::MoskaevVMaxValueElemMatrixSEQ(const InType &in) {
    SetTypeOfTask(GetStaticTypeOfTask());
    GetInput() = in;  // Получаем готовую матрицу из тестов
    GetOutput() = 0;
}

bool MoskaevVMaxValueElemMatrixSEQ::ValidationImpl() {
    return !GetInput().empty() && (GetOutput() == 0);
}

bool MoskaevVMaxValueElemMatrixSEQ::PreProcessingImpl() {
    return true; // Матрица уже готова, переданная из тестов
}

bool MoskaevVMaxValueElemMatrixSEQ::RunImpl() {
    const auto& matrix = GetInput(); // Используем матрицу из входных данных
    
    if (matrix.empty()) {
        return false;
    }

    // Только поиск максимального элемента
    int max_element = matrix[0][0];
    for (const auto& row : matrix) {
        for (int element : row) {
            if (element > max_element) {
                max_element = element;
            }
        }
    }
    
    GetOutput() = max_element;
    return true;
}

bool MoskaevVMaxValueElemMatrixSEQ::PostProcessingImpl() {
    return true;
}

}  // namespace moskaev_v_max_value_elem_matrix