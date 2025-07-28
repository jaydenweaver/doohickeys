#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  std::vector<uint64_t> bits;
  int n = 1000;

  if (argc > 1) {
    try {
      n = stoi(std::string(argv[1]));
    } catch (const std::invalid_argument &e) {
      std::cerr << "invalid argument!" << std::endl;
      return 0;
    } catch (const std::out_of_range &e) {
      std::cerr << "number for 'n' too large!" << std::endl;
      return 0;
    }
  }
  std::cout << "n: " << n << std::endl;
  return 0;
}
