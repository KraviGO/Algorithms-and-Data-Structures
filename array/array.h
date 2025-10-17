#define ARRAY_TRAITS_IMPLEMENTED

#pragma once

#include <cstdlib>
#include <stdexcept>
#include <algorithm>

class ArrayOutOfRange : public std::out_of_range {
public:
    ArrayOutOfRange() : std::out_of_range("ArrayOutOfRange") {
    }
};

template <class T, size_t N>
class Array {
public:
    T arr[N];

    const T& operator[] (const size_t idx) const {
        return arr[idx];
    };

    T& operator[] (const size_t idx) {
        return arr[idx];
    };

    const T& At (const size_t idx) const {
        if (idx >= N) {
            throw ArrayOutOfRange{};
        }

        return arr[idx];
    };

     T& At (const size_t idx) {
        if (idx >= N) {
            throw ArrayOutOfRange{};
        }

        return arr[idx];
    };

    const T& Front() const {
        return arr[0];
    }

    T& Front() {
        return arr[0];
    }

    const T& Back() const {
        return arr[N-1];
    }

    T& Back() {
        return arr[N-1];
    }

    const T* Data() const {
        return arr;
    }

    T* Data() {
        return arr;
    }

    [[nodiscard]] size_t Size() const {
        return N;
    }

    [[nodiscard]] bool Empty() const {
        return N == 0;
    }

    void Fill(const T& value) {
        for (size_t i = 0; i < N; ++i) {
            arr[i] = value;
        }
    }

    void Swap(Array<T, N>& other) {
        for (size_t i = 0; i < N; ++i) {
            std::swap(arr[i], other[i]);
        }
    }
};

template <typename T>
size_t GetSize(const T&) {
    return 0;
}

template <typename T, size_t N>
size_t GetSize(const T (&)[N]) {
    return N;
}

template <typename T>
size_t GetRank(const T&) {
    return 0;
}

template <typename T, size_t N>
size_t GetRank(const T (&array)[N]) {
    return 1 + GetRank(array[0]);
}

template <typename T>
size_t GetNumElements(const T&) {
    return 1;
}

template <typename T, size_t N>
size_t GetNumElements(const T (&array)[N]) {
    return N * GetNumElements(array[0]);
}
