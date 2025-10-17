#pragma once

#include <cstdint>
#include <iostream>
#include <numeric>
#include <cmath>
#include <string>
#include <stdexcept>

class RationalDivisionByZero : public std::runtime_error {
public:
    RationalDivisionByZero() : std::runtime_error("RationalDivisionByZero") {
    }
};

class Rational {

private:
    int64_t numerator_ = 0;
    int64_t denominator_ = 1;

    void ReduceFraction();

public:
    Rational() = default;
    Rational(int64_t numerator); // NOLINT
    Rational(int64_t numerator, int64_t denominator);

    [[nodiscard]] int64_t GetNumerator() const;
    [[nodiscard]] int64_t GetDenominator() const;

    void SetNumerator(int64_t);
    void SetDenominator(int64_t);

    [[nodiscard]] Rational operator +(const Rational &other) const;
    [[nodiscard]] Rational operator -(const Rational &other) const;
    [[nodiscard]] Rational operator *(const Rational &other) const;
    [[nodiscard]] Rational operator /(const Rational &other) const;

    Rational& operator +=(const Rational &other);
    Rational& operator -=(const Rational &other);
    Rational& operator *=(const Rational &other);
    Rational& operator /=(const Rational &other);

    [[nodiscard]] Rational operator +() const;
    [[nodiscard]] Rational operator -() const;
    Rational &operator ++();
    Rational operator ++(int);
    Rational &operator --();
    Rational operator --(int);

    bool operator ==(const Rational &other) const;
    bool operator !=(const Rational &other) const;
    bool operator <(const Rational &other) const;
    bool operator <=(const Rational &other) const;
    bool operator >(const Rational &other) const;
    bool operator >=(const Rational &other) const;

    friend bool operator ==(int64_t lhs, const Rational &rhs);
    friend bool operator !=(int64_t lhs, const Rational &rhs);
    friend bool operator <(int64_t lhs, const Rational &rhs);
    friend bool operator <=(int64_t lhs, const Rational &rhs);
    friend bool operator >(int64_t lhs, const Rational &rhs);
    friend bool operator >=(int64_t lhs, const Rational &rhs);

    friend std::ostream& operator <<(std::ostream &os, const Rational &obj);
    friend std::istream& operator >>(std::istream &is, Rational &obj);
};
