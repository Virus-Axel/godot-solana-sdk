#include "curve25519.hpp"

// Multiply two 64-bit integers with 128 bits of output.
inline __uint128_t m(uint64_t x, uint64_t y){
    return ((__uint128_t) x) * ((__uint128_t) y);
}

const FieldElement SQRT_M1((uint64_t[]){
    1718705420411056,
    234908883556509,
    2233514472574048,
    2117202627021982,
    765476049583133,
});

const FieldElement FieldElement::ONE((const uint64_t[]){
    1, 0, 0, 0, 0
});

const FieldElement FieldElement::EDWARDS_D((const uint64_t[]){
    929955233495203,
    466365720129213,
    1662059464998953,
    2033849074728123,
    1442794654840575,
});

uint64_t FieldElement::load8(const uint8_t *data){
    uint64_t ret = 0;
    for(unsigned int i = 0; i < 8; i++){
        ret |= ((uint64_t)data + i) << 8*i;
    }
    return ret;
}

FieldElement FieldElement::pow2k(uint32_t k){

    uint64_t a[5];
    for(int i = 0; i < 5; i++){
        a[i] = nums[i];
    }

    while(k > 0){
        uint64_t a3_19 = 19 * a[3];
        uint64_t a4_19 = 19 * a[4];

        __uint128_t c0 = m(a[0],  a[0]) + 2*( m(a[1], a4_19) + m(a[2], a3_19) );
        __uint128_t c1 = m(a[3], a3_19) + 2*( m(a[0],  a[1]) + m(a[2], a4_19) );
        __uint128_t c2 = m(a[1],  a[1]) + 2*( m(a[0],  a[2]) + m(a[4], a3_19) );
        __uint128_t c3 = m(a[4], a4_19) + 2*( m(a[0],  a[3]) + m(a[1],  a[2]) );
        __uint128_t c4 = m(a[2],  a[2]) + 2*( m(a[0],  a[4]) + m(a[1],  a[3]) );

        const uint64_t LOW_51_BIT_MASK = (((uint64_t)1) << 51) - 1;

        // Casting to u64 and back tells the compiler that the carry is bounded by 2^64, so
        // that the addition is a u128 + u64 rather than u128 + u128.
        c1 += (__uint128_t)((uint64_t)(c0 >> 51));
        a[0] = ((uint64_t)c0) & LOW_51_BIT_MASK;

        c2 += (__uint128_t)((uint64_t)(c1 >> 51));
        a[1] = ((uint64_t)c1) & LOW_51_BIT_MASK;

        c3 += (__uint128_t)((uint64_t)(c2 >> 51));
        a[2] = ((uint64_t)c2) & LOW_51_BIT_MASK;

        c4 += (__uint128_t)((uint64_t)(c3 >> 51));
        a[3] = ((uint64_t)c3) & LOW_51_BIT_MASK;

        uint64_t carry = (uint64_t)(c4 >> 51);
        a[4] = ((uint64_t)c4) & LOW_51_BIT_MASK;

        k -= 1;

        a[0] += carry * 19;

        a[1] += a[0] >> 51;
        a[0] &= LOW_51_BIT_MASK;

        k -= 1;
    }

    FieldElement ret;
    for(int i = 0; i < 5; i++){
        ret.nums[i] = a[i];
    }
    return ret;
}

FieldElement& FieldElement::reduce(){
    const uint64_t LOW_51_BIT_MASK = (uint64_t(1) << 51) - 1;

    uint64_t c0 = nums[0] >> 51;
    uint64_t c1 = nums[1] >> 51;
    uint64_t c2 = nums[2] >> 51;
    uint64_t c3 = nums[3] >> 51;
    uint64_t c4 = nums[4] >> 51;

    nums[0] &= LOW_51_BIT_MASK;
    nums[1] &= LOW_51_BIT_MASK;
    nums[2] &= LOW_51_BIT_MASK;
    nums[3] &= LOW_51_BIT_MASK;
    nums[4] &= LOW_51_BIT_MASK;

    nums[0] += c4 * 19;
    nums[1] += c0;
    nums[2] += c1;
    nums[3] += c2;
    nums[4] += c3;

    return *this;
}

