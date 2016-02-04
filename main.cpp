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

#include <iostream>
#include <stdexcept>
#include <vector>
#include "implicant.h"
#include "qm.h"

using namespace std;

int main() try {

    std::vector<Implicant> list;

    string input;
    
    cout << "Insert implicants followed by return. Write 'q' or 'quit' to exit..." << endl;
    
    cin >> input;
    
    while (!(input == "quit" || input == "q")) try {

        Implicant imp = input;
        list.push_back(imp);
        cin >> input;
        
    } catch (std::logic_error e) {
        cerr << "Error: " << e.what() << endl;
        break;
    } 
    
    if(list.empty()) {
        cerr << "Error: No implicants..." << endl;
        return EXIT_FAILURE;
    }
    
    std::vector<Implicant> solution = makeQM(list, {});
    
    cout << endl << "Solution:" << endl;

    for(Implicant i : solution) {
        cout << i << " (" << i.getStrCoverage() << ")" << endl;
    }

    cout << endl << "Expression: " << getBooleanExpression(solution) << endl;
    
    return EXIT_SUCCESS;

}  catch (std::logic_error e) {
    cerr << "Fatal error: " << e.what() << endl;
    return EXIT_FAILURE;
} 