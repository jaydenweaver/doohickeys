/*

COUNTING BLOOM FILTER IMPLEMENTATION - currently only supports strings
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
#include <stdexcept>
#include <string>
#include <vector>

constexpr double LN_2 = 0.69314718056;
constexpr double LN_2_SQUARED = 0.48045301391;

class CountingBloomFilter {
private:
  int array_size;
  int num_hashes;
  int buckets;
  std::vector<uint64_t> bits;

  void operate(bool remove, const std::string &value);
  // uses a standard double hash
  std::vector<uint64_t> get_hash_indexes(const std::string &value) const;

public:
  CountingBloomFilter(int n, double p);

  void insert(const std::string &value) { operate(false, value); }
  void remove(const std::string &value) { operate(true, value); }
  bool contains(const std::string &value) const;
};

// n must be greater than 0, p must be within range, 0 -> 1
CountingBloomFilter::CountingBloomFilter(int n, double p)
    : array_size(
          static_cast<int>(std::ceil(-((n * std::log(p)) / LN_2_SQUARED)))),
      num_hashes(static_cast<int>(std::ceil(((double)array_size / n) * LN_2))),
      buckets((array_size + 7) / 8), bits(buckets) {
  if (n <= 0 || p <= 0.0 || p >= 1.0) {
    throw std::invalid_argument("invalid input: n must be > 0, 0 < p < 1");
  }
}

std::vector<uint64_t>
CountingBloomFilter::get_hash_indexes(const std::string &value) const {
  std::vector<uint64_t> indexes;
  indexes.reserve(num_hashes);

  uint64_t h1 = std::hash<std::string>{}(value);
  uint64_t h2 = std::hash<std::string>{}("salt" + value);

  for (int i = 0; i < num_hashes; i++) {
    indexes.emplace_back((h1 + i * h2) % array_size);
  }

  return indexes;
}

void CountingBloomFilter::operate(bool remove, const std::string &value) {
  std::vector<uint64_t> indexes = get_hash_indexes(value);
  for (uint64_t index : indexes) {
    int bucket = index / 8;
    int offset = (index % 8) * 8;

    uint8_t count = (bits[bucket] >> offset) & 0xFF;

    if (remove && count > 0)
      count--;
    else if (!remove && count != 255)
      count++;
    else
      continue;

    bits[bucket] &= ~(0xFFULL << offset);
    bits[bucket] |= (static_cast<uint64_t>(count) << offset);
  }
}

bool CountingBloomFilter::contains(const std::string &value) const {
  std::vector<uint64_t> indexes = get_hash_indexes(value);
  for (uint64_t index : indexes) {
    int bucket = index / 8;
    int offset = (index % 8) * 8;

    uint8_t count = (bits[bucket] >> offset) & 0xFF;
    if (count == 0)
      return false;
  }
  return true;
}
