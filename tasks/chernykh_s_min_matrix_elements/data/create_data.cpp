#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// Константы для конфигурации
// Размер матрицы 4096 x 2048
const size_t kRows = 7;
const size_t kCols = 7;
const std::string kFilename = "create_data_101x101.txt";
// Заведомо минимальное число для надежной проверки алгоритма
const double kMinSentinel = -1000.0;

int main() {
  // Устанавливаем высокую точность для вывода double в консоль
  std::cout << std::fixed << std::setprecision(10);

  // Параметры матрицы
  const size_t stroki = kRows;
  const size_t stolbci = kCols;
  const size_t total_elements = stroki * stolbci;

  // Генератор случайных чисел
  std::random_device rd;
  std::mt19937 generator(rd());
  // Диапазон значений
  std::uniform_real_distribution<double> distribution(-500.0, 500.0);

  // Выделение памяти под матрицу (1D представление)
  std::vector<double> matrica(total_elements);

  // 1. Заполнение матрицы случайными числами
  std::generate(matrica.begin(), matrica.end(), [&]() { return distribution(generator); });

  // 2. Внедрение контрольного минимального элемента
  // Выбираем случайную позицию
  std::uniform_int_distribution<size_t> index_dist(0, total_elements - 1);
  size_t min_index = index_dist(generator);

  // Вставляем заведомо минимальный элемент
  matrica[min_index] = kMinSentinel;

  // 3. Открытие файла для записи в текстовом режиме
  std::ofstream out_file(kFilename);
  if (!out_file.is_open()) {
    std::cerr << "Не удалось открыть файл '" << kFilename << "' для записи!\n";
    return 1;
  }

  // Устанавливаем высокую точность для записи double в файл
  out_file << std::fixed << std::setprecision(10);

  // *** ИЗМЕНЕНИЕ: Запись размеров матрицы удалена, в файле только данные. ***

  // Запись данных матрицы построчно (для удобного просмотра)
  for (size_t i = 0; i < total_elements; ++i) {
    out_file << matrica[i];

    // Переход на новую строку после каждого столбца
    if ((i + 1) % stolbci == 0) {
      out_file << "\n";
    } else {
      // Разделение элементов пробелом
      out_file << " ";
    }
  }

  out_file.close();

  std::cout << "Текстовый файл '" << kFilename << "' успешно создан.\n";
  std::cout << "Размер матрицы: " << stroki << " x " << stolbci << " (Размеры НЕ записаны в файл!)\n";
  std::cout << "Ожидаемый минимальный элемент (для проверки алгоритма): " << kMinSentinel
            << " (на 1D позиции: " << min_index << ")\n";

  return 0;
}
