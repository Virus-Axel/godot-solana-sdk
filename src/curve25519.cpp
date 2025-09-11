#include "curve25519.hpp"

#include <cstdint>
#include <initializer_list>

//NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-identifier-length,cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays,cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-pointer-arithmetic,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

namespace {
// Multiply two 64-bit integers with 128 bits of output.
inline __uint128_t m(uint64_t x, uint64_t y) {
	return static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y);
}
} //namespace

const FieldElement SQRT_M1((uint64_t[]){
		1718705420411056,
		234908883556509,
		2233514472574048,
		2117202627021982,
		765476049583133,
});

const FieldElement FieldElement::ONE((const uint64_t[]){
		1, 0, 0, 0, 0 });

const FieldElement FieldElement::EDWARDS_D((const uint64_t[]){
		929955233495203,
		466365720129213,
		1662059464998953,
		2033849074728123,
		1442794654840575,
});

FieldElement FieldElement::pow2k(uint32_t k) {
	uint64_t a[5];
	for (int i = 0; i < 5; i++) {
		a[i] = nums[i];
	}

	while (k > 0) {
		const uint64_t a3_19 = a[3] * 19;
		const uint64_t a4_19 = a[4] * 19;

		const __uint128_t c0 = m(a[0], a[0]) + (2 * (m(a[1], a4_19) + m(a[2], a3_19)));
		__uint128_t c1 = m(a[3], a3_19) + (2 * (m(a[0], a[1]) + m(a[2], a4_19)));
		__uint128_t c2 = m(a[1], a[1]) + (2 * (m(a[0], a[2]) + m(a[4], a3_19)));
		__uint128_t c3 = m(a[4], a4_19) + (2 * (m(a[0], a[3]) + m(a[1], a[2])));
		__uint128_t c4 = m(a[2], a[2]) + (2 * (m(a[0], a[4]) + m(a[1], a[3])));

		const uint64_t LOW_51_BIT_MASK = (1ULL << 51U) - 1ULL;

		// Casting to u64 and back tells the compiler that the carry is bounded by 2^64, so
		// that the addition is a u128 + u64 rather than u128 + u128.
		c1 += static_cast<__uint128_t>(static_cast<uint64_t>(c0 >> 51U));
		a[0] = static_cast<uint64_t>(c0) & LOW_51_BIT_MASK;

		c2 += static_cast<__uint128_t>(static_cast<uint64_t>(c1 >> 51U));
		a[1] = static_cast<uint64_t>(c1) & LOW_51_BIT_MASK;

		c3 += static_cast<__uint128_t>(static_cast<uint64_t>(c2 >> 51U));
		a[2] = static_cast<uint64_t>(c2) & LOW_51_BIT_MASK;

		c4 += static_cast<__uint128_t>(static_cast<uint64_t>(c3 >> 51U));
		a[3] = static_cast<uint64_t>(c3) & LOW_51_BIT_MASK;

		const auto carry = static_cast<uint64_t>(c4 >> 51U);
		a[4] = static_cast<uint64_t>(c4) & LOW_51_BIT_MASK;

		a[0] += carry * 19;

		a[1] += a[0] >> 51U;
		a[0] &= LOW_51_BIT_MASK;

		k -= 1;
	}

	FieldElement ret;
	for (int i = 0; i < 5; i++) {
		ret.nums[i] = a[i];
	}
	return ret;
}

