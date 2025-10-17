#define BIG_INTEGER_DIVISION_IMPLEMENTED

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <algorithm>

#include <type_traits>

class BigIntegerOverflow : public std::runtime_error {
 public:
  BigIntegerOverflow() : std::runtime_error("BigIntegerOverflow") {
  }
};

class BigIntegerDivisionByZero : public std::runtime_error {
 public:
  BigIntegerDivisionByZero() : std::runtime_error("BigIntegerDivisionByZero") {
  }
};

class BigInteger {
 private:
  using Digit = uint32_t;
  using DoubleDigit = uint64_t;
  static const Digit kBase = 10000;
  static const int kBaseDigits = 4;
  static const size_t kMaxDecimalDigits = 30000;

  std::vector<Digit> digits_;
  bool is_negative_;

  void RemoveLeadingZeros();
  void CheckOverflow() const;
  friend BigInteger Abs(const BigInteger& a);

 public:
  BigInteger();
  BigInteger(int64_t value);  // NOLINT
  explicit BigInteger(const std::string& str);

  template <typename T, typename = std::enable_if_t<std::is_same_v<T, const char*>>>
  explicit BigInteger(T c_str) : BigInteger(std::string(c_str)) {
  }

  BigInteger operator+() const;
  BigInteger operator-() const;

  BigInteger& operator+=(const BigInteger& other);
  BigInteger operator+(const BigInteger& other) const;

  BigInteger& operator-=(const BigInteger& other);
  BigInteger operator-(const BigInteger& other) const;

  BigInteger& operator*=(const BigInteger& other);
  BigInteger operator*(const BigInteger& other) const;

  BigInteger operator/(const BigInteger& other) const;
  BigInteger& operator/=(const BigInteger& other);

  BigInteger operator%(const BigInteger& other) const;
  BigInteger& operator%=(const BigInteger& other);

  BigInteger& operator++();
  BigInteger operator++(int);
  BigInteger& operator--();
  BigInteger operator--(int);

  bool IsNegative() const;
  explicit operator bool() const;

  friend bool operator==(const BigInteger& a, const BigInteger& b);
  friend bool operator!=(const BigInteger& a, const BigInteger& b);
  friend bool operator<(const BigInteger& a, const BigInteger& b);
  friend bool operator<=(const BigInteger& a, const BigInteger& b);
  friend bool operator>(const BigInteger& a, const BigInteger& b);
  friend bool operator>=(const BigInteger& a, const BigInteger& b);

  friend std::ostream& operator<<(std::ostream& os, const BigInteger& num);
  friend std::istream& operator>>(std::istream& is, BigInteger& num);
};