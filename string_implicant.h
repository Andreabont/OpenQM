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

#ifndef STRING_IMPLICANT_H
#define STRING_IMPLICANT_H

#include <string>
#include <vector>
#include <set>
#include <cmath>

class StringImplicant {
    std::string implicant_str;
    void init(const std::string& other);
public:
    StringImplicant(const std::string &str);
    StringImplicant(const char *other_str);
    StringImplicant& operator=(const  std::string& other);
    StringImplicant& operator=(const StringImplicant& other);
    StringImplicant operator*(const StringImplicant& other);
    bool operator!=(const StringImplicant& other) const;
    bool operator<(const StringImplicant& other) const;
    bool trivial() const;
    std::string getStr() const;
    int num_ones() const;
    int rank() const;
    friend bool operator==(const StringImplicant&, const StringImplicant&);
    bool operator()(const StringImplicant &x) const;
    size_t size() const;
};

std::ostream &operator<<(std::ostream &os, const StringImplicant& other);

#endif // STRING_IMPLICANT_H
