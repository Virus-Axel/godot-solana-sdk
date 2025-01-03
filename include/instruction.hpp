#ifndef SOLANA_SDK_INSTRUCTION_HPP
#define SOLANA_SDK_INSTRUCTION_HPP

#include "account_meta.hpp"
#include "hash.hpp"
#include "pubkey.hpp"
#include "utils.hpp"
#include <vector>

#include <godot_cpp/classes/node.hpp>

namespace godot {

class Instruction : public Resource {
	GDCLASS(Instruction, Resource)

private:
	Variant program_id;
	PackedByteArray data;
	TypedArray<AccountMeta> accounts;

protected:
	static void _bind_methods();

public:
	Instruction();

	void set_program_id(const Variant &p_value);
	Variant get_program_id() const;

	void set_data(const PackedByteArray &p_value);
	PackedByteArray get_data() const;

	void set_accounts(const TypedArray<AccountMeta> &p_value);
	TypedArray<AccountMeta> get_accounts() const;

	void append_meta(const AccountMeta &meta);

	PackedByteArray serialize();
	bool is_serializable();

	~Instruction();
};

} //namespace godot

#endif