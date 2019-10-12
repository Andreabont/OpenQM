#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Implicant"
#include "../string_implicant.h"
#include "../qm.h"

#include <vector>
#include <set>
#include <algorithm>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( expand_parentheses_1 ) {
    std::vector<std::vector<std::set<int>>> v1 = {
        {{6}, {1}},
        {{1}, {2}},
        {{2}, {3}},
        {{3}, {4}},
        {{4}, {5}},
        {{5}, {6}}
    };
    std::vector<std::set<int>> correct = {
        {1, 3, 5},
        {2, 4, 6},
        {1, 2, 4, 5},
        {2, 3, 5, 6},
        {1, 3, 4, 6}
    };
    auto res = expand_parentheses(v1);

    std::sort(res.begin(), res.end());
    std::sort(correct.begin(), correct.end());

    BOOST_CHECK(res == correct);
}
