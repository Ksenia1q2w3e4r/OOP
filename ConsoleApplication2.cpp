#include <iostream>
#include <chrono>

int main() {
    int* fibonacci = new int[2]; // Начальные числа Фибоначчи
    fibonacci[0] = 0;
    fibonacci[1] = 1;
    int size = 2;
    int N;
    bool firstIteration = true; // Флаг для отслеживания первой итерации

    auto start_time = std::chrono::steady_clock::now(); // Начало отсчета времени

    while (true) {
        std::cout << "Enter number (0 for exit): ";
        std::cin >> N;

        if (N == 0) break;

        // Вывод уже посчитанных чисел
        for (int i = 0; i < size; ++i) {
            if (fibonacci[i] > N) break;
            if (!firstIteration) {
                std::cout << "*" << fibonacci[i] << " ";
            }
            else {
                std::cout << fibonacci[i] << " ";
            }
        }

        // Если необходимо, досчитываем числа Фибоначчи
        while (fibonacci[size - 1] <= N) {
            int nextFib = fibonacci[size - 1] + fibonacci[size - 2];
            if (nextFib > N) break;

            // Расширение массива
            int* newFibonacci = new int[size + 1];
            for (int i = 0; i < size; ++i) {
                newFibonacci[i] = fibonacci[i];
            }
            newFibonacci[size] = nextFib;

            delete[] fibonacci;
            fibonacci = newFibonacci;
            ++size;

            std::cout << nextFib << " ";
        }

        std::cout << std::endl; // Завершение строки после вывода всех чисел
        firstIteration = false; // Снимаем флаг после первой итерации
    }

    delete[] fibonacci; // Освобождение памяти

    auto end_time = std::chrono::steady_clock::now(); // Конец отсчета времени
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    // Вывод времени выполнения
    std::cout << "Program execution time : "
        << duration.count() / 60 << " minutes "
        << duration.count() % 60 << " seconds" << std::endl;

    return 0;
}