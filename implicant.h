#ifndef IMPLICANT_H
#define IMPLICANT_H

#include <string>

class Implicant
{
public:
    virtual bool trivial() const = 0;
    virtual std::string getStr() const = 0;
    virtual int num_ones() const = 0;
};

#endif // IMPLICANT_H
