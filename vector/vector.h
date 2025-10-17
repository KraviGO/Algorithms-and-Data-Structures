#define VECTOR_MEMORY_IMPLEMENTED

#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <algorithm>

//===== Исключение: выход за границы вектора =====
class VectorOutOfRange : public std::out_of_range {
public:
    VectorOutOfRange() : std::out_of_range("VectorOutOfRange") {
    }
};

template <class T>
class Vector {
public:
    //===== Псевдонимы типов =====
    using ValueType = T;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;
    using SizeType = std::size_t;
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

private:
    //===== Внутреннее состояние =====
    SizeType size_ = 0;
    SizeType capacity_ = 0;
    Pointer data_ = nullptr;
    std::allocator<T> alloc_;

    //===== Выделение и освобождение памяти =====
    Pointer AllocateStorage(SizeType n) {
        return n == 0 ? nullptr : alloc_.allocate(n);
    }

    void DeallocateStorage(Pointer p) {
        if (p) {
            alloc_.deallocate(p, capacity_);
        }
    }

    //===== Рост вместимости буфера =====
    SizeType GrowthFactor() const noexcept {
        return capacity_ == 0 ? 1 : capacity_ * 2;
    }

    //===== Перевыделение памяти с добавлением элемента =====
    template <class F>
    void ReallocateAndInsert(SizeType new_capacity, F&& placement_logic) {
        Pointer new_data = AllocateStorage(new_capacity);
        try {
            std::uninitialized_move(data_, data_ + size_, new_data);
            placement_logic(new_data + size_);
        } catch (...) {
            std::destroy(new_data, new_data + size_);
            DeallocateStorage(new_data);
            throw;
        }
        std::destroy(data_, data_ + size_);
        DeallocateStorage(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    }

    //===== Универсальный конструктор =====
    template <class F>
    void ConstructWith(SizeType count, F&& constructor_logic) {
        size_ = capacity_ = count;
        data_ = AllocateStorage(count);
        try {
            constructor_logic(data_, count);
        } catch (...) {
            DeallocateStorage(data_);
            throw;
        }
    }

public:
    //===== Конструкторы =====
    Vector() = default;

    explicit Vector(SizeType size) {
        ConstructWith(size, [](Pointer p, SizeType n) {
            std::uninitialized_default_construct_n(p, n);
        });
    }

    Vector(SizeType size, const T& value) {
        ConstructWith(size, [&](Pointer p, SizeType n) {
            std::uninitialized_fill_n(p, n, value);
        });
    }

    template <class Iterator,
            class = std::enable_if_t<
                    std::is_base_of_v<
                            std::forward_iterator_tag,
                            typename std::iterator_traits<Iterator>::iterator_category
                    >
            >
    >
    Vector(Iterator first, Iterator last) {
        ConstructWith(std::distance(first, last), [&](Pointer p, SizeType) {
            std::uninitialized_copy(first, last, p);
        });
    }

    Vector(std::initializer_list<T> init) {
        ConstructWith(init.size(), [&](Pointer p, SizeType) {
            std::uninitialized_copy(init.begin(), init.end(), p);
        });
    }

    Vector(const Vector& other) {
        ConstructWith(other.size_, [&](Pointer p, SizeType) {
            std::uninitialized_copy(other.data_, other.data_ + other.size_, p);
        });
    }

    Vector(Vector&& other) noexcept : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
        other.size_ = other.capacity_ = 0;
        other.data_ = nullptr;
    }

