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

	void merge_at(const AccountMeta *account_meta, int64_t index);

protected:
public:
	MergedAccountMetas() = default;

	/**
	 * @brief Resets the account meta list.
	 */
	void clear();

	/**
	 * @brief Adds an account meta into merged list.
	 *
	 * This method adds a new entry to its merged account meta list if address is
	 * not already there. If the address is present, then the writable and signer
	 * properties are modified to have lowest permision and signing level.
	 *
	 * @param account_meta account meta to add.
	 */
	void add(const Variant &account_meta);

	/**
	 * @brief Reads accounts from a list of instructions.
	 *
	 * This method takes all account metas in all instructions in the providied list and
	 * merges them with with the internal list within the class.
	 *
	 * @param instructions A list of instructions.
	 */
	void from_instructions(const TypedArray<Instruction> &instructions);

	/**
	 * @brief Sorts the list of account metas.
	 *
	 * This method sorts the account metas in the order of writable signers,
	 * readonly signers, writable non-signers and last readonly non-signers.
	 */
	void sort();

	/**
	 * @brief Compares an account meta address to the one at an index.
	 *
	 * @param key Account to compare.
	 * @param index Index of the account meta to compare with.
	 * @return true if the accounts have the same addresses.
	 * @return false if the account do not have the same addresses.
	 */
	bool is_key_at_index(const Variant &key, int64_t index) const;

	/**
	 * @brief Checks if account meta list is empty.
	 *
	 * @return true if list is empty.
	 * @return false if list is not empty.
	 */
	bool is_empty() const;

	/**
	 * @brief Finds the index of an account meta.
	 *
	 * @param account_meta Account meta to find.
	 * @return Index of account meta
	 * @return -1 if the account meta was not found.
	 */
	int64_t find(const AccountMeta &account_meta) const;

	/**
	 * @brief Get the internal account meta list.
	 *
	 * @return List of account metas.
	 */
	TypedArray<AccountMeta> get_list() const;

	/**
	 * @brief Gets the list of signing key objects.
	 *
	 * Searches the internal account meta list and gets the key object where account
	 * meta is marked as signer. The method returns this list where signer is true.
	 *
	 * @note The list may contain non-signing objects if the signers are not yet supplied.
	 *
	 * @return Array
	 */
	Array get_signers() const;

	/**
	 * @brief Supply missing signers to the account meta list.
	 *
	 * Finds the account metas that matches the addresses of the provided signers.
	 * It then sets the provided signer object for that account meta if needed.
	 *
	 * @param signers List of signers.
	 */
	void supply_signers(const Array &signers);

	~MergedAccountMetas() = default;
};
} //namespace godot

#endif