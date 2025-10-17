#define WEAK_PTR_IMPLEMENTED

#pragma once

#include <utility>
#include <stdexcept>

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

class IncorrectSharedPtrDereference : public std::runtime_error {
public:
  IncorrectSharedPtrDereference() : std::runtime_error("Attempt to dereference empty SharedPtr") {}
};

class BadWeakPtr : public std::runtime_error {
public:
  BadWeakPtr() : std::runtime_error("BadWeakPtr") {}
};

template <class T>
struct Counter {
  T* ptr_;
  size_t strong_count_;
  size_t weak_count_;

  explicit Counter(T* ptr) : ptr_(ptr), strong_count_(1), weak_count_(0) {}

  ~Counter() {
    delete ptr_;
  }
};

template <class T>
class SharedPtr {
private:
  Counter<T>* counter_ = nullptr;

  friend class WeakPtr<T>;

public:
  SharedPtr() = default;

  explicit SharedPtr(T* ptr) {
    if (ptr) {
      counter_ = new Counter<T>(ptr);
    }
  }

  SharedPtr(const SharedPtr& other) {
    if (other.counter_) {
      counter_ = other.counter_;
      ++counter_->strong_count_;
    }
  }

  SharedPtr(SharedPtr&& other) noexcept : counter_(std::exchange(other.counter_, nullptr)) {}

  explicit SharedPtr(const WeakPtr<T>& weak) {
    if (!weak.counter_ || weak.counter_->strong_count_ == 0) {
      throw BadWeakPtr();
    }
    counter_ = weak.counter_;
    ++counter_->strong_count_;
  }

  SharedPtr& operator=(const SharedPtr& other) {
    if (this != &other) {
      Reset();

      counter_ = other.counter_;

      if (counter_) {
        ++counter_->strong_count_;
      }
    }

    return *this;
  }

  SharedPtr& operator=(SharedPtr&& other) noexcept {
    if (this != &other) {
      Reset();
      counter_ = std::exchange(other.counter_, nullptr);
    }

    return *this;
  }

  ~SharedPtr() {
    Reset();
  }

  void Reset(T* ptr = nullptr) {
    if (counter_) {
      --counter_->strong_count_;
      if (counter_->strong_count_ == 0) {
        delete std::exchange(counter_->ptr_, nullptr);
        if (counter_->weak_count_ == 0) {
          delete counter_;
        }
      }
      counter_ = nullptr;
    }

    if (ptr) {
      counter_ = new Counter<T>(ptr);
    }
  }


  void Swap(SharedPtr& other) {
    std::swap(counter_, other.counter_);
  }

  T* Get() const {
    if (!counter_) {
      return nullptr;
    }
    return counter_->ptr_;
  }

  [[nodiscard]] size_t UseCount() const {
    return counter_ ? counter_->strong_count_ : 0;
  }

  T& operator*() const {
    if (!counter_) {
      throw IncorrectSharedPtrDereference{};
    }
    return *(counter_->ptr_);
  }

  T* operator->() const {
    if (!counter_) {
      throw IncorrectSharedPtrDereference{};
    }
    return counter_->ptr_;
  }

  explicit operator bool() const {
    return counter_ && (counter_->ptr_ != nullptr);
  }
};

template <class T>
class WeakPtr {
private:
  Counter<T>* counter_ = nullptr;

  friend class SharedPtr<T>;

public:
  WeakPtr() = default;

  WeakPtr(const WeakPtr& other) {
    counter_ = other.counter_;
    if (counter_) {
      ++counter_->weak_count_;
    }
  }

  WeakPtr(WeakPtr&& other) noexcept : counter_(std::exchange(other.counter_, nullptr)) {}

  WeakPtr(const SharedPtr<T>& shared) { // NOLINT
    counter_ = shared.counter_;
    if (counter_) {
      ++counter_->weak_count_;
    }
  }

  WeakPtr& operator=(const WeakPtr& other) {
    if (this != &other) {
      Reset();

      counter_ = other.counter_;

      if (counter_) {
        ++counter_->weak_count_;
      }
    }

    return *this;
  }

  WeakPtr& operator=(WeakPtr&& other) noexcept {
    if (this != &other) {
      Reset();
      counter_ = std::exchange(other.counter_, nullptr);
    }

    return *this;
  }

  ~WeakPtr() {
    Reset();
  }

  void Reset() {
    if (counter_) {
      --counter_->weak_count_;
      if (counter_->strong_count_ == 0 && counter_->weak_count_ == 0) {
        delete counter_;
      }
      counter_ = nullptr;
    }
  }


  void Swap(WeakPtr& other) {
    std::swap(counter_, other.counter_);
  }

  [[nodiscard]] size_t UseCount() const {
    return counter_ ? counter_->strong_count_ : 0;
  }

  [[nodiscard]] bool Expired() const {
    return !counter_ || counter_->strong_count_ == 0;
  }

  SharedPtr<T> Lock() const {
    return Expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
  }
};

template <class T, class... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return SharedPtr<T>(new T(std::forward<Args>(args)...));
}