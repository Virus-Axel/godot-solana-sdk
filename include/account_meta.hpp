#ifndef SOLANA_SDK_ACCOUNT_META_HPP
#define SOLANA_SDK_ACCOUNT_META_HPP

#include "godot_cpp/classes/node.hpp"

#include "pubkey.hpp"
#include "utils.hpp"

namespace godot {

/**
 * @brief Contains account key and signer and writable information.
 *
 * Account metas are mainly used when building instructions. Instructions need to know if the
 * accounts are writable and signers. This AccountMeta is different from other implementations
 * in that it also keeps the signing object. Transactions can later make use of this object
 * when signing for example.
 */
class AccountMeta : public Resource {
	GDCLASS(AccountMeta, Resource)

private:
	bool is_signer = false;
	bool writeable = false;

	Variant key;

protected:
	/**
	 * @brief Binds methods of AccountMeta Resource.
	 */
	static void _bind_methods();

	/**
	 * @_set{AccountMeta}
	 */
	bool _set(const StringName &p_name, const Variant &p_value);

	/**
	 * @brief Gets property of AccountMeta.
	 *
	 * @param p_name Property to get.
	 * @param r_ret Variant to write property value to.
	 * @return true if operation is successful.
	 * @return false if operation failed.
	 */
	bool _get(const StringName &p_name, Variant &r_ret) const;

public:
	AccountMeta();

	/**
	 * @brief Construct a new Account Meta object from components.
	 *
	 * @param pid Account key.
	 * @param signer If key is supposed to sign.
	 * @param writeable If account is writeable.
	 */
	AccountMeta(const Variant &pid, bool signer, bool writeable);

	/**
	 * @brief Construct a new Account Meta object from another AccountMeta.
	 *
	 * @param other Other AccountMeta to copy.
	 */
	AccountMeta(const Variant &other);

	/**
	 * @brief Set key part of object.
	 *
	 * @todo Adjust method name, might be same as get_key.
	 *
	 * @param p_value Key value.
	 */
	void set_pubkey(const Variant &p_value);

	/**
	 * @brief Get the key object's Pubkey part.
	 *
	 * @return Variant Pubkey part of AccountMeta's key component.
	 */
	Variant get_pubkey() const;

	/**
	 * @brief Gets key or a keypair object.
	 *
	 * @warning Do not use.
	 * @todo Consider removing this.
	 *
	 * @return Variant key or Keypair.
	 */
	Variant get_signer() const;

	/**
	 * @brief Set the key component of the AccountMeta.
	 *
	 * @param p_value Key value.
	 */
	void set_key(const Variant &p_value);

	/**
	 * @brief Get the key component of the AccountMeta.
	 *
	 * @return Variant Key component.
	 */
	Variant get_key() const;

	/**
	 * @brief Set the is signer property.
	 *
	 * @param p_value is signer value.
	 */
	void set_is_signer(const bool p_value);

	/**
	 * @brief Get the is signer property.
	 *
	 * @return true if AccountMeta is signer.
	 * @return false if AccountMeta is not signer
	 */
	bool get_is_signer() const;

	/**
	 * @brief Set the writeable property
	 *
	 * @param p_value writable property value.
	 */
	void set_writeable(const bool p_value);

	/**
	 * @brief Get the writeable property.
	 *
	 * @return true if AccountMeta is writeable.
	 * @return false if AccountMeta is readonly.
	 */
	bool get_writeable() const;

	/**
	 * @brief Constructs this AccountMeta from components.
	 *
	 * @note This does not allocate any new memory.
	 * @param account_key Key component.
	 * @param is_signer Specifies if key is signer.
	 * @param writeable Specifies if key is writeable.
	 */
	void create_new(const Variant &account_key, bool is_signer, bool writeable);

	/**
	 * @brief Checks if Variant is of type AccountMeta object.
	 *
	 * @param other Variant to check.
	 * @return true if Variant is of type AcccountMeta object.
	 * @return false if Variant is not an AccountMeta object.
	 */
	static bool is_account_meta(const Variant &other);

	/**
	 * @brief Allocates a new AccountMeta object from components.
	 *
	 * @param account_key Key of AccountMeta.
	 * @param is_signer Specifies if key is signer.
	 * @param writeable Specifies if key is writeable.
	 * @return Variant newly created AccountMeta.
	 */
	static Variant new_account_meta(const Variant &account_key, bool is_signer, bool writeable);

	/**
	 * @brief Checks equality of AccountMeta's keys.
	 *
	 * This method checks equality of only the public part of the key objects.
	 *
	 * @param other AccountMeta to check.
	 * @return true if keys are equal.
	 * @return false if keys are not equal.
	 */
	bool operator==(const AccountMeta &other) const;

	~AccountMeta();
};
} //namespace godot

#endif