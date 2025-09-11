#ifndef SOLANA_SDK_INSTRUCTION_HPP
#define SOLANA_SDK_INSTRUCTION_HPP

#include "account_meta.hpp"
#include "solana_utils.hpp"

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {

/**
 * @brief Contains all instruction information used for transactions.
 */
class Instruction : public Resource {
	GDCLASS_CUSTOM(Instruction, Resource)

private:
	Variant program_id;
	PackedByteArray data;
	TypedArray<AccountMeta> accounts;

protected:
	/**
	 * @brief Bind methods of Instructino.
	 */
	static void _bind_methods();

public:
	Instruction() = default;

	/**
	 * @brief Set the program ID of the instruction.
	 *
	 * @param p_value Program ID to set.
	 */
	void set_program_id(const Variant &p_value);

	/**
	 * @brief Get the program ID of the instruction.
	 *
	 * @return Variant Program ID of the instruction.
	 */
	[[nodiscard]] Variant get_program_id() const;

	/**
	 * @brief Set the data property.
	 *
	 * @param p_value Byte array to set.
	 */
	void set_data(const PackedByteArray &p_value);

	/**
	 * @brief Get the data property.
	 *
	 * @return PackedByteArray Data property.
	 */
	[[nodiscard]] PackedByteArray get_data() const;

	/**
	 * @brief Set the accounts property.
	 *
	 * @param p_value Accounts value to set.
	 */
	void set_accounts(const TypedArray<AccountMeta> &p_value);

	/**
	 * @brief Get the accounts property.
	 *
	 * @return TypedArray<AccountMeta> Accounts property.
	 */
	[[nodiscard]] TypedArray<AccountMeta> get_accounts() const;

	/**
	 * @brief Append an AccountMeta to the instruction.
	 *
	 * @param meta AccountMeta to appends.
	 */
	void append_meta(const AccountMeta &meta);

	/**
	 * @brief Serialize the Instruction.
	 *
	 * @return PackedByteArray Serialized Instruction.
	 */
	PackedByteArray serialize();

	/**
	 * @brief Check if Instruction is ready to be serialized.
	 *
	 * @return true if instruction is ready to be serialized.
	 * @return false otherwise.
	 */
	bool is_serializable();

	~Instruction() override = default;
};

} //namespace godot

#endif