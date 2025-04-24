#include <iostream>
#include <cstdlib>
#include <ctime>

template <typename T>
void initializeArray(T* array, int size, T min, T max) {
    for (int i = 0; i < size; ++i) {
        array[i] = min + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX / (max - min)));
    }
}

template <typename T>
int findMaxElementIndex(const T* array, int size) {
    int maxIndex = 0;
    for (int i = 1; i < size; ++i) {
        if (array[i] > array[maxIndex]) {
            maxIndex = i;
        }
    }
    return maxIndex;
}

// Общий шаблон для всех типов
template <typename T>
void printElementsAfterMax(const T* array, int size) {
    int maxIndex = findMaxElementIndex(array, size);
    std::cout << "Elements after max (" << array[maxIndex] << "[" << maxIndex << "]): ";
    for (int i = maxIndex + 1; i < size; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

// Специализация для double - выводит только массив
template<>
void printElementsAfterMax(const double* array, int size) {
}

template <typename T>
void printArray(const T* array, int size) {
    std::cout << "Array elements: ";
    for (int i = 0; i < size; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    int size;
    do {
        std::cout << "Enter array size (positive number): ";
        std::cin >> size;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            size = 0;
        }
    } while (size <= 0);

    int* intArray = new int[size];
    double* doubleArray = new double[size];

    // Работа с массивом int
    initializeArray(intArray, size, 1, 100);
    printArray(intArray, size);
    printElementsAfterMax(intArray, size);

    // Работа с массивом double
    initializeArray(doubleArray, size, 1.0, 100.0);
    printArray(doubleArray, size);
    printElementsAfterMax(doubleArray, size);

    delete[] intArray;
    delete[] doubleArray;

    return 0;
}