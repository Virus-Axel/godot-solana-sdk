#ifndef GODOT_SOLANA_SDK_COMPILED_INSTRUCTION_HPP
#define GODOT_SOLANA_SDK_COMPILED_INSTRUCTION_HPP

#include <cstdint>

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"

#include "instruction.hpp"
#include "merged_account_metas.hpp"
#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Stores compiled instruction information.
 */
class CompiledInstruction : public Object {
	GDCLASS_CUSTOM(CompiledInstruction, Object)

protected:
	/**
	 * @brief Bind methods of CompiledInstruction Object.
	 */
	static void _bind_methods();

public:
	// TODO(VirAx): Make these private with getters and setters.
	// NOLINTBEGIN(misc-non-private-member-variables-in-classes)
	uint8_t program_id_index = 0; ///< Index of the program ID.
	PackedByteArray accounts; ///< Account indicies.
	PackedByteArray data; ///< Data bytes.
	// NOLINTEND(misc-non-private-member-variables-in-classes)

	CompiledInstruction();

	/**
	 * @brief Reconstruct this object from a byte array.
	 *
	 * @param bytes Serialized CompiledInstruction.
	 * @return int Read bytes in reconstruction.
	 */
	uint16_t create_from_bytes(const PackedByteArray &bytes);

	/**
	 * @brief Compile an instruction.
	 *
	 * This method arranges instruction data and merged accounts int a compiled instruction
	 * format.
	 *
	 * @param instruction Instruction to compile.
	 * @param merged_metas Merged account metas.
	 */
	void compile(const Instruction *instruction, const MergedAccountMetas &merged_metas);

	/**
	 * @brief Check if compiled instruction is compute limit instruction.
	 *
	 * Checks if compiled instruction together with the merged account metas is a
	 * compute limit instruction.
	 *
	 * @param merged_metas Merged account metas.
	 * @return true if instruction is a compute limit instruction.
	 * @return false otherwise.
	 */
	[[nodiscard]] bool is_compute_limit(const MergedAccountMetas &merged_metas) const;

	/**
	 * @brief Check if compiled instruction is compute price instruction.
	 *
	 * Checks if compiled instruction together with the merged account metas is a
	 * compute price instruction.
	 *
	 * @param merged_metas Merged account metas.
	 * @return true if instruction is a compute price instruction.
	 * @return false otherwise.
	 */
	[[nodiscard]] bool is_compute_price(const MergedAccountMetas &merged_metas) const;

	/**
	 * @brief Serialize compiled instruction.
	 *
	 * @return PackedByteArray Serialized bytes.
	 */
	[[nodiscard]] PackedByteArray serialize() const;

	/**
	 * @brief Set the data directly.
	 *
	 * @param data Compiled instruction bytes.
	 */
	void set_data(const PackedByteArray &data);

	/**
	 * @brief Get the raw data.
	 *
	 * @return PackedByteArray Raw instruction data.
	 */
	[[nodiscard]] PackedByteArray get_data() const;

	~CompiledInstruction() override = default;
};
} //namespace godot

#endif