void FieldElement::conditional_assign(const FieldElement& other, bool condition){
    if(condition){
        *this = other;
    }
}

void FieldElement::conditional_negate(bool condition){
    if(condition){
        *this = -*this;
    }
}

FieldElement::FieldElement(){
    for(int i = 0; i < 5; i++){
        nums[i] = 0;
    }
}

FieldElement::FieldElement(const uint64_t from[5]){
    for(int i = 0; i < 5; i++){
        nums[i] = from[i];
    }
}

FieldElement::FieldElement(const uint8_t *bytes){
    const uint64_t low_51_bit_mask = (uint64_t(1) << 51) - 1;

    const uint64_t new_nums[5] = {
        load8(bytes) & low_51_bit_mask,
        // load bits [ 48,112), shift to [ 51,112)
        (load8(bytes + 6) >>  3) & low_51_bit_mask,
        // load bits [ 96,160), shift to [102,160)
        (load8(bytes + 12) >>  6) & low_51_bit_mask,
        // load bits [152,216), shift to [153,216)
        (load8(bytes + 19) >>  1) & low_51_bit_mask,
        // load bits [192,256), shift to [204,112)
        (load8(bytes + 24) >> 12) & low_51_bit_mask
    };

    for(int i = 0; i < 5; i++){
        this->nums[i] = new_nums[i];
    }
}

FieldElement& FieldElement::operator=(const FieldElement &other){
    for(int i = 0; i < 5; i++){
        this->nums[i] = other.nums[i];
    }
    return *this;
}

bool FieldElement::operator==(const FieldElement &other) const{
    for(int i = 0; i < 5; i++){
        if (nums[i] != other.nums[i]){
            return false;
        }
    }
    return true;
}

FieldElement FieldElement::square() const{
    FieldElement ret;
    ret = *this;
    ret.pow2k(1);
    return ret;
}

FieldElement FieldElement::operator*(const FieldElement &other) const{

    const uint64_t *b = other.nums;
    const uint64_t *a = this->nums;

    const uint64_t b1_19 = b[1] * 19;
    const uint64_t b2_19 = b[2] * 19;
    const uint64_t b3_19 = b[3] * 19;
    const uint64_t b4_19 = b[4] * 19;

    // Multiply to get 128-bit coefficients of output
    const __uint128_t c0 = m(a[0], b[0]) + m(a[4], b1_19) + m(a[3], b2_19) + m(a[2], b3_19) + m(a[1], b4_19);
    __uint128_t c1 = m(a[1], b[0]) + m(a[0],  b[1]) + m(a[4], b2_19) + m(a[3], b3_19) + m(a[2], b4_19);
    __uint128_t c2 = m(a[2], b[0]) + m(a[1],  b[1]) + m(a[0],  b[2]) + m(a[4], b3_19) + m(a[3], b4_19);
    __uint128_t c3 = m(a[3], b[0]) + m(a[2],  b[1]) + m(a[1],  b[2]) + m(a[0],  b[3]) + m(a[4], b4_19);
    __uint128_t c4 = m(a[4], b[0]) + m(a[3],  b[1]) + m(a[2],  b[2]) + m(a[1],  b[3]) + m(a[0] , b[4]);

    const uint64_t LOW_51_BIT_MASK = (((uint64_t) 1) << 51) - 1;
    uint64_t out[5] = {0};

    c1 += (__uint128_t)((uint64_t)(c0 >> 51));
    out[0] = ((uint64_t)c0) & LOW_51_BIT_MASK;

    c2 += (__uint128_t)((uint64_t)(c1 >> 51));
    out[1] = ((uint64_t)c1) & LOW_51_BIT_MASK;

    c3 += (__uint128_t)((uint64_t)(c2 >> 51));
    out[2] = ((uint64_t)c2) & LOW_51_BIT_MASK;

    c4 += (__uint128_t)((uint64_t)(c3 >> 51));
    out[3] = ((uint64_t)c3) & LOW_51_BIT_MASK;

    uint64_t carry = (uint64_t)(c4 >> 51);
    out[4] = ((uint64_t)c4) & LOW_51_BIT_MASK;

    out[0] += carry * 19;

    out[1] += out[0] >> 51;
    out[0] &= LOW_51_BIT_MASK;

    FieldElement ret;
    for(unsigned int i = 0; i < 5; i++){
        ret.nums[i] = out[i];
    }
    return ret;
}

