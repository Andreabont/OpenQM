#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Implicant"
#include "../implicant.h"
#include <boost/test/unit_test.hpp> //VERY IMPORTANT - include this last

BOOST_AUTO_TEST_CASE( implicant_coverage_a ) {
    Implicant a("010");
    Implicant b("010", {2});
    BOOST_CHECK(a == b);
}

BOOST_AUTO_TEST_CASE( implicant_coverage_b ) {
    Implicant a("01-");
    Implicant b("01-", {2,3});
    BOOST_CHECK(a == b);
}

BOOST_AUTO_TEST_CASE( implicant_reduce ) {
    Implicant a("010");
    Implicant b("110");
    Implicant c("-10", {2,6});
    Implicant d = a + b;
    BOOST_CHECK(c == d);
}