#ifndef SOLANA_SDK_ANCHOR_PROGRAM
#define SOLANA_SDK_ANCHOR_PROGRAM

#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_client.hpp"

namespace godot {

const int DISCRIMINATOR_LENGTH = 8;

/**
 * @brief Describes an anchor node from an IDL specification.
 *
 * Can read IDL from accounts on chain or from json files. Can fetch accounts and build
 * instructions based on loaded IDLs. This node is used in custom dynamically created
 * Program nodes.
 */
class AnchorProgram : public Node {
	GDCLASS(AnchorProgram, Node)
private:
	Dictionary idl;
	String pid;
	String url_override = "";
	String pending_account_name = "";
	String pending_accounts_name = "";
	bool try_from_pid = false;
	Variant json_file;
	bool try_from_json_file = false;
	uint32_t accounts_discriminator_length = 8;
	Array instructions;

	SolanaClient *idl_client = nullptr;
	SolanaClient *fetch_client = nullptr;

	static bool is_int(const Variant &var);
	static bool is_float(const Variant &var);
	static bool detect_writable(const Dictionary &account);
	static bool detect_is_signer(const Dictionary &account);
	static bool detect_optional(const Dictionary &account);
	static PackedByteArray evaluate_discriminant(const Dictionary &discriminant_info);
	static Variant idl_address(const Variant &pid);

	bool load_from_pid(const String &pid);
	void idl_from_pid_callback(const Dictionary &rpc_result);
	void fetch_account_callback(const Dictionary &rpc_result);
	void fetch_all_accounts_callback(const Dictionary &rpc_result);
	void extract_idl_from_data(const Array &data_info);
	Dictionary parse_account_data(const Dictionary &account_data, const Dictionary &reference, bool emit_decoded_account = false);

	static PackedByteArray discriminator_by_name(const String &name, const String &namespace_string);
	[[nodiscard]] PackedByteArray get_instruction_discriminant(const Dictionary &instruction_info, const String &name) const;

	Dictionary find_idl_type(const String &name);

	Variant deserialize_variant(const PackedByteArray &bytes, const Variant &type, uint32_t &consumed_bytes);

	static Variant decorate_instruction_argument(const Variant &anchor_type, const Variant &argument);

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

protected:
	/**
	 * @brief Binds methods of AnchorProgram Node.
	 */
	static void _bind_methods();

public:
	AnchorProgram();

	/**
	 * @brief processes the client node components.
	 *
	 * @param delta delta time since last process.
	 */
	void _process(double delta) override;

	/**
	 * @brief Searches an IDL and returns programm address.
	 *
	 * @param idl IDL of the anchor program to search in.
	 * @return Variant Pubkey holding program ID.
	 * @return Variant nullptr if no address is found.
	 */
	static Variant get_address_from_idl(const Dictionary &idl);

	/**
	 * @brief Sets the idl of the anchor program.
	 *
	 * @param idl IDL of the anchor program.
	 */
	void set_idl(const Dictionary &idl);

	/**
	 * @brief Get the idl of the anchor program.
	 *
	 * @return Dictionary IDL of the anchor program.
	 */
	[[nodiscard]] Dictionary get_idl() const;

	/**
	 * @brief Get the accounts discriminator length.
	 * 
	 * @return uint32_t Length of the accounts discriminator.
	 */
	[[nodiscard]] uint32_t get_accounts_discriminator_length() const;

	/**
	 * @brief Set the try from pid property.
	 *
	 * @param try_from_pid property value to set.
	 */
	void set_try_from_pid(bool try_from_pid);

	/**
	 * @brief Get the try from pid property.
	 *
	 * @return bool property value.
	 */
	[[nodiscard]] bool get_try_from_pid() const;

	/**
	 * @brief Set the try from json file property
	 *
	 * @param try_from_json_file property value to set.
	 */
	void set_try_from_json_file(bool try_from_json_file);

	/**
	 * @brief Get the try from json file property
	 *
	 * @return bool property value.
	 */
	[[nodiscard]] bool get_try_from_json_file() const;

	/**
	 * @brief Set the url override property
	 *
	 * Determines if URL should be read from project settings or not. The project settings
	 * URL will be overridden if this property is not empty.
	 *
	 * @param url_override URL to override with
	 * @note Here the url must contain scheme, address and port at least.
	 */
	void set_url_override(const String &url_override);

	/**
	 * @brief Set the accounts discriminator length.
	 * 
	 * @param accounts_discriminator_length Length of the accounts discriminator.
	 */
	void set_accounts_discriminator_length(const uint32_t accounts_discriminator_length);

	/**
	 * @brief Set the pid address.
	 *
	 * @param pid Base 58 encoded program address.
	 */
	void set_pid(const String &pid);

	/**
	 * @brief Get the pid address.
	 *
	 * @return String Base 58 encoded program address.
	 */
	[[nodiscard]] String get_pid() const;

	/**
	 * @brief Get the program name from the idl.
	 *
	 * @return String program name.
	 */
	[[nodiscard]] String get_idl_name() const;

	/**
	 * @brief Set the json file property.
	 *
	 * When loading the IDL from a JSON object it can be set through this method.
	 *
	 * @param try_from_json_file JSON object containing the IDL.
	 */
	void set_json_file(const Variant &try_from_json_file);

	/**
	 * @brief Get the json file object property.
	 *
	 * @return Variant JSON object property.
	 */
	[[nodiscard]] Variant get_json_file() const;

