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

#include <cmath>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

constexpr double LN_2 = 0.69314718056;
constexpr double LN_2_SQUARED = 0.48045301391;

class BloomFilter {
private:
  int bit_array_size;
  int num_hashes;
  int buckets;
  std::vector<uint64_t> bits;

  // uses a standard double hash
  std::vector<uint64_t> get_hash_indexes(const std::string &value) const;

public:
  BloomFilter(int n, double p);

  void insert(const std::string &value);
  bool contains(const std::string &value) const;
};

// n must be greater than 0, p must be within range, 0 -> 1
BloomFilter::BloomFilter(int n, double p)
    : bit_array_size(
          static_cast<int>(std::ceil(-((n * std::log(p)) / LN_2_SQUARED)))),
      num_hashes(
          static_cast<int>(std::ceil(((double)bit_array_size / n) * LN_2))),
      buckets((bit_array_size + 63) / 64), bits(buckets) {}

std::vector<uint64_t>
BloomFilter::get_hash_indexes(const std::string &value) const {
  std::vector<uint64_t> indexes;
  indexes.reserve(num_hashes);

  uint64_t h1 = std::hash<std::string>{}(value);
  uint64_t h2 = std::hash<std::string>{}("salt" + value);

  for (int i = 0; i < num_hashes; i++) {
    uint64_t index = (h1 + i * h2) % bit_array_size;
    indexes.push_back(index);
  }

  return indexes;
}

void BloomFilter::insert(const std::string &value) {
  std::vector<uint64_t> indexes = get_hash_indexes(value);
  for (uint64_t index : indexes) {
    int bucket = index / 64;
    int index_in_bucket = index % 64;
    bits[bucket] |= (1ULL << index_in_bucket);
  }
}

bool BloomFilter::contains(const std::string &value) const {
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
