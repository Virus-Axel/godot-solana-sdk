#ifndef SOLANA_SDK_KEYPAIR_HPP
#define SOLANA_SDK_KEYPAIR_HPP


#include <godot_cpp/classes/resource.hpp>
#include "utils.hpp"
#include "ed25519.h"


namespace godot{

const int SIGNATURE_LENGTH = 64;

/**
 * @class Keypair
 * @brief An ed25519 Keypair
 *
 * The Keypair is used to sign Solana transactions. 
 * They can be used to keep control of Solana accounts.
 * The Keypair is a signer, and signing a transaction gives the
 * smart contract authority to modify the account data and balances.
 */
class Keypair : public Resource {
    GDCLASS(Keypair, Resource)

private:

    const int PUBLIC_KEY_LENGTH = 32;
    const int PRIVATE_KEY_LENGTH = 64;
    const int SEED_LENGTH = 32;

    bool unique = true;
    String public_string = "";
    PackedByteArray public_bytes;

    String private_string = "";
    PackedByteArray private_bytes;

    PackedByteArray seed;

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    Keypair();
    Keypair(const PackedByteArray &seed);
    
    /**
     * @godot
     * @brief Creates a new Keypair from a seed string.
     * 
     * Takes a seed string and creates an ed25519 keypair using the ascii byte buffer as a seed during the generation.
     * The function requires a 32 character long seed.
     * 
     * @param seed A 32 character long seed string.
     * @return A new Keypair resource created from seed.
     */
    static Variant new_from_seed(const String &seed);
    /**
     * @godot
     * @brief Creates a new Keypair from a seed byte array.
     * 
     * Takes a seed string and creates an ed25519 keypair using the ascii byte buffer as a seed during the generation.
     * The function requires a byte array with a size of 32.
     * 
     * @param seed A 32 byte long byte array seed.
     * @return A new Keypair resource created from seed.
     */
    static Variant new_from_seed(const PackedByteArray &seed);
    /**
     * @godot
     * @brief Creates a Keypair from a byte array.
     * 
     * Takes a byte array of an ed25519 seed followed by public bytes.
     * The function expects this format which must be 64 bytes long.
     * This format is the common format that can be found in exported keypairs by phantom or Solana CLI.
     * 
     * @param bytes The byte array of size 64 containing ed25519 seed and public bytes.
     * @return A new Keypair resource created from seed.
     */
    static Variant new_from_bytes(const Variant &bytes);
    /**
     * @godot
     * @brief Creates a new Keypair resource from a file containing an exported keypair.
     * 
     * Takes a filename and imports the contents and reads a new Keypair. The function expects
     * a JSON format describing an array of 64 bytes. The first 32 bytes is the Keypair seed
     * and the last 32 bytes are the public key bytes. 
     * 
     * @param filename The name of the JSON file to load a Keypair from.
     * @return A new Keypair imported from the file.
     */
    static Variant new_from_file(const String &filename);
    /**
     * @godot
     * @brief Creates a new random Keypair.
     * 
     * Generates a randomized private key and finds the corresponding public key
     * and returns this Keypair.
     * 
     * @warning The random number generation might be predictable. 
     * 
     * @return The randomly generated Keypair. 
     */
    static Variant new_random();
    /**
     * @brief Checks if the provided Variant is a Keypair.
     * 
     * Takes a Variant and checks if it is a Keypair resource.
     * 
     * @param object The Variant to check.
     * @return true if the variant is a Keypair resource, false otherwise.
     */
    static bool is_keypair(const Variant& object);

    /**
     * @brief Creates a new Pubkey resource from the public parts of the Keypair.
     * 
     * @return A Pubkey resource from the public parts of the Keypair.
     */
    Variant to_pubkey();

    /**
     * @brief Set the base58 encoded string representing the public parts.
     * 
     * @param p_value New value of base58 encoded String.
     */
    void set_public_string(const String& p_value);
    /**
     * @brief Get the base58 encoded string representing the public parts.
     * 
     * @return base58 encoded string representing the public parts. 
     */
    String get_public_string();

    /**
     * @brief Set the public parts from a byte array.
     * 
     * Takes a 32 byte long array and sets the public parts of the Keypair.
     * This method updates the public bytes and the base58 encoded representation.
     * 
     * @param p_value New value of the public parts byte array.
     */
    void set_public_bytes(const PackedByteArray& p_value);
    /**
     * @brief Get the byte array representing the public parts.
     * 
     * @return PackedByteArray array representing the public parts.
     */
    PackedByteArray get_public_bytes() const;

    /**
     * @brief Set the the private parts of the Keypair from a base58 encoded string representation.
     * 
     * Takes a base58 encoded String of a 64 byte array and sets the private parts of the Keypair.
     * This method updates the private bytes and the base58 encoded representation.
     * 
     * @param p_value A base58 encoded String of a 64 byte array.
     */
    void set_private_string(const String& p_value);
    /**
     * @brief Get the base58 encoded string representing the private parts.
     * 
     * @return base58 encoded string representing the private parts. 
     */
    String get_private_string();

    /**
     * @brief Set the private parts from a byte array.
     * 
     * Takes a 64 byte long array and sets the private parts of the Keypair.
     * This method updates the private bytes and the base58 encoded representation.
     * 
     * @param p_value New value of the private parts byte array.
     */
    void set_private_bytes(const PackedByteArray& p_value);
    /**
     * @brief Get the byte array representing the private parts.
     * 
     * @return PackedByteArray array representing the private parts.
     */
    PackedByteArray get_private_bytes();

    /**
     * @brief Sign the provided message.
     * 
     * Signs the provided message and returns a 64 byte signature.
     * 
     * @param message The message to sign.
     * @return PackedByteArray 64 byte signature.
     */
    PackedByteArray sign_message(const PackedByteArray& message);
    /**
     * @brief Checks if the message signature is from this Keypair.
     * 
     * @param signature 64 byte long byte array signature.
     * @param message PackedByteArray of the signed message.
     * @return true if the Keypair was used to sign the Keypair, false otherwise.
     */
    bool verify_signature(const PackedByteArray& signature, const PackedByteArray& message);

    /**
     * @brief Set Keypair unique indicator. 
     * 
     * @deprecated The Keypair types is not used anymore. Use new_random instead.
     * 
     * @param p_value Sets unique indicator.
     */
    void set_unique(const bool p_value);
    /**
     * @brief Get Keypair unique indicator.
     * 
     * @return true if the Keypair is unique type, false otherwise.
     */
    bool get_unique();

    /**
     * @brief Set the ed25519 seed.
     * 
     * @param p_value 32 byte seed.
     */
    void set_seed(const PackedByteArray &p_value);
    /**
     * @brief Get the ed25519 seed.
     * 
     * @return 32 byte long PackedByteArray seed.
     */
    PackedByteArray get_seed();

    /**
     * @brief Constructs this keypair randomly.
     * 
     */
    void random();
    /**
     * @brief Constructs this keypair from the internal seed property.
     * 
     */
    void from_seed();

    /**
     * @godot
     * @brief Exports this Keypair to a file.
     * 
     * Exports this Keypair to a file with the provided Keypair.
     * This function will fail if the Keypair does not have track of the ed25519 seed.
     * 
     * @param filename Filename of the exported Keypair.
     */
    void save_to_file(const String &filename);

    ~Keypair();
};
}

#endif