FieldElement FieldElement::operator-() const{

    uint64_t inverted[] = {
        (uint64_t)36028797018963664 - nums[0],
        (uint64_t)36028797018963952 - nums[1],
        (uint64_t)36028797018963952 - nums[2],
        (uint64_t)36028797018963952 - nums[3],
        (uint64_t)36028797018963952 - nums[4],
    };
    return FieldElement(inverted).reduce();
}

FieldElement FieldElement::operator-(const FieldElement &other) const{
    const uint64_t new_nums[] = {
        (nums[0] + (uint64_t)36028797018963664) - other.nums[0],
        (nums[1] + (uint64_t)36028797018963952) - other.nums[1],
        (nums[2] + (uint64_t)36028797018963952) - other.nums[2],
        (nums[3] + (uint64_t)36028797018963952) - other.nums[3],
        (nums[4] + (uint64_t)36028797018963952) - other.nums[4],
    };
    FieldElement ret(new_nums);
    ret.reduce();

    return ret;
}

FieldElement FieldElement::operator+(const FieldElement &other) const{
    const uint64_t new_nums[] = {
        nums[0] + other.nums[0],
        nums[1] + other.nums[1],
        nums[2] + other.nums[2],
        nums[3] + other.nums[3],
        nums[4] + other.nums[4],
    };
    return FieldElement(new_nums); 
}

bool FieldElement::is_negative() const{
    return (bool)(nums[0] & 1);
}

FieldElement::~FieldElement(){
    
}

void FieldElement::pow22501(FieldElement &t3, FieldElement &t19) const{
    FieldElement t0 = square();
    FieldElement t1 = t0.square().square();
    FieldElement t2 = (*this) * t1;
    t3 = t0 * t2;
    FieldElement t4 = t3.square();
    FieldElement t5 = t2 * t4;
    FieldElement t6 = t5.pow2k(5);
    FieldElement t7 = t6 * t5;
    FieldElement t8 = t7.pow2k(10);
    FieldElement t9 = t8 * t7;
    FieldElement t10 = t9.pow2k(20);
    FieldElement t11 = t10 * t9;
    FieldElement t12 = t11.pow2k(10);
    FieldElement t13 = t12 * t7;
    FieldElement t14 = t13.pow2k(50);
    FieldElement t15 = t14 * t13;
    FieldElement t16 = t15.pow2k(100);
    FieldElement t17 = t16 * t15;
    FieldElement t18 = t17.pow2k(50);
    t19 = t18 * t13;
}

FieldElement FieldElement::pow_p58() const{
    FieldElement t19;
    FieldElement dummy;
    pow22501(t19, dummy);
    FieldElement t20 = t19.pow2k(2);
    FieldElement t21 = *this * t20;

    return t21;
}

FieldElement sqrt_ratio_i(const FieldElement &u, const FieldElement &v, bool &was_nonzero_square){
    FieldElement v3 = u.square() * v;
    FieldElement v7 = v3.square() * v;

    FieldElement r = (u * v3) * (u * v7).pow_p58();

    FieldElement check = v * r.square();
    FieldElement i = SQRT_M1;

    bool correct_sign_sqrt = check == u;
    bool flipped_sign_sqrt = check == (-u);
    bool flipped_sign_sqrt_i = check == ((-u) * i);

    FieldElement r_prime = SQRT_M1 * r;
    r.conditional_assign(r_prime, flipped_sign_sqrt | flipped_sign_sqrt_i);
    bool r_is_negative = r.is_negative();
    r.conditional_negate(r_is_negative);

    was_nonzero_square = correct_sign_sqrt | flipped_sign_sqrt;

    return r;
}

bool decompress_step_1(const uint8_t *repr, FieldElement &x, FieldElement &y, FieldElement &z){
    y = FieldElement(repr);
    z = FieldElement::ONE;

    FieldElement YY = y.square();
    FieldElement u = YY - z;

    FieldElement v = (YY * FieldElement::EDWARDS_D) + z;
    bool is_valid_y_point = false;
    x = sqrt_ratio_i(u, v, is_valid_y_point);

    return is_valid_y_point;
}