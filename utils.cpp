#include "utils.h"

#include <string>

std::uintmax_t
binpow(std::uintmax_t a, std::uintmax_t n)
{
    uintmax_t res = 1;
    while (n != 0) {
        if (n % 2 == 1)
            res *= a;
        a *= a;
        n /= 2;
    }
    return res;
}

std::string
binary_repr(std::uintmax_t x, std::uintmax_t num_bin_digits)
{
    std::string s(num_bin_digits, '0');
    uintmax_t i = 0;
    while (x != 0) {
        if (x % 2 == 1)
            s[num_bin_digits - 1 - i] = '1';
        i++;
        x /= 2;
    }
    return s;
}
