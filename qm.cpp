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

#include "qm.h"
#include "implicant.h"

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>

/**
 * @brief Execute QM step
 * 
 * @param implicantList List of implicants
 * @return stepResult
 */
stepResult makeQMStep(std::vector<Implicant> implicantList){
    
    // stepResult
    stepResult result;
    
    // Save list size
    int implicantListSize = implicantList.size();
        
    // Group implicants based on implicant's one numbers
    std::map<int, std::vector<int>> implicantGroup;
    
    // Max one number group
    int implicantGroupMax = 0;
    
    for(int i = 0; i < implicantListSize; ++i) {
        int oneCount = implicantList[i].getOneCount();
        implicantGroup[oneCount].push_back(i);
        if(oneCount > implicantGroupMax) {
            implicantGroupMax = oneCount;
        }
    }
    
    // Implicant marked
    std::set<int> marked;
    
    // Combination and reduction of implicants
    for(int i = 0; i < implicantGroupMax; ++i) {
        for(int x : implicantGroup[i]) {
            for(int y : implicantGroup[i+1]) {
                
                // Reduce!
                Implicant temp = implicantList[x] + implicantList[y];
                
                // Fail?
                if(temp.getStr() == "") {
                    continue;
                }
                
                // Mark!
                marked.insert(x);
                marked.insert(y);            
                
                // Save!
                result.reduced.push_back(temp);
                
            }
        }
    }
    
    // Result must me unique
    std::sort(result.reduced.begin(), result.reduced.end() );
    result.reduced.erase(std::unique(result.reduced.begin(), result.reduced.end() ), result.reduced.end() );
    
    // Compile exclude list
    for(int i = 0; i < implicantListSize; ++i) {
        if(!marked.count(i)) {
            result.excluded.push_back(implicantList[i]);
        }
    }
    
    return result;
    
}

/**
 * @brief Execute QM
 * 
 * @param implicantList List of implicants
 * @param dontCareList List of don't care
 * @return std::vector< Implicant, std::allocator< void > >
 */

std::vector< Implicant > makeQM(const std::vector<Implicant>& implicantList, const std::vector<Implicant>& dontCareList) {

    std::vector<Implicant> solution;
    std::vector<Implicant> list = implicantList;
    
    list.insert(list.end(), dontCareList.begin(), dontCareList.end());
    
    std::set<int> dontCareCoverage;
    
    for(Implicant i : dontCareList) {
        std::set<int> tempCoverage = i.getCoverage();
        dontCareCoverage.insert(tempCoverage.begin(), tempCoverage.end());
    }
    
    stepResult result;

    do {

        result = makeQMStep(list);
        list = result.reduced;
        solution.insert(solution.end(), result.excluded.begin(), result.excluded.end());

    } while(result.reduced.size() != 0);
    
    if(dontCareCoverage.size() != 0) { 
        solution.erase(std::remove_if(solution.begin(), solution.end(), [dontCareCoverage](const Implicant& i) -> bool {
            std::set<int> check;
            std::set<int> tempCoverage = i.getCoverage();
            std::set_difference(tempCoverage.begin(), tempCoverage.end(), dontCareCoverage.begin(), dontCareCoverage.end(), std::inserter(check, check.begin()));
            return check.size() == 0;
        }), solution.end());
    }

    return solution;

}

/**
 * @brief Generate boolean expression from implicant list (solution of QM)
 * 
 * @param solution Implicant list
 * @return std::string
 */
std::string getBooleanExpression(std::vector<Implicant> solution) {
    std::stringstream result;
    for (auto i = solution.begin(); i != solution.end(); ++i) {
        std::string implStr = i->getStr();
        for (std::string::size_type charPos = 0; charPos <= implStr.size(); ++charPos) {
            switch (implStr[charPos]) {
                case '0':
                    result << '(' << charPos << "\')"; 
                    break;
                case '1':
                    result << '(' << charPos << ')';
                    break;
                default:
                    continue;
            }
        }
        if (i != --solution.end())
            result << " ";
    }
    return result.str();
}

