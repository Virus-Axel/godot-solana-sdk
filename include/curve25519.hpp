#ifndef CURVE25519_HPP
#define CURVE25519_HPP

#include <godot_cpp/classes/node.hpp>

/*
class uint128_t{
private:
    uint64_t data[2];
public:
    uint128_t(){
        data[0] = 0;
        data[1] = 0;
    }
    uint128_t(uint64_t a, uint64_t b){
        data[0] = a;
        data[1] = b;
    }

    uint128_t operator +(const uint128_t& other){
        uint128_t res;
        res.data[0] = other.data[0] + this->data[0];
        res.data[1] = other.data[1] + this->data[1];

        // check overflow.
        if((res.data[0] < other.data[0]) || (res.data[0] < this->data[0])){
            res.data[1] += 1;
        }
    }

    static uint128_t m(uint64_t a, uint64_t b){
        return uint128_t();
    }

    uint128_t operator =(const uint128_t& other){
        data[0] = other.data[0];
        data[1] = other.data[1];
    }
};*/

class FieldElement{
private:
    uint64_t nums[5];

    
    FieldElement& reduce();
    static uint64_t load8(const uint8_t *data);
public:
FieldElement pow2k(uint32_t m);
    static const FieldElement ONE;
    static const FieldElement EDWARDS_D;

    FieldElement();
    FieldElement(const uint64_t from[5]);
    FieldElement(const uint8_t *bytes);
    FieldElement(const FieldElement& other);
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
bool is_y_point_valid(const uint8_t *repr);
FieldElement sqrt_ratio_i(const FieldElement &u, const FieldElement &v, bool &was_nonzero_square);

#endif