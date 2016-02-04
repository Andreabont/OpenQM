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