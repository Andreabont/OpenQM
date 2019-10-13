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
#include <cmath>
#include "implicant.h"

/**
 * @brief Real Constructor
 * 
 * @param other Implicant formatted as string
 * @param covr Coverage of implicant (optional)
 * @return void
 */
void Implicant::init(const std::string& other, const std::set<int>& covr) {
    
    if(other.find_first_not_of("01-") != std::string::npos) {
        throw std::logic_error("'"+other+"' isn't an implicant!");
    }

    this->implicant_str = other;
    this->coverage = covr;
    
    if(covr.size() == 0) {
        this->generateCoverage();
    }

}

/**
 * @brief Generate coverage for new implicant.
 * 
 * @return void
 */
void Implicant::generateCoverage() {
    
    if(this->implicant_str.find_first_of("-") != std::string::npos) {
        // Implicant is a reduction of other implicants
        for(Implicant other : this->getExplodedList()) {
            auto other_cov = other.getCoverage();
            std::set_union(this->coverage.begin(), this->coverage.end(), other_cov.begin(), other_cov.end(), std::inserter(this->coverage, this->coverage.begin()));
        }
        return;
    }    
    
    int index = 0;
    int result = 0;
    for(auto i = --this->implicant_str.end(); i >= this->implicant_str.begin(); i--) {
        if(*i == '1') {
            result += std::pow(2,index);
        }
        index++;
    }
    this->coverage = {result};
}

/**
 * @brief Generate list of all base implicants (recursive)
 *        The algorithm use recursion for generate the tree of all possibile implicants.
 * @param pos Cursor that indicate the current bit (internal use, don't touch)
 * @param step Buffer that save the implicant generated in a branch
 * @return std::vector< Implicant, std::allocator< void > >
 */
std::vector<Implicant> Implicant::getExplodedList(int pos, std::string step) {
    
    if(pos > this->implicant_str.size()-1) {
        std::vector<Implicant> ret = {step};
        return ret;
    }
    
    if(this->implicant_str[pos] == '-') {
        std::vector<Implicant> a = this->getExplodedList(pos+1, step+'0');
        std::vector<Implicant> b = this->getExplodedList(pos+1, step+'1');
        std::vector<Implicant> ret;
        std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(ret, ret.begin()));
        return ret;
    } else {
        return this->getExplodedList(pos+1, step+this->implicant_str[pos]);
    }
    
}

/**
 * @brief Constructor from std::string
 * 
 * @param other String
 * @param covr Coverage
 */
Implicant::Implicant(const std::string& other, const std::set<int>& covr) {
    if(other != "") init(other, covr);
}

/**
 * @brief Constructor from const char*
 * 
 * @param other_str const char*
 * @param covr Coverage
 */
Implicant::Implicant(const char* other_str, const std::set<int>&  covr) {
    std::string other = other_str;
    if(other != "") init(other, covr);
}


/**
 * @brief Assignment operator
 * 
 * @param other Implicant&
 * @return Implicant&
 */
Implicant& Implicant::operator=(const Implicant& other) {
    this->implicant_str = other.implicant_str;
    this->coverage = other.coverage;
    return *this;
}

/**
 * @brief Assignment operator
 * 
 * @param other std::string
 * @return Implicant&
 */
Implicant& Implicant::operator=(const std::string& other) {
    this->implicant_str = other;
    this->coverage = {};
    return *this;
}

/**
 * @brief Operator that execute the reduction.
 * 
 * @param other Implicant&
 * @return Implicant
 */
Implicant Implicant::operator+(const Implicant& other) {

    Implicant result("", {});

    if(this->implicant_str.size() != other.implicant_str.size()) {
        throw std::logic_error("Can't reduce '"+other.implicant_str+"' and '"+this->implicant_str+"', they have different size!");
    }
    
    if(abs(this->getOneCount() - other.getOneCount()) > 1) {
        return result;
    }

    std::string new_implicant;
    
    bool found_different = false;

    for (int i = 0; i < this->implicant_str.size(); ++i) {
        if(this->implicant_str[i] == other.implicant_str[i]) {
            new_implicant += this->implicant_str[i];
        } else {
            if(!found_different && !(this->implicant_str[i] == '-' || other.implicant_str[i] == '-')) {
	        found_different = true;
                new_implicant += '-';
            } else {
                return result;
            }
        }
    }
    
    result.implicant_str = new_implicant;

    std::set_union(this->coverage.begin(), this->coverage.end(), other.coverage.begin(), other.coverage.end(), std::inserter(result.coverage, result.coverage.begin()));

    return result;
}


/**
 * @brief Equality operator
 * 
 * @param other Implicant&
 * @return bool
 */
bool Implicant::operator==(const Implicant& other) const {
    return this->implicant_str == other.implicant_str && this->coverage == other.coverage;
}

/**
 * @brief Inequality operator
 * 
 * @param other Implicant&
 * @return bool
 */
bool Implicant::operator!=(const Implicant& other) const {
    return this->implicant_str != other.implicant_str || this->coverage != other.coverage;
}

bool Implicant::operator<(const Implicant& other) const {
    return this->implicant_str < other.implicant_str;
}

/**
 * @brief Return the numbers of '1' in the implicant
 * 
 * @return int
 */
int Implicant::getOneCount() const {
    return std::count(this->implicant_str.begin(), this->implicant_str.end(), '1');
}

/**
 * @brief Return the implicant as a string
 * 
 * @return std::string
 */
std::string Implicant::getStr() const {
    return this->implicant_str;
}

/**
 * @brief Return the coverage as a string
 * 
 * @return std::string
 */
std::string Implicant::getStrCoverage() const {
    std::stringstream result;
    for(auto i = this->coverage.begin(); i != this->coverage.end(); ++i) {
        result << *i;
        if(i != --this->coverage.end()) result << ", ";
    }
    return result.str();
}


/**
 * @brief Return the coverage as a set of int
 * 
 * @return std::set< int, std::less, std::allocator< void > >
 */
std::set<int> Implicant::getCoverage() const {
    return this->coverage;
}

/**
 * @brief Flow operator for print implicant as a string
 * 
 * @param os ostream
 * @param other Implicant&
 * @return std::ostream&
 */
std::ostream &operator<<(std::ostream &os, const Implicant& other) { 
    return os << other.getStr();
}
