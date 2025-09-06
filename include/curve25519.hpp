#ifndef CURVE25519_HPP
#define CURVE25519_HPP

#include <cstdint>
#include <initializer_list>

// NOLINTBEGIN(readability-identifier-length,cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

/**
 * @brief Component used in curve25519 encryption.
 */
class FieldElement {
private:
	static const uint8_t INTERNAL_ARRAY_LENGTH = 5;

	uint64_t nums[INTERNAL_ARRAY_LENGTH] = { 0ULL };

	FieldElement &reduce();
	FieldElement pow2k(uint32_t k);

public:
	static const FieldElement ONE; ///< FieldElement representing 1
	static const FieldElement EDWARDS_D; ///< FieldElement representing curve constant D.

	FieldElement() = default;

	/**
	 * @brief Construct a new Field Element object from a normal 64 bit integer array.
	 *
	 * @param from The integer array representation.
	 */
	explicit FieldElement(const uint64_t from[INTERNAL_ARRAY_LENGTH]);

	/**
	 * @brief Construct a new Field Element object from a stream of bytes.
	 *
	 * @param bytes Pointer to an array of bytes.
	 */
	explicit FieldElement(const uint8_t *bytes);

	/**
	 * @brief Construct a new Field Element object from another Field Element.
	 *
	 * @param other The object to copy.
	 */
	FieldElement(const FieldElement &other);

	/**
	 * @brief Construct a new Field Element object from an initializer list.
	 *
	 * @param init Initializer list with elements.
	 */
	FieldElement(std::initializer_list<uint64_t> init);

	/**
	 * @brief Assigns a Field Element if condition is true.
	 *
	 * @param other Field element to maybe assign.
	 * @param condition Specifies if assign should happen.
	 */
	void conditional_assign(const FieldElement &other, bool condition);

	/**
	 * @brief Negates a Field Element if condition is true.
	 *
	 * @param condition Specifies if negate should happen.
	 */
	void conditional_negate(bool condition);

	/**
	 * @brief Raises a field element to the power of \(2^{250} - 1\).
	 *
	 * @param t3 Value after third operation.
	 * @param t19 Value after 19th operation.
	 */
	void pow22501(FieldElement &t3, FieldElement &t19) const;

	/**
	 * @brief Raises the field element to the power of \((p-5)/8\).
	 *
	 * @return FieldElement The resulting `FieldElement` after exponentiation.
	 */
	[[nodiscard]] FieldElement pow_p58() const;

	/**
	 * @brief Computes the square of the field element.
	 *
	 * @return FieldElement Squared field element.
	 */
	[[nodiscard]] FieldElement square() const;

	/**
	 * @brief Assigns this object with data from another FieldElement.
	 *
	 * @param other Data to copy from.
	 * @return FieldElement& Reference to the newly assigned object.
	 */
	FieldElement &operator=(const FieldElement &other);

	/**
	 * @brief Multiplies two FieldElements.
	 *
	 * @param other FieldElement to multiply with.
	 * @return FieldElement Product of the multiplication.
	 */
	FieldElement operator*(const FieldElement &other) const;

	/**
	 * @brief Inverts and reduces by 1.
	 *
	 * @return Inverted FieldElement reduced by 1.
	 */
	FieldElement operator-() const;

	/**
	 * @brief Subtracts FieldElements
	 *
	 * @param other FieldElement to subtract.
	 * @return FieldElement Differance of the subtraction.
	 */
	FieldElement operator-(const FieldElement &other) const;

	/**
	 * @brief Adds two field elements.
	 *
	 * @param other FieldElement to add.
	 * @return FieldElement Sum of the addition.
	 */
	FieldElement operator+(const FieldElement &other) const;

	/**
	 * @brief Checks equality of two FieldElements.
	 *
	 * @param other FieldElement to check against.
	 * @return true when both elements are equal.
	 * @return false when elements are not equal.
	 */
	bool operator==(const FieldElement &other) const;

	/**
	 * @brief Checks if FieldElement is negative.
	 *
	 * @return true if FieldElement is negative.
	 * @return false if FieldElement is zero or possitive.
	 */
	[[nodiscard]] bool is_negative() const;
	~FieldElement() = default;
};

bool decompress_step_1(const uint8_t *repr, FieldElement &x, FieldElement &y, FieldElement &z);
bool is_y_point_valid(const uint8_t *repr);
FieldElement sqrt_ratio_i(const FieldElement &u, const FieldElement &v, bool &was_nonzero_square);

// NOLINTEND(readability-identifier-length,cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

#endif