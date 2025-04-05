#ifndef SOLANA_SDK_COMPUTE_BUDGET_HPP
#define SOLANA_SDK_COMPUTE_BUDGET_HPP

#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>

namespace godot {

/**
 * @brief Instruction builder for Compute Budget program.
 */
class ComputeBudget : public Node {
	GDCLASS(ComputeBudget, Node)
private:
	static const std::string ID;

protected:
	/**
	 * @brief Bind methods of ComputeBudget Node.
	 */
	static void _bind_methods();

public:
	static const int COMPUTE_UNIT_LIMIT_DATA_SIZE = 5; ///< Data length of a compute unit limit instruction.
	static const int COMPUTE_UNIT_PRICE_DATA_SIZE = 9; ///< Data length of a compute price limit instruction.
	static const int UNIT_LIMIT_DISCRIMINATOR = 2; ///< Discriminator of a compute unit limit instruction.
	static const int UNIT_PRICE_DISCRIMINATOR = 3; ///< Discriminator of a compute unit price instruction.

	/**
	 * @brief Create a compute unit limit instruction.
	 *
	 * @param unit_limit Unit limit.
	 * @return Variant Compute unit limit Instruction.
	 */
	static Variant set_compute_unit_limit(const uint32_t unit_limit);

	/**
	 * @brief Create a compute unit price instruction.
	 *
	 * @param unit_price compute price.
	 * @return Variant Compute unit Instruction.
	 */
	static Variant set_compute_unit_price(const uint32_t unit_price);

	/**
	 * @brief Get the Compute Budget program ID.
	 *
	 * @return Variant Program ID Pubkey.
	 */
	static Variant get_pid();
};

} //namespace godot

#endif