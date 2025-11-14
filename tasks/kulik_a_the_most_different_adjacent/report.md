# Нахождение наиболее отличающихся по значению соседних элементов вектора

- Студент: Кулик Артур Игоревич, группа 3823Б1ПМоп3
- Технология: SEQ, MPI
- Вариант: 8

## 1. Введение
Для решения сложных вычислительных задач (hpc) на практике используются кластерные системы с большим количеством многоядерных процессоров. Для коммуникации между процессами, запущенными на кластере, используют технологию MPI (Message Passing Interface). Цель работы - распараллелить алгоритм поиска максимальной разности между соседними элементами с использованием MPI для уменьшения времени выполнения при работе с большим вектором на нескольких потоках. 

## 2. Постановка задачи
Задача: Найти наиболее отличающиеся по значению соседние элементы вещественного вектора.

**Входные данные:** вектор `std::vector<double>`.
**Выходные данные:** пара неотрицательных целых чисел - позиции элементов `std::pair<size_t,size_t>`.

**Ограничения к входным данным:** длина вектора не меньше 2.

**Необходимо реализовать:**
1. последовательную (**SEQ**) и параллельную (**MPI**) версию алгоритма.
2. Покрытие кода тестами, проверяющими функционал и производительность. 

## 3. Описание последовательного алгоритма
Формализируя задачу, необходимо найти следующую величину: 
max(|A[ i ] - A[ i - 1 ]|) для i = 1, ..., (n-1); где n - это размер вектора.
Алгоритм представляет собой одноразовый проход по вектору, где на каждой итерации мы сравниваем модуль разницы двух соседних элементов с текущим максимумом разности. Если эта разница превышает максимум, то сама становится максимумом, а индекс элементов, на которых она достигнута, - новым ответом. 

## 4. Схема распараллеливания
- For MPI: data distribution, communication pattern/topology, rank roles.
Diagrams or short pseudocode are welcome.

## 5. Implementation Details
- Code structure (files, key classes/functions)
- Important assumptions and corner cases
- Memory usage considerations

## 6. Experimental Setup
- Hardware/OS: CPU model, cores/threads, RAM, OS version
- Toolchain: compiler, version, build type (Release/RelWithDebInfo)
- Environment: PPC_NUM_THREADS / PPC_NUM_PROC, other relevant vars
- Data: how test data is generated or sourced (relative paths)

## 7. Results and Discussion

### 7.1 Correctness
Briefly explain how correctness was verified (reference results, invariants, unit tests).

### 7.2 Performance
Present time, speedup and efficiency. Example table:

| Mode        | Count | Time, s | Speedup | Efficiency |
|-------------|-------|---------|---------|------------|
| seq         | 1     | 1.234   | 1.00    | N/A        |
| omp         | 2     | 0.700   | 1.76    | 88.0%      |
| omp         | 4     | 0.390   | 3.16    | 79.0%      |

Optionally add plots (use relative paths), and discuss bottlenecks and scalability limits.

## 8. Conclusions
Summarize findings and limitations.

## 9. References
1. <Article/Book/Doc URL>
2. <Another source>

## Appendix (Optional)
```cpp
// Short, readable code excerpts if needed
```