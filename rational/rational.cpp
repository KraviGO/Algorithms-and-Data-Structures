#include "rational.h"

void Rational::ReduceFraction() {
    if (denominator_ < 0) {
        numerator_ *= -1;
        denominator_ *= -1;
    }

    int64_t gcd = std::gcd(std::abs(numerator_), std::abs(denominator_));
    numerator_ /= gcd;
    denominator_ /= gcd;
}

Rational::Rational(int64_t num) : numerator_(num), denominator_(1) {}

Rational::Rational(int64_t num, int64_t den) : numerator_(num), denominator_(den) {
    if (denominator_ == 0) {
        throw RationalDivisionByZero{};
    }

    ReduceFraction();
}

int64_t Rational::GetNumerator() const {
    return numerator_;
}

int64_t Rational::GetDenominator() const {
    return denominator_;
}

void Rational::SetNumerator(int64_t value) {
    numerator_ = value;
    ReduceFraction();
}

void Rational::SetDenominator(int64_t value) {
    if (value == 0) {
        throw RationalDivisionByZero{};
    }
    denominator_ = value;

    ReduceFraction();
}

Rational Rational::operator+(const Rational &other) const {
    Rational result = *this;
    result += other;
    return result;
}

Rational Rational::operator-(const Rational &other) const {
    Rational result = *this;
    result -= other;
    return result;
}

Rational Rational::operator*(const Rational &other) const {
    Rational result = *this;
    result *= other;
    return result;
}

Rational Rational::operator/(const Rational &other) const {
    Rational result = *this;
    result /= other;
    return result;
}

Rational &Rational::operator+=(const Rational &other) {
    numerator_ = numerator_ * other.denominator_ + other.numerator_ * denominator_;
    denominator_ *= other.denominator_;
    ReduceFraction();
    return *this;
}

Rational &Rational::operator-=(const Rational &other) {
    numerator_ = numerator_ * other.denominator_ - other.numerator_ * denominator_;
    denominator_ *= other.denominator_;
    ReduceFraction();
    return *this;
}

Rational &Rational::operator*=(const Rational &other) {
    numerator_ *= other.numerator_;
    denominator_ *= other.denominator_;
    ReduceFraction();
    return *this;
}

Rational &Rational::operator/=(const Rational &other) {
    numerator_ *= other.denominator_;
    denominator_ *= other.numerator_;

    if (denominator_ == 0) {
        throw RationalDivisionByZero{};
    }

    ReduceFraction();

    return *this;
}

Rational Rational::operator+() const {
    return *this;
}

Rational Rational::operator-() const {
    return {-numerator_, denominator_};
}

Rational &Rational::operator++() {
    numerator_ += denominator_;
    return *this;
}

Rational Rational::operator++(int) {
    Rational tmp = *this;
    numerator_ += denominator_;
    return tmp;
}

Rational &Rational::operator--() {
    numerator_ -= denominator_;
    return *this;
}

Rational Rational::operator--(int) {
    Rational tmp = *this;
    numerator_ -= denominator_;
    return tmp;
}

bool Rational::operator==(const Rational &other) const {
    return (numerator_ == other.numerator_) && (denominator_ == other.denominator_);
}

bool Rational::operator!=(const Rational &other) const {
    return (numerator_ != other.numerator_) || (denominator_ != other.denominator_);
}

bool Rational::operator<(const Rational &other) const {
    return numerator_ * other.denominator_ < other.numerator_ * denominator_;
}

bool Rational::operator<=(const Rational &other) const {
    if (*this == other) {
        return true;
    }
    return *this < other;
}

bool Rational::operator>(const Rational &other) const {
    return numerator_ * other.denominator_ > other.numerator_ * denominator_;
}

bool Rational::operator>=(const Rational &other) const {
    if (*this == other) {
        return true;
    }
    return *this > other;
}

bool operator==(int64_t lhs, const Rational &rhs) {
    return Rational(lhs) == rhs;
}

bool operator!=(int64_t lhs, const Rational &rhs) {
    return Rational(lhs) != rhs;
}

bool operator<(int64_t lhs, const Rational &rhs) {
    return Rational(lhs) < rhs;
}

bool operator<=(int64_t lhs, const Rational &rhs) {
    return Rational(lhs) <= rhs;
}

bool operator>(int64_t lhs, const Rational &rhs) {
    return Rational(lhs) > rhs;
}

bool operator>=(int64_t lhs, const Rational &rhs) {
    return Rational(lhs) >= rhs;
}

std::ostream &operator<<(std::ostream &os, const Rational &obj) {
    os << obj.numerator_;

    if (obj.denominator_ != 1) {
        os << "/" << obj.denominator_;
    }

    return os;
}

std::istream& operator>>(std::istream& is, Rational& obj) {
    int64_t num = 0;
    int64_t den = 1;
    char sep = 0;

    is >> num;
    if (is.peek() == '/') {
        is >> sep >> den;
    } else {
        obj = Rational(num);
        return is;
    }

    if (den == 0) {
        throw RationalDivisionByZero{};
    }

    obj = Rational(num, den);
    return is;
}
