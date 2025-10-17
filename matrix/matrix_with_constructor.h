#pragma once

#include <stdexcept>
#include <initializer_list>
#include <iostream>

template<class T>
using InitList = std::initializer_list<T>;

class MatrixIsDegenerateError : public std::runtime_error {
public:
    MatrixIsDegenerateError() : std::runtime_error("MatrixIsDegenerateError") {
    }
};

class MatrixOutOfRange : public std::out_of_range {
public:
    MatrixOutOfRange() : std::out_of_range("MatrixOutOfRange") {
    }
};

template <class T = int, size_t N = 0, size_t M = 0>
class Matrix {
private:
    T matrix_[N][M];

public:
    Matrix() {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] = T();
            }
        }
    }

    Matrix(const Matrix<T, N, M>& other) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] = other(i, j);
            }
        }
    }

    Matrix(InitList<T> init) {
        size_t i = 0;
        size_t j = 0;
        for (const T& elem : init) {
            if (i >= N) {
                throw std::runtime_error("Incorrect number of elements");
            }

            matrix_[i][j] = elem;

            ++j;
            if (j == M) {
                j = 0;
                ++i;
            }
        }

        for (; i < N; ++i) {
            for (; j < M; ++j) {
                matrix_[i][j] = T();
            }
        }
    }

    Matrix(InitList<InitList<T>> init) {
        size_t i = 0;
        size_t j = 0;
        for (const InitList<T>& list : init) {
            for (const T& elem : list) {
                if (i >= N) {
                    throw std::runtime_error("Incorrect number of elements");
                }

                matrix_[i][j] = elem;

                ++j;
                if (j == M) {
                    j = 0;
                    ++i;
                }
            }
        }

        for (; i < N; ++i) {
            for (; j < M; ++j) {
                matrix_[i][j] = T();
            }
        }
    }

public:
    [[nodiscard]] size_t RowsNumber() const {
        return N;
    };

    [[nodiscard]] size_t ColumnsNumber() const {
        return M;
    };

    T operator()(size_t n, size_t m) const {
        return matrix_[n][m];
    }

    T& operator()(size_t n, size_t m) {
        return matrix_[n][m];
    }

    T At(size_t n, size_t m) const {
        if (n >= N || m >= M) {
            throw MatrixOutOfRange{};
        }
        return matrix_[n][m];
    }

    T& At(size_t n, size_t m) {
        if (n >= N || m >= M) {
            throw MatrixOutOfRange{};
        }
        return matrix_[n][m];
    }

    Matrix<T, N, M>& operator+=(const Matrix<T, N, M> other) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] += other(i, j);
            }
        }
        return *this;
    }

    Matrix<T, N, M> operator+(const Matrix<T, N, M> other) const {
        Matrix<T, N, M> tmp = *this;
        tmp += other;
        return tmp;
    }

    Matrix<T, N, M>& operator-=(const Matrix<T, N, M> other) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] -= other(i, j);
            }
        }
        return *this;
    }

    Matrix<T, N, M> operator-(const Matrix<T, N, M> other) const {
        Matrix<T, N, M> tmp = *this;
        tmp -= other;
        return tmp;
    }

    template<size_t K = 0>
    Matrix<T, N, K> operator*(const Matrix<T, M, K> other) const {
        Matrix<T, N, K> tmp;

        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < K; ++j) {
                tmp(i, j) = 0;
                for (size_t k = 0; k < M; ++k) {
                    tmp(i, j) += matrix_[i][k] * other(k, j);
                }
            }
        }

        return tmp;
    }

    Matrix<T, N, M>& operator*=(const Matrix<T, M, M> other) {
        *this = *this * other;
        return *this;
    }

    Matrix<T, N, M>& operator*=(T num) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] *= num;
            }
        }
        return *this;
    }

    Matrix<T, N, M> operator*(T num) const {
        Matrix<T, N, M> tmp = *this;
        tmp *= num;
        return tmp;
    }

    Matrix<T, N, M>& operator/=(T num) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] /= num;
            }
        }
        return *this;
    }

    Matrix<T, N, M> operator/(T num) const {
        Matrix<T, N, M> tmp = *this;
        tmp /= num;
        return tmp;
    }

    Matrix<T, N, M>& operator=(Matrix<T, N, M> other) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                matrix_[i][j] = other(i, j);
            }
        }
        return *this;
    }

    bool operator==(const Matrix<T, N, M> other) const {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                if (matrix_[i][j] != other(i, j)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Matrix<T, N, M> other) const {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                if (matrix_[i][j] != other(i, j)) {
                    return true;
                }
            }
        }
        return false;
    }
};