    //===== Операторы присваивания =====
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector tmp(other);
            Swap(tmp);
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            Vector temp(std::move(other));
            Swap(temp);
        }
        return *this;
    }

    //===== Деструктор =====
    ~Vector() {
        Clear();
        DeallocateStorage(data_);
    }

    //===== Доступ к элементам =====
    ConstReference operator[](SizeType id) const {
        return data_[id];
    }

    Reference operator[](SizeType id) {
        return data_[id];
    }

    ConstReference At(SizeType id) const {
        if (id >= size_) {
            throw VectorOutOfRange{};
        }
        return data_[id];
    }

    Reference At(SizeType id) {
        if (id >= size_) {
            throw VectorOutOfRange{};
        }
        return data_[id];
    }

    ConstReference Front() const {
        return data_[0];
    }

    Reference Front() {
        return data_[0];
    }

    ConstReference Back() const {
        return data_[size_ - 1];
    }

    Reference Back() {
        return data_[size_ - 1];
    }

    ConstPointer Data() const {
        return data_;
    }

    Pointer Data() {
        return data_;
    }

    //===== Различные методы =====
    SizeType Size() const {
        return size_;
    }

    SizeType Capacity() const {
        return capacity_;
    }

    bool Empty() const {
        return size_ == 0;
    }

    void Swap(Vector& other) {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(data_, other.data_);
    }

    void Reserve(SizeType new_cap) {
        if (new_cap > capacity_) {
            ReallocateAndInsert(new_cap, [](Pointer) {});
        }
    }

    void ShrinkToFit() {
        if (size_ == 0) {
            DeallocateStorage(data_);
            capacity_ = 0;
            data_ = nullptr;
        }
        else if (size_ < capacity_) {
            ReallocateAndInsert(size_, [](Pointer) {});
        }
    }

    void Clear() noexcept {
        if (data_ != nullptr) {
            std::destroy(data_, data_ + size_);
            size_ = 0;
        }
    }

    void Resize(SizeType new_size) {
        if (new_size > capacity_) {
            ReallocateAndInsert(new_size, [&](Pointer p) {
                std::uninitialized_default_construct_n(p, new_size - size_);
            });
        }
        else if (new_size < size_) {
            std::destroy(data_ + new_size, data_ + size_);
        }
        else {
            std::uninitialized_default_construct_n(data_ + size_, new_size - size_);
        }
        size_ = new_size;
    }

    void Resize(SizeType new_size, const T& value) {
        if (new_size > capacity_) {
            ReallocateAndInsert(new_size, [&](Pointer p) {
                std::uninitialized_fill_n(p, new_size - size_, value);
            });
        }
        else if (new_size < size_) {
            std::destroy(data_ + new_size, data_ + size_);
        }
        else {
            std::uninitialized_fill_n(data_ + size_, new_size - size_, value);
        }
        size_ = new_size;
    }

    void PushBack(const T& value) {
        if (size_ == capacity_) {
            ReallocateAndInsert(GrowthFactor(), [&](Pointer p) {
                new (p) T(value);
            });
        }
        else {
            new (data_ + size_) T(value);
        }
        ++size_;
    }

    void PushBack(T&& value) {
        if (size_ == capacity_) {
            ReallocateAndInsert(GrowthFactor(), [&](Pointer p) {
                new (p) T(std::move(value));
            });
        }
        else {
            new (data_ + size_) T(std::move(value));
        }
        ++size_;
    }

    template <class... Args>
    void EmplaceBack(Args&&... args) {
        if (size_ == capacity_) {
            ReallocateAndInsert(GrowthFactor(), [&](Pointer p) {
                new (p) T(std::forward<Args>(args)...);
            });
        }
        else {
            new (data_ + size_) T(std::forward<Args>(args)...);
        }
        ++size_;
    }

    void PopBack() {
        if (size_ > 0) {
            std::destroy_at(data_ + size_ - 1);
            --size_;
        }
    }

    //===== Итераторы =====
    Iterator begin() noexcept { // NOLINT
        return data_;
    }

    ConstIterator begin() const noexcept { // NOLINT
        return data_;
    }

    ConstIterator cbegin() const noexcept { // NOLINT
        return data_;
    }

    Iterator end() noexcept { // NOLINT
        return data_ + size_;
    }

    ConstIterator end() const noexcept { // NOLINT
        return data_ + size_;
    }

    ConstIterator cend() const noexcept { // NOLINT
        return data_ + size_;
    }

    ReverseIterator rbegin() noexcept { // NOLINT
        return ReverseIterator(end());
    }

    ConstReverseIterator rbegin() const noexcept { // NOLINT
        return ConstReverseIterator(end());
    }

    ConstReverseIterator crbegin() const noexcept { // NOLINT
        return ConstReverseIterator(cend());
    }

    ReverseIterator rend() noexcept { // NOLINT
        return ReverseIterator(begin());
    }

    ConstReverseIterator rend() const noexcept { // NOLINT
        return ConstReverseIterator(begin());
    }

    ConstReverseIterator crend() const noexcept { // NOLINT
        return ConstReverseIterator(cbegin());
    }

    //===== Сравнение =====
    friend bool operator==(const Vector& a, const Vector& b) {
        return a.size_ == b.size_ && std::equal(a.begin(), a.end(), b.begin());
    }

    friend bool operator!=(const Vector& a, const Vector& b) {
        return !(a == b); // NOLINT
    }

    friend bool operator<(const Vector& a, const Vector& b) {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
    }

    friend bool operator<=(const Vector& a, const Vector& b) {
        return !(b < a); // NOLINT
    }

    friend bool operator>(const Vector& a, const Vector& b) {
        return b < a;
    }

    friend bool operator>=(const Vector& a, const Vector& b) {
        return !(a < b); // NOLINT
    }
};