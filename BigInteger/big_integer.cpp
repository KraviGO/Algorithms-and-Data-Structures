#include "big_integer.h"

BigInteger::BigInteger() : is_negative_(false) {
  digits_.push_back(0);
}

BigInteger::BigInteger(int64_t value) {
  is_negative_ = value < 0;
  uint64_t abs_val = is_negative_ ? -value : value;

  while (abs_val > 0) {
    digits_.push_back(abs_val % kBase);
    abs_val /= kBase;
  }

  if (digits_.empty()) {
    digits_.push_back(0);
  }

  CheckOverflow();
}

BigInteger::BigInteger(const std::string& s) {
  if (s.size() - 1 > kMaxDecimalDigits) {
    throw BigIntegerOverflow{};
  }

  is_negative_ = !s.empty() && s[0] == '-';
  size_t pos = is_negative_;

  for (size_t i = s.size(); i > pos;) {
    size_t len = std::min(static_cast<size_t>(kBaseDigits), i - pos);
    i -= len;
    digits_.push_back(std::stoi(s.substr(i, len)));
  }

  if (digits_.empty()) {
    digits_.push_back(0);
    is_negative_ = false;
  }

  CheckOverflow();
}

bool BigInteger::IsNegative() const {
  return is_negative_;
}

BigInteger::operator bool() const {
  return digits_.size() != 1 || digits_[0] != 0;
}

void BigInteger::CheckOverflow() const {
  if (digits_.size() * kBaseDigits > kMaxDecimalDigits) {
    throw BigIntegerOverflow{};
  }
}

void BigInteger::RemoveLeadingZeros() {
  while (digits_.size() > 1 && digits_.back() == 0) {
    digits_.pop_back();
  }

  if (digits_.size() == 1 && digits_[0] == 0) {
    is_negative_ = false;
  }
}

BigInteger Abs(const BigInteger& a) {
  return a.IsNegative() ? -a : a;
}

BigInteger BigInteger::operator+() const {
  return *this;
}

BigInteger BigInteger::operator-() const {
  BigInteger copy = *this;

  if (*this) {
    copy.is_negative_ = !copy.is_negative_;
  }

  return copy;
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if (!other) {
    return *this;
  }

  if (is_negative_ != other.is_negative_) {
    return *this -= -other;
  }

  size_t max_len = std::max(digits_.size(), other.digits_.size());
  digits_.resize(max_len, 0);
  Digit carry = 0;

  for (size_t i = 0; i < max_len || carry; ++i) {
    if (i == digits_.size()) {
      digits_.push_back(0);
    }

    Digit sum = digits_[i] + carry;

    if (i < other.digits_.size()) {
      sum += other.digits_[i];
    }

    digits_[i] = sum % kBase;
    carry = sum / kBase;
  }
  CheckOverflow();

  return *this;
}

BigInteger BigInteger::operator+(const BigInteger& other) const {
  BigInteger res = *this;
  res += other;
  return res;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
  if (!other) {
    return *this;
  }

  if (is_negative_ != other.is_negative_) {
    *this += -other;
    return *this;
  }

  if (Abs(*this) < Abs(other)) {
    BigInteger tmp = other;
    tmp -= *this;
    *this = tmp;
    is_negative_ = !is_negative_;
    return *this;
  }

  Digit borrow = 0;
  for (size_t i = 0; i < digits_.size(); ++i) {
    int64_t cur = static_cast<int64_t>(digits_[i]) - borrow;

    if (i < other.digits_.size()) {
      cur -= other.digits_[i];
    }

    if (cur < 0) {
      cur += kBase;
      borrow = 1;
    } else {
      borrow = 0;
    }

    digits_[i] = static_cast<Digit>(cur);
  }

  RemoveLeadingZeros();
  CheckOverflow();

  return *this;
}

BigInteger BigInteger::operator-(const BigInteger& other) const {
  BigInteger res = *this;
  res -= other;
  return res;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  std::vector<Digit> res(digits_.size() + other.digits_.size(), 0);

  for (size_t i = 0; i < digits_.size(); ++i) {
    DoubleDigit carry = 0;
    for (size_t j = 0; j < other.digits_.size() || carry != 0; ++j) {
      DoubleDigit cur =
          res[i + j] + static_cast<DoubleDigit>(digits_[i]) * (j < other.digits_.size() ? other.digits_[j] : 0) + carry;

      res[i + j] = static_cast<Digit>(cur % kBase);
      carry = cur / kBase;
    }
  }

  digits_ = std::move(res);
  is_negative_ = (is_negative_ != other.is_negative_);

  RemoveLeadingZeros();
  CheckOverflow();

  return *this;
}

