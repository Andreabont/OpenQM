#ifndef VECTORBOOLIMPLICANT_H
#define VECTORBOOLIMPLICANT_H

#include <vector>
#include <string>
#include <iostream>

class VectorBoolImplicant
{
    std::vector<bool> code;
public:
    VectorBoolImplicant(const std::string &str);
    friend VectorBoolImplicant operator*(const VectorBoolImplicant &l, const VectorBoolImplicant &r);
    friend bool operator==(const VectorBoolImplicant &l, const VectorBoolImplicant &r);
    friend bool operator<(const VectorBoolImplicant &l, const VectorBoolImplicant &r);
    friend std::ostream &operator<<(std::ostream &os, const VectorBoolImplicant &implicant);
    bool trivial() const;
};

#endif // VECTORBOOLIMPLICANT_H
