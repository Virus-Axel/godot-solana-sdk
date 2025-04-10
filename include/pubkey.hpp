#ifndef SOLANA_SDK_PUBKEY_HPP
#define SOLANA_SDK_PUBKEY_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>

namespace godot {

const unsigned int PUBKEY_LENGTH = 32;

/**
 * @class Pubkey
 * @brief The address of a Solana account.
 *
 * A 32 byte key that represents a Solana account address. It can be used for various things.
 * The Pubkey class stores the 32 bytes and also a base58 encoded representation of the bytes.
 */
class Pubkey : public Resource {
	GDCLASS(Pubkey, Resource)

private:
	// Number of bytes in a pubkey
	static const unsigned int PUBKEY_BYTES = 32;
	// maximum length of derived `Pubkey` seed
	const unsigned int MAX_SEED_LEN = 32;
	// Maximum number of seeds
	const unsigned int MAX_SEEDS = 16;
	// Maximum string length of a base58 encoded pubkey
	const unsigned int MAX_BASE58_LEN = 44;

	const unsigned char PDA_MARKER[22] = "ProgramDerivedAddress";

	String type = "UNIQUE";
	String seed = "";
	String value = "";
	PackedByteArray bytes;

	Variant base;
	Variant owner;

	Variant wallet_address;
	Variant token_mint_address;

	bool are_bytes_curve_point() const;
	static bool is_variant_valid_key(const Variant &variant);

protected:
	/**
	 * @brief @bindmethods{Pubkey, Resource}
	 */
	static void _bind_methods();

	/**
	 * @_set{Pubkey}
	 */
	bool _set(const StringName &p_name, const Variant &p_value);

	/**
	 * @_get{Pubkey}
	 */
	bool _get(const StringName &p_name, Variant &r_ret) const;
	
	/**
	 * @_get_property_list
	 */
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
	/**
	 * @brief Constructor for Pubkey.
	 *
	 * Creates an empty invalid Pubkey with empty byte array.
	 */
	Pubkey();

	/**
	 * @brief Constructor for Pubkey.
	 *
	 * Creates a Pubkey with 32 bytes from the base58 encoded string.
	 * If the decoded bytes length is not 32 an error will be emitted.
	 * In that case an empty Pubkey is created instead.
	 */
	Pubkey(const String &from);

	/**
	 * @brief Constructor for Pubkey.
	 *
	 * Creates an empty invalid Pubkey with empty byte array.
	 */
	Pubkey(const Variant &from);

	/**
	 * @godot
	 * @brief Configure Pubkey from base58 encoded string.
	 *
	 * Decodes p_value into 32 bytes key and configures the Pubkey properties.
	 * If the decoded bytes length is not 32 an error will be emitted.
	 *
	 * @param p_value String of the base58 encoded address.
	 */
	void from_string(const String &p_value);

	/**
	 * @godot
	 * @brief Returns the base58 encoded key.
	 *
	 * @return Base58 encoded key.
	 */
	String to_string() const;

	/**
	 * @godot
	 * @brief Sets an unused seed property.
	 *
	 * @deprecated Try either new_from_bytes or new_from_seed instead.
	 *
	 * @param p_value Value to set.
	 */
	void set_seed(const String &p_value);

	/**
	 * @godot
	 * @brief Gets an unused seed property.
	 *
	 * @deprecated Seed is not used.
	 *
	 * @return Seed String
	 */
	String get_seed();

	/**
	 * @godot
	 * @brief Configure Pubkey from 32 byte array.
	 *
	 * Configures the Pubkey properties from a 32 byte array.
	 * If the byte array length is not 32 an error will be emitted.
	 *
	 * @param p_value 32 byte array.
	 */
	void from_bytes(const PackedByteArray &p_value);

	/**
	 * @godot
	 * @brief Returns the 32 byte key array.
	 *
	 * @return The 32 byte key array.
	 */
	PackedByteArray to_bytes() const;

	/**
	 * @godot
	 * @brief Set type of Pubkey.
	 *
	 * @deprecated Type is not used anything.
	 *
	 * @param p_value Type of Pubkey.
	 */
	void set_type(const String p_value);

	/**
	 * @godot
	 * @brief Gets Pubkey type.
	 *
	 * @deprecated Type is not used anything.
	 *
	 * @return Type of Pubkey.
	 */
	String get_type() const;

	/**
	 * @godot
	 * @brief Set base of Pubkey.
	 *
	 * Used when creating Pubkeys from seeds.
	 *
	 * @deprecated Use new_from_seed instead.
	 *
	 * @param p_value Base Pubkey to derive Pubkey from.
	 */
	void set_base(const Variant p_value);

	/**
	 * @godot
	 * @brief Get base property of Pubkey.
	 *
	 * Gets the base used when creating Pubkeys from seeds.
	 *
	 * @deprecated Does not really have a purpose.
	 *
	 * @return Base Pubkey property.
	 */
	Variant get_base();

