#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <string>

std::string
binary_repr(std::uintmax_t x, std::uintmax_t num_bin_digits);

std::uintmax_t
binpow(std::uintmax_t a, std::uintmax_t n);

#endif // UTILS_H
