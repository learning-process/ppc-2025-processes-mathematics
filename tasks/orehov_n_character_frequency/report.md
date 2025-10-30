# отчёт

## введение
задача 1
вариант 23
сделал: Орехов Никита Антонович
группа: 3823Б1ПМоп3

## постановка задача
Задача посчитать частоту определённого символа в строке. 
Реализовать последовательную и паралелльную версию. 
Сравнить их производительность. 

На вход приходит строка и символ. Данные берем из файлов. 

## описание алгоритма

Проверяем не пустая ли строка. 
Просто проходимся по всей строке, сравния с нашим символом. 
При совпадении увеличиваем счётчик

## Описание схемы параллельного алгоритма (MPI)

Вначале считаем сколько данных получит каждый процесс. Распределяем их равномерно. 
После каждый процесс считает частоту символа в своей части данных
Дальше с помощью MPI_AllReduce собираем данные и складываем их
И результат отправляется всем процессам

## Результаты экспериментов

Последовательная версия - 5 ms
Параллельная версия - 11 ms

## Выводы из результатов

Параллельная версия показала более лучшую производительность. 
Ускорение получилось примерно 2.2

## Приложение

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