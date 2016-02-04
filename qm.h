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

#ifndef LIBQM_H
#define LIBQM_H

#include "qm.h"
#include "implicant.h"

#include <vector>

struct stepResult {
    std::vector<Implicant> reduced;
    std::vector<Implicant> excluded;
};

stepResult makeQMStep(std::vector<Implicant> implicantList);
std::vector<Implicant> makeQM(const std::vector<Implicant>& implicantList, const std::vector<Implicant>& dontCareList);
std::string getBooleanExpression(std::vector<Implicant> solution);

#endif // LIBQM_H
