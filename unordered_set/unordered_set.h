#pragma once

#include <vector>
#include <list>
#include <stdexcept>
#include <utility>

template <class KeyT>
class UnorderedSet {
 private:
  size_t elements_count_;
  size_t size_;
  std::vector<std::list<KeyT>> table_;

  size_t GetHash(const KeyT& key) const {
    return std::hash<KeyT>{}(key);
  }

 public:
  UnorderedSet() : elements_count_(0), size_(0){};

  explicit UnorderedSet(size_t count) : elements_count_(0), size_(0) {
    table_.resize(count);
  }

  template <class Iterator>
  UnorderedSet(Iterator begin, Iterator end) : elements_count_(0), size_(0) {
    table_.resize(std::distance(begin, end));
    for (auto it = begin; it != end; ++it) {
      Insert(*it);
    }
  }

  UnorderedSet(const UnorderedSet& other)
      : elements_count_(other.elements_count_), size_(other.size_), table_(other.table_) {
  }

  UnorderedSet(UnorderedSet&& other) noexcept
      : elements_count_(std::exchange(other.elements_count_, 0))
      , size_(std::exchange(other.size_, 0))
      , table_(std::move(other.table_)) {
  }

  UnorderedSet& operator=(const UnorderedSet& other) {
    Clear();
    elements_count_ = other.elements_count_;
    size_ = other.size_;
    table_ = other.table_;
    return *this;
  }

  UnorderedSet& operator=(UnorderedSet&& other) noexcept {
    Clear();
    elements_count_ = std::exchange(other.elements_count_, 0);
    size_ = std::exchange(other.size_, 0);
    table_ = std::exchange(other.table_, {});
    return *this;
  }

  [[nodiscard]] size_t Size() const {
    return size_;
  }

  [[nodiscard]] size_t Empty() const {
    return size_ == 0;
  }

  void Clear() {
    elements_count_ = 0;
    size_ = 0;
    table_.clear();
  }

  void Insert(const KeyT& key) {
    if (elements_count_ == BucketCount()) {
      Rehash(BucketCount() ? BucketCount() * 2 : 1);
    }

    if (!Find(key)) {
      ++size_;
    }

    table_[Bucket(key)].push_back(key);
    ++elements_count_;
  }

  void Insert(KeyT&& key) {
    if (elements_count_ == BucketCount()) {
      Rehash(BucketCount() ? BucketCount() * 2 : 1);
    }

    if (!Find(key)) {
      ++size_;
    }

    table_[Bucket(key)].push_back(std::move(key));
    ++elements_count_;
  }

  void Erase(const KeyT& key) {
    std::list<KeyT>& list = table_[Bucket(key)];
    for (auto it = list.begin(); it != list.end(); ++it) {
      if (*it == key) {
        list.erase(it);
        --elements_count_;

        if (!Find(key)) {
          --size_;
        }

        return;
      }
    }
  }

  bool Find(const KeyT& key) const {
    if (Empty()) {
      return false;
    }

    for (auto x : table_[Bucket(key)]) {
      if (x == key) {
        return true;
      }
    }
    return false;
  }

  void Rehash(size_t new_bucket_count) {
    if (new_bucket_count == BucketCount() || new_bucket_count < elements_count_) {
      return;
    }

    size_t old_bucket_count = BucketCount();

    if (new_bucket_count > old_bucket_count) {
      table_.resize(new_bucket_count);
    }

    for (size_t bucket = 0; bucket < old_bucket_count; ++bucket) {
      auto it = table_[bucket].begin();
      while (it != table_[bucket].end()) {
        size_t new_bucket = GetHash(*it) % new_bucket_count;

        if (new_bucket != bucket) {
          auto tmp = it++;
          table_[new_bucket].splice(table_[new_bucket].end(), table_[bucket], tmp);
        } else {
          ++it;
        }
      }
    }

    table_.resize(new_bucket_count);
  }

  void Reserve(size_t new_bucket_count) {
    if (new_bucket_count > BucketCount()) {
      Rehash(new_bucket_count);
    }
  }

  [[nodiscard]] size_t BucketCount() const {
    return table_.size();
  }

  [[nodiscard]] size_t BucketSize(size_t id) const {
    if (id >= table_.size()) {
      return 0;
    }
    return table_[id].size();
  }

  size_t Bucket(const KeyT& key) const {
    return GetHash(key) % BucketCount();
  }

  [[nodiscard]] double LoadFactor() const {
    if (BucketCount() == 0) {
      return 0;
    }
    return static_cast<double>(elements_count_) / BucketCount();
  }
};