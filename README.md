Fork of [@Andreabont's implementation](https://github.com/Andreabont/OpenQM) of [Quine–McCluskey algorithm](https://en.wikipedia.org/wiki/Quine%E2%80%93McCluskey_algorithm).

Build
=====

Installing the dependencies
---------------------------
- cmake
- libboost (system, test, programoptions)

Compile!
--------
```sh
git clone https://github.com/fourlex/OpenQM
mkdir build
cd build
cmake ..
make
```

Usage in CLI
============

```bash
#> ./OpenQM -h
Data is read from stdin. Options::
  -h [ --help ]                       print this help message
  -f [ --input-format ] arg (=vector) specify input format of function 
                                      definition. can be 'vector','charset' or 
                                      'implicants' (see README.md)
  -n [ --num-var ] arg                specify number of function variables. 
                                      required by 'vector' input format

#> cat input.txt 
0100000010111101100101101000011100101100011111000011101010011110
#> ./OpenQM -n 6 < input.txt
000001
-10011
-11000
0-1000
001-00
001-11
0010-0
01-000
01-101
01111-
1-0010
1-1011
10-010
101-01
1010-1
11-011
110-10
11001-
111-00
--1101
-0101-
-0110-
-1-110
0-11-1
1--100
1-110-
10-10-
11-1-0
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
