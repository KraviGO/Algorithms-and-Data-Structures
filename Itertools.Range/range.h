#define REVERSE_RANGE_IMPLEMENTED

#pragma once

class RangeIterator {
private:
  int value_;
  int step_;

public:
  RangeIterator(int value, int step) : value_(value), step_(step) {}

  int operator*() const {
    return value_;
  }

  RangeIterator& operator++() {
    value_ += step_;
    return *this;
  }

  bool operator!=(const RangeIterator& other) const {
    return step_ != 0 && (step_ > 0 ? value_ < other.value_ : value_ > other.value_);
  }
};

class RangeRevIterator {
private:
  int value_;
  int step_;

public:
  RangeRevIterator(int value, int step) : value_(value), step_(step) {}

  int operator*() const {
    return value_;
  }

  RangeRevIterator& operator++() {
    value_ -= step_;
    return *this;
  }

  bool operator!=(const RangeRevIterator& other) const {
    return step_ != 0 && step_ > 0 ? value_ > other.value_ : value_ < other.value_;
  }
};

class Range {
private:
  int begin_;
  int end_;
  int step_;

public:
  explicit Range(int end) : begin_(0), end_(end), step_(1) {}
  Range(int begin, int end) : begin_(begin), end_(end), step_(1) {}
  Range(int begin, int end, int step) : begin_(begin), end_(end), step_(step) {}

  [[nodiscard]] RangeIterator begin() const { // NOLINT
    return {begin_, step_};
  }

  [[nodiscard]] RangeIterator end() const { // NOLINT
    return {end_, step_};
  }

  [[nodiscard]] RangeRevIterator rbegin() const { // NOLINT
    if (step_ == 0) {
      return rend();
    }

    int d = (end_ - (begin_ - step_ + (step_ > 0 ? 1 : -1))) / step_ * step_;
    return {begin_ - step_ + d, step_};
  }

  [[nodiscard]] RangeRevIterator rend() const { // NOLINT
    return {begin_ - step_, step_};
  }
};