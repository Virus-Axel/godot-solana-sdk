#ifndef CURVE25519_HPP
#define CURVE25519_HPP

#include <godot_cpp/classes/node.hpp>

class FieldElement{
private:
    uint64_t nums[5];

    FieldElement pow2k(uint32_t m);
    FieldElement& reduce();
    static uint64_t load8(const uint8_t *data);
public:
    static const FieldElement ONE;
    static const FieldElement EDWARDS_D;

    FieldElement();
    FieldElement(const uint64_t from[5]);
    FieldElement(const uint8_t *bytes);
    void conditional_assign(const FieldElement& other, bool condition);
    void conditional_negate(bool condition);
    void pow22501(FieldElement &t3, FieldElement &t19) const;
    FieldElement pow_p58() const;
    FieldElement square() const;
    FieldElement& operator=(const FieldElement &other);
    FieldElement operator*(const FieldElement &other) const;
    FieldElement operator-() const;
    FieldElement operator-(const FieldElement &other) const;
    FieldElement operator+(const FieldElement &other) const;
    bool operator==(const FieldElement &other) const;
    bool is_negative() const;
    ~FieldElement();
};

bool decompress_step_1(const uint8_t *repr, FieldElement &x, FieldElement &y, FieldElement &z);
FieldElement sqrt_ratio_i(const FieldElement &u, const FieldElement &v, bool &was_nonzero_square);

#endif