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

#include <stdexcept>
#include <algorithm>
#include <cstdlib>
#include <sstream>

#include "string_implicant.h"
#include "utils.h"

StringImplicant::StringImplicant(const std::string& str)
{
    if (str.find_first_not_of("01-") != std::string::npos) {
        throw std::logic_error("'" + str + "' isn't an implicant!");
    }
    implicant_str = str;
}

size_t
StringImplicant::size() const
{
    return implicant_str.size();
}

int
StringImplicant::rank() const
{
    int res = 0;
    for (char ch : implicant_str)
        res += (ch != '-');
    return res;
}

bool
StringImplicant::operator()(const StringImplicant &x) const
{
    if (x.size() != size()) {
        throw std::runtime_error("size mismatch");
    }
    for (size_t i = 0; i < x.size(); i++) {
        if (implicant_str[i] != x.implicant_str[i] && implicant_str[i] != '-')
            return false;
    }
    return true;
}

StringImplicant&
StringImplicant::operator=(const StringImplicant& other) {
    this->implicant_str = other.implicant_str;
    return *this;
}

StringImplicant& StringImplicant::operator=(const std::string& other) {
    this->implicant_str = other;
    return *this;
}

StringImplicant StringImplicant::operator*(const StringImplicant& other) {

    StringImplicant result(std::string(""));

    if (this->implicant_str.size() != other.implicant_str.size()) {
        throw std::logic_error("Can't reduce '"+other.implicant_str+"' and '"+this->implicant_str+"', they have different size!");
    }
    
    if (abs(this->num_ones() - other.num_ones()) > 1) {
        return result;
    }

    std::string new_implicant;
    
    bool found_different = false;

    for (int i = 0; i < this->implicant_str.size(); ++i) {
        if(this->implicant_str[i] == other.implicant_str[i]) {
            new_implicant += this->implicant_str[i];
        } else {
            if (!found_different && !(this->implicant_str[i] == '-' || other.implicant_str[i] == '-')) {
                found_different = true;
                new_implicant += '-';
            } else {
                return result;
            }
        }
    }
    
    result.implicant_str = new_implicant;

    return result;
}


/**
 * @brief Equality operator
 * 
 * @param other Implicant&
 * @return bool
 */
bool
operator==(const StringImplicant &l, const StringImplicant& r)
{
    return l.implicant_str == r.implicant_str;
}


bool StringImplicant::operator!=(const StringImplicant& other) const {
    return this->implicant_str != other.implicant_str;
}

bool StringImplicant::operator<(const StringImplicant& other) const {
    return this->implicant_str < other.implicant_str;
}

/**
 * @brief Return the numbers of '1' in the implicant
 * 
 * @return int
 */
int StringImplicant::num_ones() const {
    return std::count(this->implicant_str.begin(), this->implicant_str.end(), '0');
}

bool
StringImplicant::trivial() const
{
    return implicant_str == "";
}

std::string
StringImplicant::getStr() const
{
    return this->implicant_str;
}

std::ostream
&operator<<(std::ostream &os, const StringImplicant& other)
{
    return os << other.getStr();
}