FieldElement &FieldElement::reduce() {
	const uint64_t LOW_51_BIT_MASK = (1ULL << 51U) - 1U;

	const uint64_t c0 = nums[0] >> 51U;
	const uint64_t c1 = nums[1] >> 51U;
	const uint64_t c2 = nums[2] >> 51U;
	const uint64_t c3 = nums[3] >> 51U;
	const uint64_t c4 = nums[4] >> 51U;

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

void FieldElement::conditional_assign(const FieldElement &other, bool condition) {
	if (condition) {
		*this = other;
	}
}

void FieldElement::conditional_negate(bool condition) {
	if (condition) {
		*this = -*this;
	}
}

FieldElement::FieldElement(const uint64_t from[INTERNAL_ARRAY_LENGTH]) { // NOLINT(hicpp-member-init)
	for (int i = 0; i < 5; i++) {
		nums[i] = from[i];
	}
}

FieldElement::FieldElement(const FieldElement &other) { // NOLINT(hicpp-member-init)
	for (int i = 0; i < 5; i++) {
		nums[i] = other.nums[i];
	}
}

FieldElement::FieldElement(std::initializer_list<uint64_t> init) { // NOLINT(hicpp-member-init)
	int i = 0;
	for (auto v : init) {
		if (i < 5) {
			nums[i++] = v;
		}
	}
}

FieldElement::FieldElement(const uint8_t *bytes) { // NOLINT(hicpp-member-init)
	uint8_t temp[32];
	for (unsigned int i = 0; i < 32; i++) {
		temp[i] = bytes[i];
	}
	temp[31] &= 127U;

	const uint64_t x1 = (static_cast<uint64_t>(temp[31]) << 44U);
	const uint64_t x2 = (static_cast<uint64_t>(temp[30]) << 36U);
	const uint64_t x3 = (static_cast<uint64_t>(temp[29]) << 28U);
	const uint64_t x4 = (static_cast<uint64_t>(temp[28]) << 20U);
	const uint64_t x5 = (static_cast<uint64_t>(temp[27]) << 12U);
	const uint64_t x6 = (static_cast<uint64_t>(temp[26]) << 4U);
	const uint64_t x7 = (static_cast<uint64_t>(temp[25]) << 47U);
	const uint64_t x8 = (static_cast<uint64_t>(temp[24]) << 39U);
	const uint64_t x9 = (static_cast<uint64_t>(temp[23]) << 31U);
	const uint64_t x10 = (static_cast<uint64_t>(temp[22]) << 23U);
	const uint64_t x11 = (static_cast<uint64_t>(temp[21]) << 15U);
	const uint64_t x12 = (static_cast<uint64_t>(temp[20]) << 7U);
	const uint64_t x13 = (static_cast<uint64_t>(temp[19]) << 50U);
	const uint64_t x14 = (static_cast<uint64_t>(temp[18]) << 42U);
	const uint64_t x15 = (static_cast<uint64_t>(temp[17]) << 34U);
	const uint64_t x16 = (static_cast<uint64_t>(temp[16]) << 26U);
	const uint64_t x17 = (static_cast<uint64_t>(temp[15]) << 18U);
	const uint64_t x18 = (static_cast<uint64_t>(temp[14]) << 10U);
	const uint64_t x19 = (static_cast<uint64_t>(temp[13]) << 2U);
	const uint64_t x20 = (static_cast<uint64_t>(temp[12]) << 45U);
	const uint64_t x21 = (static_cast<uint64_t>(temp[11]) << 37U);
	const uint64_t x22 = (static_cast<uint64_t>(temp[10]) << 29U);
	const uint64_t x23 = (static_cast<uint64_t>(temp[9]) << 21U);
	const uint64_t x24 = (static_cast<uint64_t>(temp[8]) << 13U);
	const uint64_t x25 = (static_cast<uint64_t>(temp[7]) << 5U);
	const uint64_t x26 = (static_cast<uint64_t>(temp[6]) << 48U);
	const uint64_t x27 = (static_cast<uint64_t>(temp[5]) << 40U);
	const uint64_t x28 = (static_cast<uint64_t>(temp[4]) << 32U);
	const uint64_t x29 = (static_cast<uint64_t>(temp[3]) << 24U);
	const uint64_t x30 = (static_cast<uint64_t>(temp[2]) << 16U);
	const uint64_t x31 = (static_cast<uint64_t>(temp[1]) << 8U);
	const uint64_t x32 = (temp[0]);
	const uint64_t x33 = (x31 + x32);
	const uint64_t x34 = (x30 + x33);
	const uint64_t x35 = (x29 + x34);
	const uint64_t x36 = (x28 + x35);
	const uint64_t x37 = (x27 + x36);
	const uint64_t x38 = (x26 + x37);
	const uint64_t x39 = (x38 & 0x7ffffffffffffULL);
	const uint64_t x40 = static_cast<uint8_t>(x38 >> 51U);
	const uint64_t x41 = (x25 + x40);
	const uint64_t x42 = (x24 + x41);
	const uint64_t x43 = (x23 + x42);
	const uint64_t x44 = (x22 + x43);
	const uint64_t x45 = (x21 + x44);
	const uint64_t x46 = (x20 + x45);
	const uint64_t x47 = (x46 & 0x7ffffffffffffULL);
	const uint64_t x48 = static_cast<uint8_t>(x46 >> 51U);
	const uint64_t x49 = x19 + x48;
	const uint64_t x50 = (x18 + x49);
	const uint64_t x51 = (x17 + x50);
	const uint64_t x52 = (x16 + x51);
	const uint64_t x53 = (x15 + x52);
	const uint64_t x54 = (x14 + x53);
	const uint64_t x55 = (x13 + x54);
	const uint64_t x56 = (x55 & 0x7ffffffffffffULL);
	const uint64_t x57 = static_cast<uint8_t>(x55 >> 51U);
	const uint64_t x58 = x12 + x57;
	const uint64_t x59 = (x11 + x58);
	const uint64_t x60 = (x10 + x59);
	const uint64_t x61 = (x9 + x60);
	const uint64_t x62 = (x8 + x61);
	const uint64_t x63 = (x7 + x62);
	const uint64_t x64 = (x63 & 0x7ffffffffffffULL);
	const uint64_t x65 = static_cast<uint8_t>(x63 >> 51U);
	const uint64_t x66 = x6 + x65;
	const uint64_t x67 = (x5 + x66);
	const uint64_t x68 = (x4 + x67);
	const uint64_t x69 = (x3 + x68);
	const uint64_t x70 = (x2 + x69);
	const uint64_t x71 = (x1 + x70);

	this->nums[0] = x39;
	this->nums[1] = x47;
	this->nums[2] = x56;
	this->nums[3] = x64;
	this->nums[4] = x71;
}

FieldElement &FieldElement::operator=(const FieldElement &other) {
	if (this == &other) {
		return *this;
	}
	for (int i = 0; i < 5; i++) {
		this->nums[i] = other.nums[i];
	}
	return *this;
}

bool FieldElement::operator==(const FieldElement &other) const {
	for (int i = 0; i < 5; i++) {
		if (nums[i] != other.nums[i]) {
			return false;
		}
	}
	return true;
}

FieldElement FieldElement::square() const {
	FieldElement ret;
	ret = (*this);
	return ret.pow2k(1);
}

FieldElement FieldElement::operator*(const FieldElement &other) const {
	const uint64_t b[] = { other.nums[0], other.nums[1], other.nums[2], other.nums[3], other.nums[4] };
	const uint64_t a[] = { this->nums[0], this->nums[1], this->nums[2], this->nums[3], this->nums[4] };

	const uint64_t b1_19 = b[1] * 19;
	const uint64_t b2_19 = b[2] * 19;
	const uint64_t b3_19 = b[3] * 19;
	const uint64_t b4_19 = b[4] * 19;

	// Multiply to get 128-bit coefficients of output
	const __uint128_t c0 = m(a[0], b[0]) + m(a[4], b1_19) + m(a[3], b2_19) + m(a[2], b3_19) + m(a[1], b4_19);
	__uint128_t c1 = m(a[1], b[0]) + m(a[0], b[1]) + m(a[4], b2_19) + m(a[3], b3_19) + m(a[2], b4_19);
	__uint128_t c2 = m(a[2], b[0]) + m(a[1], b[1]) + m(a[0], b[2]) + m(a[4], b3_19) + m(a[3], b4_19);
	__uint128_t c3 = m(a[3], b[0]) + m(a[2], b[1]) + m(a[1], b[2]) + m(a[0], b[3]) + m(a[4], b4_19);
	__uint128_t c4 = m(a[4], b[0]) + m(a[3], b[1]) + m(a[2], b[2]) + m(a[1], b[3]) + m(a[0], b[4]);

	const uint64_t LOW_51_BIT_MASK = (1ULL << 51U) - 1U;
	uint64_t out[5] = { 0U };

	c1 += static_cast<__uint128_t>(static_cast<uint64_t>(c0 >> 51U));
	out[0] = static_cast<uint64_t>(c0) & LOW_51_BIT_MASK;

	c2 += static_cast<__uint128_t>(static_cast<uint64_t>(c1 >> 51U));
	out[1] = static_cast<uint64_t>(c1) & LOW_51_BIT_MASK;

	c3 += static_cast<__uint128_t>(static_cast<uint64_t>(c2 >> 51U));
	out[2] = static_cast<uint64_t>(c2) & LOW_51_BIT_MASK;

	c4 += static_cast<__uint128_t>(static_cast<uint64_t>(c3 >> 51U));
	out[3] = static_cast<uint64_t>(c3) & LOW_51_BIT_MASK;

	const auto carry = static_cast<uint64_t>(c4 >> 51U);

	out[4] = (static_cast<uint64_t>(c4)) & LOW_51_BIT_MASK;

	out[0] += carry * 19;

	out[1] += out[0] >> 51U;
	out[0] &= LOW_51_BIT_MASK;

	return FieldElement({ out[0], out[1], out[2], out[3], out[4] });
}

FieldElement FieldElement::operator-() const {
	return FieldElement({
								36028797018963664ULL - nums[0],
								36028797018963952ULL - nums[1],
								36028797018963952ULL - nums[2],
								36028797018963952ULL - nums[3],
								36028797018963952ULL - nums[4],
						})
			.reduce();
}

FieldElement FieldElement::operator-(const FieldElement &other) const {
	FieldElement ret({
			(nums[0] + 36028797018963664ULL) - other.nums[0],
			(nums[1] + 36028797018963952ULL) - other.nums[1],
			(nums[2] + 36028797018963952ULL) - other.nums[2],
			(nums[3] + 36028797018963952ULL) - other.nums[3],
			(nums[4] + 36028797018963952ULL) - other.nums[4],
	});
	ret.reduce();

	return ret;
}

FieldElement FieldElement::operator+(const FieldElement &other) const {
	return FieldElement({
			nums[0] + other.nums[0],
			nums[1] + other.nums[1],
			nums[2] + other.nums[2],
			nums[3] + other.nums[3],
			nums[4] + other.nums[4],
	});
}

bool FieldElement::is_negative() const {
	return static_cast<bool>(nums[0] & 1U);
}

void FieldElement::pow22501(FieldElement &t3, FieldElement &t19) const {
	const FieldElement t0 = square();
	const FieldElement t1 = t0.square().square();
	const FieldElement t2 = (*this) * t1;
	t3 = t0 * t2;
	const FieldElement t4 = t3.square();
	FieldElement t5 = t2 * t4;
	const FieldElement t6 = t5.pow2k(5);
	FieldElement t7 = t6 * t5;
	const FieldElement t8 = t7.pow2k(10);
	FieldElement t9 = t8 * t7;
	const FieldElement t10 = t9.pow2k(20);
	FieldElement t11 = t10 * t9;
	const FieldElement t12 = t11.pow2k(10);
	FieldElement t13 = t12 * t7;
	const FieldElement t14 = t13.pow2k(50);
	FieldElement t15 = t14 * t13;
	const FieldElement t16 = t15.pow2k(100);
	FieldElement t17 = t16 * t15;
	const FieldElement t18 = t17.pow2k(50);
	t19 = t18 * t13;
}

FieldElement FieldElement::pow_p58() const {
	FieldElement t19;
	FieldElement dummy;
	pow22501(dummy, t19);

	const FieldElement t20 = t19.pow2k(2);
	const FieldElement t21 = (*this) * t20;

	return t21;
}

FieldElement sqrt_ratio_i(const FieldElement &u, const FieldElement &v, bool &was_nonzero_square) {
	const FieldElement v3 = v.square() * v;
	const FieldElement v7 = v3.square() * v;

	FieldElement r = (u * v3) * (u * v7).pow_p58();

	const FieldElement check = v * r.square();
	const FieldElement i = SQRT_M1;

	const bool correct_sign_sqrt = (check == u);
	const bool flipped_sign_sqrt = (check == (-u));
	const bool flipped_sign_sqrt_i = (check == ((-u) * i));

	const FieldElement r_prime = SQRT_M1 * r;
	r.conditional_assign(r_prime, flipped_sign_sqrt || flipped_sign_sqrt_i);
	const bool r_is_negative = r.is_negative();
	r.conditional_negate(r_is_negative);

	was_nonzero_square = (correct_sign_sqrt || flipped_sign_sqrt);

	return r;
}

bool decompress_step_1(const uint8_t *repr, FieldElement &x, FieldElement &y, FieldElement &z) {
	y = FieldElement(repr);
	z = FieldElement::ONE;

	const FieldElement YY = y.square();
	const FieldElement u = YY - z;

	const FieldElement v = (YY * FieldElement::EDWARDS_D) + z;
	bool is_valid_y_point = false;
	x = sqrt_ratio_i(u, v, is_valid_y_point);

	return is_valid_y_point;
}

bool is_y_point_valid(const uint8_t *repr) {
	const FieldElement y(repr);

	const FieldElement YY = y.square();
	const FieldElement u = YY - FieldElement::ONE;

	const FieldElement v = (YY * FieldElement::EDWARDS_D) + FieldElement::ONE;
	bool is_valid_y_point = false;

	sqrt_ratio_i(u, v, is_valid_y_point);

	return is_valid_y_point;
}

//NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-identifier-length,cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays,cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-pointer-arithmetic,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)