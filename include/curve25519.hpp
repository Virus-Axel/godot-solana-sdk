#ifndef CURVE25519_HPP
#define CURVE25519_HPP

#include <godot_cpp/classes/node.hpp>

class FieldElement{
private:
    uint64_t nums[5];

    bool pow2k(uint32_t m);
public:
    FieldElement();
    FieldElement square() const;
    FieldElement& operator=(const FieldElement &other);
    FieldElement operator*(const FieldElement &other);
    ~FieldElement();
};

FieldElement sqrt_ratio_i(const FieldElement &u, const FieldElement &v);

#endif