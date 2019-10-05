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

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>

#include "qm.h"
#include "implicant.h"

using std::vector;

void
qm_step(vector<Implicant> &prime, vector<Implicant> &yet_unknown_if_prime,
        vector<Implicant> &next_step)
{
    // make an alias for yet_unknown_if_prime (it is too long to type
    // but just 'unknown' does not make sense as a parameter name to me
    vector<Implicant> &unknown = yet_unknown_if_prime;

    // group[i] will contain all implicants with exatcly i ones.
    // due to if two implicants can be reduced than number of ones
    // in them differ by 1, we can only do glue attemts for pairs of
    // implicants from cosequtive groups.
    int max_num_ones = 0;
    std::map<int, vector<int>> group;
    for (int i = 0; i < unknown.size(); ++i) {
        int num_ones = yet_unknown_if_prime[i].num_ones();
        group[num_ones].push_back(i);
        max_num_ones = std::max(num_ones, max_num_ones);
    }

    // combination and reduction of implicants
    vector<int> was_glued(unknown.size(), false);
    for (int cur_num_ones = 0; cur_num_ones < max_num_ones; ++cur_num_ones) {
        for (auto x : group[cur_num_ones]) {
            for (auto y : group[cur_num_ones + 1]) {
                Implicant temp = unknown[x] + unknown[y];
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

vector<Implicant>
make_qm(const vector<Implicant>& implicants)
{
    vector<Implicant> prime;
    vector<Implicant> yet_unknown_if_prime = implicants;
    vector<Implicant> next_step;
    do {
        qm_step(prime, yet_unknown_if_prime, next_step);
        std::swap(next_step, yet_unknown_if_prime);
        next_step.clear();
    } while (yet_unknown_if_prime.size() != 0);

    return prime;
}