	/**
	 * @godot
	 * @brief Set owner of Pubkey.
	 *
	 * Sets owner when generating the pubkey from seed.
	 *
	 * @deprecated Use new_from_seed instead.
	 *
	 * @param p_value owner of the Pubkey.
	 */
	void set_owner(const Variant p_value);

	/**
	 * @godot
	 * @brief Gets the owner of the Pubkey.
	 *
	 * Gets the owner used when generating pubkey from seed.
	 *
	 * @deprecated Does not have a purpose. Owner cannot be reversed anymore.
	 *
	 * @return Owner of the Pubkey.
	 */
	Variant get_owner();

	/**
	 * @godot
	 * @brief Set the wallet address object.
	 *
	 * Sets the wallet address used when generating an associated token address.
	 *
	 * @deprecated The owner is not stored internally anymore.
	 *
	 * @param p_value Wallet address to set.
	 */
	void set_wallet_address(const Variant p_value);

	/**
	 * @godot
	 * @brief Get the wallet address object
	 *
	 * Gets the wallet address used when derriving associated token address.
	 *
	 * @deprecated The wallet address is not stored internally anymore.
	 *
	 * @return Wallet address used when derriving associated token address.
	 */
	Variant get_wallet_address();

	/**
	 * @godot
	 * @brief Set the token mint address object
	 *
	 * Sets the token mint address used when generating associated token accounts.
	 *
	 * @deprecated Use new_associated_token_address instead.
	 *
	 * @param p_value token mint used when generating associated token
	 */
	void set_token_mint_address(const Variant p_value);

	/**
	 * @godot
	 * @brief Get the token mint address object
	 *
	 * Gets the token mint address used when generating associated token accounts.
	 *
	 * @deprecated The token mint address is not stored internally anymore.
	 *
	 * @return Variant containting the token mint Pubkey resource.
	 */
	Variant get_token_mint_address();

	/**
	 * @godot
	 * @brief Constructs this Pubkey resource from string object.
	 *
	 * Takes a base58 encoded string and constructs this Pubkey resource from it.
	 * The base58 encoded string must represent a 32 byte sized array.
	 *
	 * @param from A base58 encoded string of an account address.
	 */
	void create_from_string(const String &from);

	/**
	 * @brief Constructs the Pubkey resource from a byte array.
	 *
	 * Takes a pointer and reads 32 bytes and constructs this Pubkey resource from the bytes.
	 *
	 * @param data Pointer to byte array.
	 */
	void create_from_array(const unsigned char *data);

	/**
	 * @godot
	 * @brief Creates a new Pubkey resource from seed.
	 *
	 * Takes a base address and derives a new Pubkey from a seed and an owner address.
	 *
	 * @param basePubkey Base address of the derived Pubkey. Accepts WalletAdapter, Keypair or a Pubkey.
	 * @param seed Text seed to use when derriving the new Pubkey.
	 * @param owner_pubkey Owner address of the derived Pubkey. Accepts WalletAdapter, Keypair or a Pubkey.
	 * @return A new derived Pubkey resource.
	 */
	static Variant new_from_seed(Variant basePubkey, String seed, Variant owner_pubkey);

	/**
	 * @godot
	 * @brief Creates a new Pubkey from string
	 *
	 * Takes a base58 encoded string and creates a new Pubkey resource from it.
	 * The base58 encoded string must represent a 32 byte sized array.
	 *
	 * @param from A base58 encoded string of an account address.
	 * @return Variant
	 */
	static Variant new_from_string(const String &from);

	/**
	 * @godot
	 * @brief Creates a new Pubkey resource from a PackedByteArray.
	 *
	 * Takes a 32 byte long PackedByteArray and creates a new Pubkey resource from it.
	 *
	 * @param from A 32 byte sized byte array.
	 * @return A new Pubkey resource.
	 */
	static Variant new_from_bytes(const PackedByteArray &from);

	/**
	 * @brief Creates a new Pubkey from a Variant
	 *
	 * Checks the Varaint for any compatible type and creates a new object accordingly.
	 *
	 * @param from Variant to create the Pubkey from.
	 * @return Variant containing the pubkey.
	 * @return nullptr if Variant is not compatible.
	 */
	static Variant new_from_variant(const Variant &from);

	/**
	 * @godot
	 * @brief Creates a new program derrived Pubkey resource.
	 *
	 * Creates a program derrived address from an array of seed strings.
	 * Unlike new_pda, this function does not try to find a valid program derived address.
	 *
	 * @param seeds An array of seed strings.
	 * @param program_id The program ID to derrive from.
	 * @return A new program derrived Pubkey resource.
	 */
	static Variant new_program_address(const PackedStringArray seeds, const Variant &program_id);