template <class T = int, size_t N = 0, size_t M = 0>
Matrix<T, M, N> GetTransposed(const Matrix<T, N, M> matrix) {
    Matrix<T, M, N> matrix_t;

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            matrix_t(j, i) = matrix(i, j);
        }
    }

    return matrix_t;
}

template <class T = int, size_t N = 0>
void Transpose(Matrix<T, N, N>& matrix) {
    matrix = GetTransposed(matrix);
}

template <class T = int, size_t N = 0>
T Trace(const Matrix<T, N, N> &matrix) {
    T trace = T();
    for (size_t i = 0; i < N; ++i) {
        trace += matrix(i, i);
    }
    return trace;
}

template <class T = int, size_t N = 0>
T Determinant(const Matrix<T, N, N> &matrix) {
    Matrix<T, N, N> a = matrix;
    T det = 1;
    T factor = 1;
    int sign = 1;

    for (size_t i = 0; i < N; ++i) {
        size_t pivot = i;
        while (pivot < N && a(pivot, i) == T{}) {
            ++pivot;
        }

        if (pivot == N) {
            return T{};
        }

        if (i != pivot) {
            for (size_t j = 0; j < N; ++j) {
                std::swap(a(j, i), a(j, pivot));
            }
            sign *= -1;
        }

        det *= a(i, i);

        for (size_t j = i + 1; j < N; ++j) {
            T tmp = a(j, i);
            factor *= a(i, i);
            for (size_t k = i; k < N; ++k) {
                a(j, k) *= a(i, i);
                a(j, k) -= tmp * a(i, k);
            }
        }
    }

    return det / factor * sign;
}

template <class T = int, size_t N = 0>
Matrix<T, N, N> GetInversed(Matrix<T, N, N> matrix) {
    Matrix<T, N, N> a = matrix;
    Matrix<T, N, N> inv;

    for (size_t i = 0; i < N; ++i) {
        inv(i, i) = 1;
    }

    for (size_t i = 0; i < N; ++i) {
        size_t pivot = i;
        while (pivot < N && a(pivot, i) == T{}) {
            ++pivot;
        }

        if (pivot == N) {
            throw MatrixIsDegenerateError{};
        }

        if (pivot != i) {
            for (size_t j = 0; j < N; ++j) {
                std::swap(a(i, j), a(pivot, j));
                std::swap(inv(i, j), inv(pivot, j));
            }
        }

        T pivot_val = a(i, i);
        for (size_t j = 0; j < N; ++j) {
            a(i, j) = a(i, j) / pivot_val;
            inv(i, j) = inv(i, j) / pivot_val;
        }

        for (size_t k = 0; k < N; ++k) {
            if (k == i) {
                continue;
            }

            T factor = a(k, i);
            for (size_t j = 0; j < N; ++j) {
                a(k, j) -= factor * a(i, j);
                inv(k, j) -= factor * inv(i, j);
            }
        }
    }

    return inv;
}

template <class T = int, size_t N = 0>
void Inverse(Matrix<T, N, N> &matrix) {
    matrix = GetInversed(matrix);
}

template <class T, size_t N, size_t M>
Matrix<T, N, M> operator*(T num, const Matrix<T, N, M> matrix) {
    return matrix * num;
}

template <class T, size_t N, size_t M>
Matrix<T, N, M> operator/(T num, const Matrix<T, N, M> &matrix) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            matrix(i, j) = num / matrix(i, j);
        }
    }
    return matrix;
}

template <class T = int, size_t N = 0, size_t M = 0>
std::istream &operator>>(std::istream &is, Matrix<T, N, M>& obj) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            is >> obj(i, j);
        }
    }
    return is;
}

template <class T = int, size_t N = 0, size_t M = 0>
std::ostream &operator<<(std::ostream &os, const Matrix<T, N, M>& obj) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            os << obj(i, j) << (j == M-1 ? "" : " ");
        }
        os << '\n';
    }

    return os;
}
