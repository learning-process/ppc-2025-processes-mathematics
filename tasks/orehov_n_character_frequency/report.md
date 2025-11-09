# отчёт

 - задача 1
 - вариант 23
 - сделал: Орехов Никита Антонович
 - группа: 3823Б1ПМоп3

## 1. Introduction
Задача заключается в подсчёте частоты определённого символа в строке. 
Цель - реализовать последовательную и параллельную версию и сравнить их производительность.
Ожидание - выигрыш параллельной версии

## 2. Problem Statement
**Входные данные:**
 - Строка (Берётся из файла)
 - Целевой символ (Берётся из следующей строки в том же файле)

**Выходные данные:**

 - Частота целевого символа в строке

**Ограничения**

 - Строка не должна быть пустой
 - Строка с целевым символом не должна быть пустой

## 3. Baseline Algorithm (Sequential)

В цикле проверяем на совпадение поочередно каждый символ. 
При совпадении увеличиваем счётчик

## 4. Parallelization Scheme

Вначале считаем сколько данных получит каждый процесс. Распределяем их равномерно. 
После каждый процесс считает частоту символа в своей части данных
Дальше с помощью MPI_AllReduce собираем данные и складываем их
И результат отправляется всем процессам

**Распределение данных:**

Данные делятся на равные части между процессами. 
Оставшиеся данные распределяются равномерно между первыми процессами

**Шаблон коммуникаций**

Используется AllReduce для суммирования частот со всех процессов и отправки результата всем

**Роли процессов**

Все равны

## 5. Implementation Details

'''cpp
bool OrehovNCharacterFrequencyMPI::RunImpl() {
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::string str = std::get<0>(GetInput());
  std::string symbol = std::get<1>(GetInput());
  int length = str.length();

  int part_size = length / size;
  int remains = length % size;

  int start = rank * part_size + std::min(rank, remains);
  int end = (rank + 1) * part_size + std::min(rank + 1, remains);

  int local_result = 0;

  for (int i = start; i < end; i++){
    if (str[i] == symbol[0]) local_result++;
  }

  int global_result = 0;

  MPI_Allreduce(&local_result, &global_result, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  GetOutput() = global_result;

  return true;
}
'''

## 6. Experimental Setup

 - Процессор: Процессор	Intel(R) Core(TM) i5-9300H CPU @ 2.40GHz   2.40 GHz
 - Количество ядер: 4
 - RAM: 16
 - OS Version: Windows 10


## 7. Performance

 Mode         | Count | Time, s | Speedup | Efficiency |
|-------------|-------|---------|---------|------------|
| seq         | 1     | 0.04743 | 1.00    | N/A        |
| omp         | 4     | 0.05366 | 0.88    | 22%        |


## 8. Conclusions

Параллельная версия показала замедление. 
Основная причина в отсутствии вычислений, кроме счётчика. 
Время MPI коммуникаций значительно преобладает над временем вычислений

