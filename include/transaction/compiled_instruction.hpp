#ifndef GODOT_SOLANA_SDK_COMPILED_INSTRUCTION_HPP
#define GODOT_SOLANA_SDK_COMPILED_INSTRUCTION_HPP
#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>
#include <merged_account_metas.hpp>

namespace godot {

/**
 * @brief Stores compiled instruction information.
 */
class CompiledInstruction : public Object {
	GDCLASS(CompiledInstruction, Object)

protected:
	/**
	 * @brief Bind methods of CompiledInstruction Object.
	 */
	static void _bind_methods();

public:
	uint8_t program_id_index = 0; ///< Index of the program ID.
	PackedByteArray accounts; ///< Account indicies.
	PackedByteArray data; ///< Data bytes.

	CompiledInstruction();

	/**
	 * @brief Reconstruct this object from a byte array.
	 * 
	 * @param bytes Serialized CompiledInstruction.
	 * @return int Read bytes in reconstruction.
	 */
	int create_from_bytes(const PackedByteArray &bytes);

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
	bool is_compute_limit(const MergedAccountMetas &merged_metas) const;

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
	bool is_compute_price(const MergedAccountMetas &merged_metas) const;

	/**
	 * @brief Serialize compiled instruction.
	 * 
	 * @return PackedByteArray Serialized bytes.
	 */
	PackedByteArray serialize();

	/**
	 * @brief Set the data directly.
	 * 
	 * @param data Compiled instruction bytes.
	 */
    void set_data(const PackedByteArray& data);

	/**
	 * @brief Get the raw data.
	 * 
	 * @return PackedByteArray Raw instruction data.
	 */
    PackedByteArray get_data() const;

	~CompiledInstruction();
};
} //namespace godot

#endif