#pragma once

#include <functional>

template<typename T, typename Cmp = std::less<T>>
void quickSort(T* arr, size_t n, Cmp cmp = Cmp()) {
    if (n <= 1) {
        return;
    }

    T pivot = arr[rand() % n];
    size_t left = 0;
    size_t right = n - 1;
    while (true) {
        while (cmp(arr[left], pivot)) {
            ++left;
        }
        while (right >= 0 && cmp(pivot, arr[right])) {
            --right;
        }
        if (left >= right) {
            break;
        }
	std::swap(arr[left], arr[right]);
        ++left;
        --right;
    }
    quickSort(arr, left, cmp);
    quickSort(arr + left, n - left, cmp);
}
