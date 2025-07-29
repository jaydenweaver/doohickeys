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
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr int DEFAULT_N = 1000;
constexpr double DEFAULT_P = 0.001;

constexpr double LN_2 = 0.69314718056;
constexpr double LN_2_SQUARED = 0.48045301391;

bool parse_arg(const char *arg, int &n);

int get_bit_array_size(const int n, const double p) {
  return static_cast<int>(std::ceil(-((n * std::log(p)) / LN_2_SQUARED)));
}

int get_num_hashes(const int m, const int n) {
  return static_cast<int>(std::ceil(((double)m / n) * LN_2));
}

int main(int argc, char *argv[]) {
  int n = DEFAULT_N;
  double p = DEFAULT_P;

  if (argc > 1) {
    if (!parse_arg(argv[1], n))
      return EXIT_FAILURE;
  }

  const int bit_array_size = get_bit_array_size(n, p);
  const int num_hashes = get_num_hashes(bit_array_size, n);
  const int buckets = (bit_array_size + 63) / 64;

  std::vector<uint64_t> bits(buckets);

  std::cout << "n: " << n << ", bit array size: " << bit_array_size
            << ", hashes: " << num_hashes << ", buckets: " << buckets
            << std::endl;

  return 0;
}

bool parse_arg(const char *arg, int &n) {
  try {
    n = stoi(std::string(arg));
  } catch (const std::invalid_argument &e) {
    std::cerr << "invalid argument!" << std::endl;
    return false;
  } catch (const std::out_of_range &e) {
    std::cerr << "number for 'n' too large!" << std::endl;
    return false;
  }

  if (n < 1) {
    std::cerr << "'n' must be greater than 0!" << std::endl;
    return false;
  }

  return true;
}