	/**
	 * @godot
	 * @brief Creates a new associated token address.
	 *
	 * Takes a mint address and a wallet address and returns the associated token address.
	 *
	 * @param wallet_address Wallet address to associate token address from. Can be Keypair, Pubkey and WalletAdapter.
	 * @param token_mint_address Token mint address. Can be Keypair, Pubkey and WalletAdapter.
	 * @return A new associated token account Pubkey resource.
	 */
	static Variant new_associated_token_address(const Variant &wallet_address, const Variant &token_mint_address);

	/**
	 * @godot
	 * @brief Creates a new valid program derrived Pubkey resource.
	 *
	 * Creates a program derrived address from an array of seed strings.
	 * Unlike new_program_address, this function tries to find a valid program derrived address.
	 * If the function is unsuccessful, the function will return a null Variant and trigger a warning.
	 *
	 * @param seeds An array of seed strings.
	 * @param program_id The program ID to derrive from.
	 * @return A new valid program derrived Pubkey resource. Null Variant if unsuccessful.
	 */
	static Variant new_pda(const PackedStringArray seeds, const Variant &program_id);

	/**
	 * @godot
	 * @brief Same as new_pda but with a byte array of seeds.
	 *
	 * Creates a program derrived address from an array of byte arrays.
	 * Unlike new_program_address, this function tries to find a valid program derrived address.
	 * If the function is unsuccessful, the function will return a null Variant and trigger a warning.
	 *
	 * @param seeds An array of seed byte arrays.
	 * @param program_id The program ID to derrive from.
	 * @return A new valid program derrived Pubkey resource. Null Variant if unsuccessful.
	 */
	static Variant new_pda_bytes(const Array seeds, const Variant &program_id);

	/**
	 * @godot
	 * @brief Creates a new random Pubkey resource.
	 *
	 * Creates a new random Pubkey by filling each byte randomly.
	 *
	 * @note The random generator might be predictable.
	 *
	 * @return A new random Pubkey resource.
	 */
	static Variant new_random();

	/**
	 * @brief Determines if a Variant is a Pubkey resource.
	 *
	 * This function returns true if the provided Variant is of type Pubkey, false otherwise.
	 *
	 * @param object The Variant to check.
	 * @return true if provided Variant is a Pubkey, false otherwise.
	 */
	static bool is_pubkey(const Variant &object);

	/**
	 * @brief Returns the public bytes of a key.
	 *
	 * @param other Either a Keypair, Pubkey or a WalletAdapter to get the bytes from.
	 * @return PackedByteArray The public bytes of a key.
	 */
	static PackedByteArray bytes_from_variant(const Variant &other);

	/**
	 * @brief Same as bytes_from_variant but returns the base58 encoded string.
	 *
	 * @param other Either a Keypair, Pubkey or a WalletAdapter to get the string from.
	 * @return PackedByteArray The public base58 encoded string of a key.
	 */
	static String string_from_variant(const Variant &other);

	/**
	 * @godot
	 * @brief Constructs a program address object
	 *
	 * Constructs this Pubkey into a program derived address.
	 * Also checks if the address is valid and returns true if it is, false otherwise.
	 *
	 * @param seeds Array of seed strings to use when deriving address.
	 * @param program_id Program id of the program to derrive address from.
	 * @return true if the constructed Pubkey is valid, false otherwise.
	 */
	bool create_program_address(const PackedStringArray seeds, const Variant &program_id);

	/**
	 * @brief Same as create_program_address but with an array of byte arrays seeds.
	 *
	 * Constructs this Pubkey into a program derived address.
	 * Also checks if the address is valid and returns true if it is, false otherwise.
	 *
	 * @param seeds Array of seed byte arrays to use when deriving address.
	 * @param program_id Program id of the program to derrive address from.
	 * @return true if the constructed Pubkey is valid, false otherwise.
	 */
	bool create_program_address_bytes(const Array seeds, const Variant &program_id);

	/**
	 * @godot
	 * @brief Constructs an associated token address.
	 *
	 * Takes a wallet address and token mint address to get an associated token account.
	 *
	 * @note This function does not try to find a valid associated token address.
	 *
	 * @param wallet_address Wallet address to associate token address from. Can be Keypair, Pubkey and WalletAdapter.
	 * @param token_mint_address Token mint address. Can be Keypair, Pubkey and WalletAdapter.
	 * @return true if the constructed Pubkey is valid, false otherwise.
	 */
	bool get_associated_token_address(const Variant &wallet_address, const Variant &token_mint_address);

	/**
	 * @brief Assign this Pubkey from a Variant.
	 * 
	 * @param other Pubkey compatible key Variant.
	 */
	void operator=(const Variant &other);

	/**
	 * @brief Compare two pubkeys.
	 * 
	 * @param other Pubkey to compare to.
	 * @return true If Pubkeys are equal.
	 * @return false If Pubkeys are not equal.
	 */
	bool operator==(const Pubkey &other) const;

	~Pubkey();
};
} //namespace godot

#endif