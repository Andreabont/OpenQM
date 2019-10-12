/**
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 *
 * Name        :  OpenQM
 * Author      :  Andrea Bontempi
 * Description :  Logic Network Synthesizer
 *
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 *
 * This file is part of OpenQM.
 *
 *  OpenQM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenQM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this project.  If not, see <http://www.gnu.org/licenses/>.
 *
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 */

#ifndef LIBQM_H
#define LIBQM_H

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>

#include "implicant.h"
#include "string_implicant.h"

template<typename ImplicantType>
void
qm_step(std::vector<ImplicantType> &prime,
        std::vector<ImplicantType> &yet_unknown_if_prime,
        std::vector<ImplicantType> &next_step)
{
    // make an alias for yet_unknown_if_prime (it is too long to type
    // but just 'unknown' does not make sense as a parameter name to me
    std::vector<ImplicantType> &unknown = yet_unknown_if_prime;

    // combination and reduction of implicants
    std::vector<int> was_glued(unknown.size(), false);
    for (uintmax_t i = 0; i < unknown.size(); i++) {
        for (uintmax_t j = i + 1; j < unknown.size(); j++) {
            ImplicantType temp = unknown[i] * unknown[j];
            if (temp.trivial())
                continue;
            was_glued[i] = true;
            was_glued[j] = true;
            next_step.push_back(temp);
        }
    }

    // Result must me unique
    std::sort(next_step.begin(), next_step.end());
    next_step.erase(std::unique(next_step.begin(), next_step.end()), next_step.end());

    // Compile exclude list
    for (uintmax_t i = 0; i < unknown.size(); ++i) {
        if (!was_glued[i]) {
            prime.push_back(unknown[i]);
        }
    }
}

void
qm_step(std::vector<StringImplicant> &prime,
        std::vector<StringImplicant> &yet_unknown_if_prime,
        std::vector<StringImplicant> &next_step)
{
    // make an alias for yet_unknown_if_prime (it is too long to type
    // but just 'unknown' does not make sense as a parameter name to me
    std::vector<StringImplicant> &unknown = yet_unknown_if_prime;

    // this StringImplicant specialization of qm_step makes use of
    // the following 'ones count' optimization.
    // group[i] will contain all implicants with exatcly i ones.
    // due to fact that if two implicants can be reduced than number of ones
    // in them differ by 1, we can only do glue attemts for pairs of
    // implicants from cosequtive groups.
    int max_num_ones = 0;
    std::map<int, std::vector<int>> group;
    for (int i = 0; i < unknown.size(); ++i) {
        int num_ones = yet_unknown_if_prime[i].num_ones();
        group[num_ones].push_back(i);
        max_num_ones = std::max(num_ones, max_num_ones);
    }

    // combination and reduction of implicants
    std::vector<int> was_glued(unknown.size(), false);
    for (int cur_num_ones = 0; cur_num_ones < max_num_ones; ++cur_num_ones) {
        for (auto x : group[cur_num_ones]) {
            for (auto y : group[cur_num_ones + 1]) {
                StringImplicant temp = unknown[x] * unknown[y];
                if (temp.trivial())
                    continue;
                was_glued[x] = true;
                was_glued[y] = true;
                next_step.push_back(temp);
            }
        }
    }

    // Result must me unique
    std::sort(next_step.begin(), next_step.end());
    next_step.erase(std::unique(next_step.begin(), next_step.end()), next_step.end());

    // Compile exclude list
    for (auto i = 0; i < unknown.size(); ++i) {
        if (!was_glued[i]) {
            prime.push_back(unknown[i]);
        }
    }
}

template<typename ImplicantType>
std::vector<ImplicantType>
make_qm(const std::vector<ImplicantType> &implicants)
{
    std::vector<ImplicantType> prime;
    std::vector<ImplicantType> yet_unknown_if_prime = implicants;
    std::vector<ImplicantType> next_step;

    do {
        qm_step(prime, yet_unknown_if_prime, next_step);
        std::swap(next_step, yet_unknown_if_prime);
        next_step.clear();
    } while (yet_unknown_if_prime.size() != 0);

    return prime;
}

template<typename T>
bool
subset(const std::set<T> &cats, const std::set<T> &animals)
{
    for (const T &cat : cats)
        if (animals.count(cat) == 0)
            return false;
    return true;
}

std::vector<std::set<int>>
expand_parentheses(const std::vector<std::set<int>> &lv, const std::vector<std::set<int>> &rv)
{
    std::set<std::set<int>> temp;
    for (auto &r : rv) {
        for (auto &l : lv) {
            std::set<int> s = l;
            s.insert(r.begin(), r.end());
            temp.insert(s);
        }
    }
    std::vector<std::set<int>> temp_vec;
    temp_vec.reserve(temp.size());
    std::copy(temp.begin(), temp.end(), std::back_inserter(temp_vec));
    std::vector<int> reduced(temp_vec.size(), 0);
    for (size_t i = 0; i < temp_vec.size(); i++) {
        for (size_t j = i + 1; j < temp_vec.size(); j++) {
            if (subset(temp_vec[i], temp_vec[j]))
                reduced[j] = 1;
            else if (subset(temp_vec[j], temp_vec[i]))
                reduced[i] = 1;
        }
    }
    std::vector<std::set<int>> res;
    res.reserve(temp_vec.size());
    for (size_t i = 0; i < temp_vec.size(); i++) {
        if (!reduced[i])
            res.push_back(temp_vec[i]);
    }
    res.shrink_to_fit();
    return res;
}

std::vector<std::set<int>>
expand_parentheses(const std::vector<std::vector<std::set<int>>> &v)
{
    std::vector<std::set<int>> res;
    if (v.empty())
        return res;
    res = v[0];
    for (size_t i = 1; i < v.size(); i++)
        res = expand_parentheses(res, v[i]);
    return res;
}

template<typename ImplicantTypePerfect, typename ImplicantTypeReduced>
std::vector<std::set<int>>
get_deadends(const std::vector<ImplicantTypePerfect> &perfect_dnf,
             const std::vector<ImplicantTypeReduced> &reduced_dnf)
{
    std::vector<std::vector<std::set<int>>> covering_func;
    covering_func.reserve(perfect_dnf.size());
    for (const ImplicantTypePerfect &val : perfect_dnf) {
        std::vector<std::set<int>> one_on_val;
        for (size_t i = 0; i < reduced_dnf.size(); i++)
            if (reduced_dnf[i](val))
                one_on_val.push_back(std::set<int> {static_cast<int>(i)});
        covering_func.push_back(one_on_val);
    }

    return expand_parentheses(covering_func);
}

template<typename ImplicantTypePerfect, typename ImplicantTypeReduced>
std::vector<int>
get_kernels(const std::vector<ImplicantTypePerfect> &perfect_dnf,
           const std::vector<ImplicantTypeReduced> &reduced_dnf)
{
    std::vector<int> res(reduced_dnf.size(), 0);
    for (const ImplicantTypePerfect &val : perfect_dnf) {
        bool found = false;
        bool only = true;
        size_t ind = 0;
        for (size_t i = 0; i < reduced_dnf.size(); i++) {
            if (reduced_dnf[i](val)) {
                if (!found) {
                    found = true;
                    ind = i;
                } else {
                    only = false;
                    break;
                }
            }
        }
        if (only)
            res[ind] = 1;
    }
    return res;
}

#endif // LIBQM_H