	/**
	 * @brief Constructs a type decorated f32 Dictionary.
	 *
	 * @param val float value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary f32(float val);

	/**
	 * @brief Constructs a type decorated f64 Dictionary.
	 *
	 * @param val double value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary f64(double val);

	/**
	 * @brief Constructs a type decorated s8 Dictionary.
	 *
	 * @param val integer value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary s8(int val);

	/**
	 * @brief Constructs a type decorated s16 Dictionary.
	 *
	 * @param val integer value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary s16(int val);

	/**
	 * @brief Constructs a type decorated s32 Dictionary.
	 *
	 * @param val integer value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary s32(int32_t val);

	/**
	 * @brief Constructs a type decorated s64 Dictionary.
	 *
	 * @param val integer value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary s64(int64_t val);

	/**
	 * @brief Constructs a type decorated u8 Dictionary.
	 *
	 * @param val integer value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary u8(unsigned int val);

	/**
	 * @brief Constructs a type decorated u16 Dictionary.
	 *
	 * @param val integer value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary u16(unsigned int val);

	/**
	 * @brief Constructs a type decorated u32 Dictionary.
	 *
	 * @param val integer value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary u32(uint32_t val);

	/**
	 * @brief Constructs a type decorated u64 Dictionary.
	 *
	 * @param val integer value.
	 * @return Dictionary decorated value.
	 */
	static Dictionary u64(uint64_t val);

	/**
	 * @brief Constructs a type decorated option Dictionary.
	 *
	 * @param val Optional variant value.
	 * @return Dictionary decorated optional value.
	 */
	static Dictionary option(const Variant &val);

	/**
	 * @brief Checks if a Variant is of an anchor enum type.
	 *
	 * Anchor IDLs seems to have two different ways to describe enums. This method
	 * checks if the type is any of these types.
	 *
	 * @param anchor_type Anchor type, typically from IDL
	 * @return true if type is an anchor enum.
	 * @return false otherwise.
	 */
	static bool is_enum(const Variant &anchor_type);

	/**
	 * @brief Get the name of an anchor object.
	 *
	 * @param anchor_type Anchor object.
	 * @return String name of anchor object.
	 */
	static String get_object_name(const Variant &anchor_type);

	/**
	 * @brief Check if anchor object is of type vec.
	 *
	 * @param anchor_type Anchor object.
	 * @return true if anchor type is vec.
	 * @return false otherwise.
	 */
	static bool is_vec_type(const Variant &anchor_type);

	/**
	 * @brief Get the int type info from an anchor int type.
	 *
	 * @param anchor_type Anchor object.
	 * @return String integer name of anchor type.
	 * @return Empty string if integer cannot be identified.
	 */
	static String get_int_type_info(const Variant &anchor_type);

	/**
	 * @brief Get the godot class hint from an anchor type.
	 *
	 * @param anchor_type Anchor type.
	 * @return String godot class hint.
	 */
	static String get_godot_class_hint(const Variant &anchor_type);

	/**
	 * @brief Get the godot hint from an anchor type.
	 *
	 * @param anchor_type Anchor type.
	 * @return String godot type hint.
	 */
	static PropertyHint get_godot_hint(const Variant &anchor_type);

	/**
	 * @brief Reconstruct a Variant from a byte array.
	 *
	 * Uses a byte array and a Dictionary with an Anchor type specification to reconstruct
	 * a Variant with corresponding Godot type.
	 *
	 * @param bytes Serialized data to deserialize.
	 * @param type Anchor type specification.
	 * @param consumed_bytes Amount of bytes read from the byte serialization.
	 * @return Variant Reconstructed type.
	 */
	Variant deserialize_dict(const PackedByteArray &bytes, const Dictionary &type, uint32_t &consumed_bytes);

	/**
	 * @brief Find an account anchor specification from name.
	 *
	 * @param name Account name to search for.
	 * @return Dictionary Found account specification.
	 * @return Dictionary Empty dictinary if no account was found.
	 */
	Dictionary find_idl_account(const String &name);

	/**
	 * @brief Find an instruction anchor specification from name.
	 *
	 * @param name Instructino name to search for.
	 * @return Dictionary Found instruction specification.
	 * @return Dictionary Empty dictinary if no instruction was found.
	 */
	[[nodiscard]] Dictionary find_idl_instruction(const String &name) const;

	/**
	 * @brief Build a argument dictionary for instructions.
	 *
	 * The argument dictionary is used for instructions and can be passed along with accounts
	 * to create anchor instructions.
	 *
	 * @param arguments Array of argument values.
	 * @note Same order as they appear in IDL.
	 * @param instruction_name Name of instruction with the argument specification.
	 * @return Variant Dictionary with arguments.
	 * @return Variant null on failure.
	 */
	[[nodiscard]] Variant build_argument_dictionary(const Array &arguments, const StringName &instruction_name) const;

	/**
	 * @brief Build an instruction from parameters.
	 *
	 * Searches IDL for instruction specifications and builds an instruction with
	 * provided parameters.
	 *
	 * @param name Instruction name.
	 * @param accounts Accounts to use in instruction.
	 * @param arguments Arguments to use in instruction.
	 * @return Variant Instruction object.
	 * @return null on failure.
	 */
	[[nodiscard]] Variant build_instruction(const String &name, const Array &accounts, const Variant &arguments) const;

	/**
	 * @brief Fetch account of specified account type.
	 *
	 * Asynchronously sends a request to fetch account.
	 * @note Emits account_fetched on completion.
	 *
	 * @param name Account type name.
	 * @param account Account addres.
	 * @return Error status of operation.
	 */
	Error fetch_account(const String &name, const Variant &account);

	/**
	 * @brief Fetches all accounts of an account type.
	 *
	 * @note Emits all_account_fetched on completion.
	 *
	 * @param name Name of account type.
	 * @param additional_filters Account filters.
	 * @return Error status of operation.
	 */
	Error fetch_all_accounts(const String &name, const Array &additional_filters = Array());
};

} //namespace godot

#endif