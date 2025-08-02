/*

BLOOM FILTER IMPLEMENTATION
---------------------------------------------------------

optimal bit array size is calculated via the following:
  m = (-n * ln(p)) / (ln(2))^2

where:
  m = bit array size
  n = input size
  p = false positive probability

NOTE:
  p = 0.1 = ~4.8 bits per element
  p = 0.01 = ~9.6 bits per element
  p = 0.001 = ~14.4 bits per element
  p = 0.0001 = ~19.2 bits per element

optimal number of hash functions is calculated via the following:
  k = (m / n) * ln(2)
  where k is the number of hash functions

*/

#pragma once
#include <cmath>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <vector>

constexpr double LN_2 = 0.69314718056;
constexpr double LN_2_SQUARED = 0.48045301391;

template <typename T> class BloomFilter {
  static_assert(std::is_default_constructible_v<std::hash<T>>,
                "type T must be hashable (std::hash<T> must be defined)");

private:
  int bit_array_size;
  int num_hashes;
  int buckets;
  std::vector<uint64_t> bits;

  // uses a standard double hash
  std::vector<uint64_t> get_hash_indexes(const T &value) const;

public:
  BloomFilter(int n, double p);

  void insert(const T &value);
  bool contains(const T &value) const;
};

// n must be greater than 0, p must be within range, 0 -> 1
template <typename T>
BloomFilter<T>::BloomFilter(int n, double p)
    : bit_array_size(
          static_cast<int>(std::ceil(-((n * std::log(p)) / LN_2_SQUARED)))),
      num_hashes(
          static_cast<int>(std::ceil(((double)bit_array_size / n) * LN_2))),
      buckets((bit_array_size + 63) / 64), bits(buckets) {
  if (n <= 0 || p <= 0.0 || p >= 1.0) {
    throw std::invalid_argument("invalid input: n must be > 0, 0 < p < 1");
  }
}

template <typename T>
std::vector<uint64_t> BloomFilter<T>::get_hash_indexes(const T &value) const {
  std::vector<uint64_t> indexes;
  indexes.reserve(num_hashes);

  uint64_t h1 = std::hash<T>{}(value);
  uint64_t h2 = std::hash<std::pair<T, int>>{}({value, 123});

  for (int i = 0; i < num_hashes; i++) {
    indexes.emplace_back((h1 + i * h2) % bit_array_size);
  }

  return indexes;
}

template <typename T> void BloomFilter<T>::insert(const T &value) {
  std::vector<uint64_t> indexes = get_hash_indexes(value);
  for (uint64_t index : indexes) {
    int bucket = index / 64;
    int index_in_bucket = index % 64;
    bits[bucket] |= (1ULL << index_in_bucket);
  }
}

template <typename T> bool BloomFilter<T>::contains(const T &value) const {
  std::vector<uint64_t> indexes = get_hash_indexes(value);
  for (uint64_t index : indexes) {
    int bucket = index / 64;
    int index_in_bucket = index % 64;

    if ((bits[bucket] & (1ULL << index_in_bucket)) == 0) {
      return false;
    }
  }
  return true;
}
