#ifndef GODOT_SOLANA_SDK_MERGED_ACCOUNT_METAS_HPP
#define GODOT_SOLANA_SDK_MERGED_ACCOUNT_METAS_HPP
#include <account_meta.hpp>
#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>

namespace godot {

class MergedAccountMetas {
private:
	TypedArray<AccountMeta> merged_metas;

	static Variant preferred_signer(const Variant &left, const Variant &right);
	
	void merge_at(int64_t index, const Variant& account_meta);

protected:
public:
	MergedAccountMetas() = default;
	void add(const Variant &account_meta);
	void from_instructions(const TypedArray<Instruction> &instructions);
	void sort();

	bool is_key_at_index(const Variant &key, uint64_t index) const;

	int64_t find(const AccountMeta &account_meta) const;
	TypedArray<AccountMeta> get_list() const;
	Array get_signers() const;
	void supply_signers(const Array& signers);

	~MergedAccountMetas() = default;
};
} //namespace godot

#endif