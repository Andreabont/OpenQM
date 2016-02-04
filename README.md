OpenQM is a implementation of [Quine–McCluskey algorithm](https://en.wikipedia.org/wiki/Quine%E2%80%93McCluskey_algorithm) focused on usability, relased under GNU GPLv3 license. It provide a library to integrate the algorithm in other prjects and a simple CLI executable.

Build
=====

Installing the dependencies
---------------------------
- cmake
- libboost

Compile!
--------
```sh
git clone https://github.com/Andreabont/OpenQM.git
mkdir build
cd build
cmake ..
make
make test
```

Usage in CLI
============

```bash
#>./OpenMP 
Insert implicants followed by return. Write 'q' or 'quit' to exit...
1001
1110
1100
q

Solution:
1001 (9)
11-0 (12, 14)

Expression: ab'c'd + abd'

```

Usage as library
================

Implicants
----------

```cpp
#include <vector>
#include "implicant.h"

int main() {

    Implicant singleImplicant = "0101";

    std::vector<Implicant> listOfImplicants = {
        "0001",
        "1001",
        "1011",
        "1100",
        "1101",
        "1110",
        "1111"
    };

}
```

Apply Quine–McCluskey
---------------------

```cpp
# Get the QM solution
std::vector<Implicant> solution = makeQM(implicant_list, dont_care_list);

# Get the boolean expression
std:string expression = getBooleanExpression(solution);

```
