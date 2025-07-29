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

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#define DEFAULT_N 1000
#define DEFAULT_P 0.001

bool parse_arg(const char *arg, int &n);

int main(int argc, char *argv[]) {
  std::vector<uint64_t> bits;
  int n = DEFAULT_N;

  if (argc > 1) {
    if (!parse_arg(argv[1], n))
      return 0;
  }
  std::cout << "n: " << n << std::endl;
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

  if (n < 0) {
    std::cerr << "'n' cannot be negative!" << std::endl;
    return false;
  }

  return true;
}
