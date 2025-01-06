#ifndef GODOT_SOLANA_SDK_COMPILED_INSTRUCTION_HPP
#define GODOT_SOLANA_SDK_COMPILED_INSTRUCTION_HPP
#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>
#include <merged_account_metas.hpp>

namespace godot {

class CompiledInstruction : public Object {
	GDCLASS(CompiledInstruction, Object)

protected:
	static void _bind_methods();

public:
	uint8_t program_id_index = 0;
	PackedByteArray accounts;
	PackedByteArray data;

	CompiledInstruction();
	int create_from_bytes(const PackedByteArray &bytes);
	void compile(const Instruction *instruction, const MergedAccountMetas &merged_metas);

	bool is_compute_limit(const MergedAccountMetas &merged_metas) const;
	bool is_compute_price(const MergedAccountMetas &merged_metas) const;

	PackedByteArray serialize();
    void set_data(const PackedByteArray& data);
    PackedByteArray get_data() const;

	~CompiledInstruction();
};
} //namespace godot

#endif