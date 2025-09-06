#ifndef SOLANA_SDK_ACCOUNT_HPP
#define SOLANA_SDK_ACCOUNT_HPP

#include <cstdint>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Account data storage class.
 *
 * Class meant to store the contents of a solana account.
 * @todo Implement a real functionality for this class.
 * @warning WIP, do not use.
 */
class Account : public Node {
	GDCLASS_CUSTOM(Account, Node)

private:
	void *data_pointer = nullptr;

protected:
	/**
	 * @brief Binds methods for Account Node.
	 */
	static void _bind_methods();

public:
	Account() = default;

	/**
	 * @brief Does nothing
	 *
	 * @todo Implement if needed, otherwise remove.
	 *
	 * @param lamports Not used
	 * @param space Not used
	 * @param owner Not used
	 */
	void create_new(uint64_t lamports, uint64_t space, const Variant &owner);

	~Account() override = default;
};
} //namespace godot

#endif