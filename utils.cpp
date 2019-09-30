#include "utils.h"

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
