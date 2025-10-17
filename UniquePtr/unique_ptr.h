#pragma once

#include <utility>

template <typename T>
class UniquePtr {
private:
  T* ptr_;

public:
  UniquePtr() : ptr_(nullptr) {}
  explicit UniquePtr(T* ptr) : ptr_(ptr) {}

  UniquePtr(const UniquePtr&) = delete;
  UniquePtr& operator=(const UniquePtr&) = delete;

  ~UniquePtr() {
    delete ptr_;
  }

  UniquePtr(UniquePtr&& other) noexcept : ptr_(std::exchange(other.ptr_, nullptr)) {}

  UniquePtr& operator=(UniquePtr&& other) noexcept {
    if (this != &other) {
      delete ptr_;
      ptr_ = std::exchange(other.ptr_, nullptr);
    }
    return *this;
  }

  T* Release() {
    T* old_ptr = std::exchange(ptr_, nullptr);
    return old_ptr;
  }

  void Reset(T* new_ptr = nullptr) {
    if (ptr_ != new_ptr) {
      delete ptr_;
      ptr_ = new_ptr;
    }
  }

  void Swap(UniquePtr& other) {
    std::swap(ptr_, other.ptr_);
  }

  T* Get() const {
    return ptr_;
  }

  T& operator*() const {
    return *ptr_;
  }

  T* operator->() const {
    return ptr_;
  }

  explicit operator bool() const {
    return ptr_ != nullptr;
  }
};
