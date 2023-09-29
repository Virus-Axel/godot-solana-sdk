#include "curve25519.hpp"

// Multiply two 64-bit integers with 128 bits of output.
inline __uint128_t m(uint64_t x, uint64_t y){
    return ((__uint128_t) x) * ((__uint128_t) y);
}

bool FieldElement::pow2k(uint32_t k){

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

    for(int i = 0; i < 5; i++){
        this->nums[i] = a[i];
    }
    return true;
}

FieldElement::FieldElement(){
    for(int i = 0; i < 5; i++){
        nums[i] = 0;
    }
}

FieldElement& FieldElement::operator=(const FieldElement &other){
    for(int i = 0; i < 5; i++){
        this->nums[i] = other.nums[i];
    }
    return *this;
}

FieldElement FieldElement::square() const{
    FieldElement ret;
    ret = *this;
    ret.pow2k(1);
    return ret;
}

FieldElement FieldElement::operator*(const FieldElement &other){

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

FieldElement::~FieldElement(){
    
}

FieldElement sqrt_ratio_i(const FieldElement &u, const FieldElement &v){
    FieldElement v3 = u.square() * v;
    FieldElement v7 = v3.square() * v;

    // TODO
    //FieldElement r = (u * v3) * (u * v7).pow_p58();

    return v3;
}