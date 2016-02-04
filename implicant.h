/**
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 *
 * Name        :  LNS
 * Author      :  Andrea Bontempi
 * Description :  Logic Network Synthesizer
 *
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 *
 * This file is part of the project Riddle.
 *
 *  LNS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  LNS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this project.  If not, see <http://www.gnu.org/licenses/>.
 *
 * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * - * -
 */

#ifndef LIBIMPLICANT_H
#define LIBIMPLICANT_H

#include <string>
#include <vector>
#include <set>

class Implicant {
  
private:

    std::string implicant_str;
    std::set<int> coverage;
    void init(const std::string& other, const std::set<int>& covr = {});
    void generateCoverage();

public:

    Implicant(const std::string& other, const std::set<int>& covr = {});
    Implicant(const char *other_str, const std::set<int>& covr = {});
    Implicant& operator=(const  std::string& other);
    Implicant& operator=(const Implicant& other);
    Implicant operator+(const Implicant& other);
    bool operator==(const Implicant& other) const;
    bool operator!=(const Implicant& other) const;
    bool operator<(const Implicant& other) const;
    std::string getStr() const;
    std::string getStrCoverage() const;
    int getOneCount() const;
    std::set<int> getCoverage() const;
    std::vector<Implicant> getExplodedList(int pos = 0, std::string step = "");

};

std::ostream &operator<<(std::ostream &os, const Implicant& other);

#endif //LIBIMPLICANT_H