BigInteger BigInteger::operator*(const BigInteger& other) const {
  BigInteger result = *this;
  result *= other;
  return result;
}

BigInteger BigInteger::operator/(const BigInteger& other) const {
  if (other == 0) {
    throw BigIntegerDivisionByZero{};
  }

  BigInteger abs_dividend = Abs(*this);
  BigInteger abs_divisor = Abs(other);
  BigInteger result;

  int32_t result_len =
      static_cast<int32_t>(abs_dividend.digits_.size()) - static_cast<int32_t>(abs_divisor.digits_.size()) + 1;

  if (result_len <= 0) {
    return {0};
  }

  result.digits_.resize(result_len, 0);

  BigInteger current;
  for (int i = static_cast<int>(abs_dividend.digits_.size() - 1); i >= static_cast<int>(result_len - 1); --i) {
    current = current * kBase + abs_dividend.digits_[i];
  }

  for (size_t i = 0; i < static_cast<size_t>(result_len); ++i) {
    Digit left = 0;
    Digit right = kBase + 1;
    while (right - left > 1) {
      Digit mid = (left + right) / 2;

      if (abs_divisor * mid <= current) {
        left = mid;
      } else {
        right = mid;
      }
    }

    result.digits_[result_len - i - 1] = left;
    current -= abs_divisor * left;

    if (i + 1 != static_cast<size_t>(result_len)) {
      current = current * kBase + abs_dividend.digits_[result_len - i - 2];
    }
  }

  result.is_negative_ = (is_negative_ != other.is_negative_);
  result.RemoveLeadingZeros();

  if (result.digits_.empty()) {
    result.digits_.push_back(0);
  }

  result.CheckOverflow();

  return result;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
  *this = *this / other;
  return *this;
}

BigInteger BigInteger::operator%(const BigInteger& other) const {
  return *this - *this / other * other;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  *this = *this % other;
  return *this;
}

BigInteger& BigInteger::operator++() {
  *this += BigInteger(1);
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger temp = *this;
  ++(*this);
  return temp;
}

BigInteger& BigInteger::operator--() {
  *this -= BigInteger(1);
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger temp = *this;
  --(*this);
  return temp;
}

bool operator==(const BigInteger& a, const BigInteger& b) {
  return a.is_negative_ == b.is_negative_ && a.digits_ == b.digits_;
}

bool operator!=(const BigInteger& a, const BigInteger& b) {
  return !(a == b);
}

bool operator<(const BigInteger& a, const BigInteger& b) {
  if (a.is_negative_ != b.is_negative_) {
    return a.is_negative_;
  }

  if (a.digits_.size() != b.digits_.size()) {
    return a.is_negative_ ? a.digits_.size() > b.digits_.size() : a.digits_.size() < b.digits_.size();
  }

  for (size_t i = a.digits_.size(); i-- > 0;) {
    if (a.digits_[i] != b.digits_[i]) {
      return a.is_negative_ ? a.digits_[i] > b.digits_[i] : a.digits_[i] < b.digits_[i];
    }
  }

  return false;
}

bool operator<=(const BigInteger& a, const BigInteger& b) {
  return !(b < a);
}

bool operator>(const BigInteger& a, const BigInteger& b) {
  return b < a;
}

bool operator>=(const BigInteger& a, const BigInteger& b) {
  return !(a < b);
}

std::ostream& operator<<(std::ostream& os, const BigInteger& num) {
  if (num.IsNegative()) {
    os << '-';
  }

  auto it = num.digits_.rbegin();
  os << *it++;

  while (it != num.digits_.rend()) {
    os.width(BigInteger::kBaseDigits);
    os.fill('0');
    os << *it++;
  }

  return os;
}

std::istream& operator>>(std::istream& is, BigInteger& num) {
  std::string s;
  is >> s;

  num = BigInteger(s);

  return is;
}