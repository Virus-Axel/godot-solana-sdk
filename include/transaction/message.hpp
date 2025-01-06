#ifndef SOLANA_SDK_MESSAGE_HPP
#define SOLANA_SDK_MESSAGE_HPP

#include <compiled_instruction.hpp>
#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>

#include "keypair.hpp"

namespace godot {

const int64_t BLOCKHASH_LENGTH = 32;

class Message {
private:
	uint8_t payer_index = 0;
	uint8_t num_required_signatures = 0;
	uint8_t num_readonly_signed_accounts = 0;
	uint8_t num_readonly_unsigned_accounts = 0;
	TypedArray<Pubkey> account_keys;
	String latest_blockhash = "";
	TypedArray<CompiledInstruction> compiled_instructions;
	MergedAccountMetas meta_list;

	bool is_versioned_transaction = false;

	Array address_lookup_tables;

	void compile_instruction(Variant instruction);
	void recalculate_headers();
	void read_accounts_from_bytes(const PackedByteArray bytes, uint8_t num_accounts, bool is_signer, bool is_writable, int64_t &cursor);

	bool is_versioned();

protected:
public:
	Message() = default;
	void create(const MergedAccountMetas &merged_meta_list, Variant &payer);
	void create(const PackedByteArray &bytes);
	void set_latest_blockhash(const String &blockhash);

	PackedByteArray serialize();
	PackedByteArray serialize_blockhash();
	PackedByteArray serialize_lookup_tables();

	TypedArray<CompiledInstruction> get_compiled_instructions() const;

	int get_amount_signers() const;
	Array get_signers();
	void set_address_lookup_tables(const Array &address_lookup_tables);
	Array get_address_lookup_tables();
	void compile_instructions(const TypedArray<Instruction> &instructions);
	void supply_signers(const Array &signers);
	~Message() = default;
};

} //namespace godot

#endif