#include "pubkey.hpp"

#include <climits>
#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "sha256.hpp"

#include "account_meta.hpp"
#include "curve25519.hpp"
#include "keypair.hpp"
#include "solana_utils.hpp"
#include "wallet_adapter.hpp"

namespace godot {

const uint8_t PDA_MARKER[] = "ProgramDerivedAddress"; //NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

bool Pubkey::are_bytes_curve_point() const {
	return is_y_point_valid(bytes.ptr());
}

bool Pubkey::is_variant_valid_key(const Variant &variant) {
	if (variant.get_type() == Variant::STRING) {
		return true;
	}
	if (variant.get_type() == Variant::ARRAY) {
		return true;
	}
	if (variant.get_type() == Variant::PACKED_BYTE_ARRAY) {
		return true;
	}
	if (variant.get_type() != Variant::Type::OBJECT) {
		return false;
	}

	if (Pubkey::is_pubkey(variant)) {
		return true;
	}
	if (Keypair::is_keypair(variant)) {
		return true;
	}
	if (AccountMeta::is_account_meta(variant)) {
		return true;
	}
	if (WalletAdapter::is_wallet_adapter(variant)) {
		auto *phantom_controller = Object::cast_to<WalletAdapter>(variant);
		return phantom_controller->is_connected();
	}
	return false;
}

void Pubkey::_bind_methods() {
	ClassDB::bind_static_method("Pubkey", D_METHOD("new_from_seed", "basePubkey", "seed", "owner_pubkey"), &Pubkey::new_from_seed);
	ClassDB::bind_static_method("Pubkey", D_METHOD("new_from_string", "from"), &Pubkey::new_from_string);
	ClassDB::bind_static_method("Pubkey", D_METHOD("new_from_bytes", "from"), &Pubkey::new_from_bytes);
	ClassDB::bind_static_method("Pubkey", D_METHOD("new_program_address", "seeds", "program_id"), &Pubkey::new_program_address);
	ClassDB::bind_static_method("Pubkey", D_METHOD("new_associated_token_address", "wallet_address", "token_mint_address", "token_program_id"), &Pubkey::new_associated_token_address);
	ClassDB::bind_static_method("Pubkey", D_METHOD("new_pda", "seeds", "program_id"), &Pubkey::new_pda);
	ClassDB::bind_static_method("Pubkey", D_METHOD("new_pda_bytes", "seeds", "program_id"), &Pubkey::new_pda_bytes);
	ClassDB::bind_static_method("Pubkey", D_METHOD("new_random"), &Pubkey::new_random);

	ClassDB::bind_method(D_METHOD("to_string"), &Pubkey::to_string);
	ClassDB::bind_method(D_METHOD("from_string", "p_value"), &Pubkey::from_string);
	ClassDB::bind_method(D_METHOD("to_bytes"), &Pubkey::to_bytes);
	ClassDB::bind_method(D_METHOD("from_bytes", "p_value"), &Pubkey::from_bytes);
	ClassDB::bind_method(D_METHOD("get_seed"), &Pubkey::get_seed);
	ClassDB::bind_method(D_METHOD("set_seed", "p_value"), &Pubkey::set_seed);
	ClassDB::bind_method(D_METHOD("get_type"), &Pubkey::get_type);
	ClassDB::bind_method(D_METHOD("set_type", "p_value"), &Pubkey::set_type);
	ClassDB::bind_method(D_METHOD("get_base"), &Pubkey::get_base);
	ClassDB::bind_method(D_METHOD("set_base", "p_value"), &Pubkey::set_base);
	ClassDB::bind_method(D_METHOD("get_owner"), &Pubkey::get_owner);
	ClassDB::bind_method(D_METHOD("set_owner", "p_value"), &Pubkey::set_owner);
	ClassDB::bind_method(D_METHOD("get_wallet_address"), &Pubkey::get_wallet_address);
	ClassDB::bind_method(D_METHOD("set_wallet_address", "p_value"), &Pubkey::set_wallet_address);
	ClassDB::bind_method(D_METHOD("get_token_mint_address"), &Pubkey::get_token_mint_address);
	ClassDB::bind_method(D_METHOD("set_token_mint_address", "p_value"), &Pubkey::set_token_mint_address);
	ClassDB::bind_method(D_METHOD("is_on_curve"), &Pubkey::are_bytes_curve_point);

	ClassDB::bind_method(D_METHOD("create_from_string", "from"), &Pubkey::create_from_string);
	ClassDB::bind_method(D_METHOD("create_program_address", "seeds", "program_id"), &Pubkey::create_program_address);
	ClassDB::bind_method(D_METHOD("get_associated_token_address", "wallet_pubkey", "token_mint_pubkey"), &Pubkey::get_associated_token_address);

	BIND_CONSTANT(PUBKEY_BYTES);
	BIND_CONSTANT(MAX_SEED_LEN);
	BIND_CONSTANT(MAX_SEEDS);
}

bool Pubkey::_set(const StringName &p_name, const Variant &p_value) {
	const String name = p_name;
	if (name == "value") {
		from_string(p_value);
	} else if (name == "bytes") {
		from_bytes(p_value);
	} else if (name == "seed") {
		set_seed(p_value);
	} else if (name == "type") {
		set_type(p_value);
	} else if (name == "base") {
		set_base(p_value);
	} else if (name == "owner") {
		set_owner(p_value);
	} else if (name == "wallet_address") {
		set_wallet_address(p_value);
	} else if (name == "token_mint_address") {
		set_token_mint_address(p_value);
	} else {
		return false;
	}
	return true;
}

bool Pubkey::_get(const StringName &p_name, Variant &r_ret) const {
	const String name = p_name;
	if (name == "value") {
		r_ret = value;
	} else if (name == "bytes") {
		r_ret = bytes;
	} else if (name == "seed") {
		r_ret = seed;
	} else if (name == "type") {
		r_ret = type;
	} else if (name == "base") {
		r_ret = base;
	} else if (name == "owner") {
		r_ret = owner;
	} else if (name == "wallet_address") {
		r_ret = wallet_address;
	} else if (name == "token_mint_address") {
		r_ret = token_mint_address;
	} else {
		return false;
	}
	return true;
}

String Pubkey::_to_string() const {
	return vformat("[Pubkey:%s]", to_string().utf8().get_data());
}

void Pubkey::from_string(const String &p_value) {
	value = p_value;

	// Update bytes accordingly.
	const PackedByteArray decoded_value = SolanaUtils::bs58_decode(value);
	bytes = decoded_value;

	// Print warnings if key length is bad.
	if (decoded_value.is_empty() && value.length() != 0) {
		ERR_PRINT_ED("Value contains non-base58 characters");
	} else if (decoded_value.size() != PUBKEY_BYTES) {
		Array params;
		params.push_back(decoded_value.size());
		ERR_PRINT_ED(String("Pubkey must be 32 bytes. It is {0}").format(params).utf8());
	}
}

String Pubkey::to_string() const {
	return value;
}

void Pubkey::set_seed(const String &p_value) {
	seed = p_value;
}

String Pubkey::get_seed() {
	return seed;
}

void Pubkey::from_bytes(const PackedByteArray &p_value) {
	bytes = p_value;

	// Do not feed zero bytes into encode function.
	if (bytes.size() == 0) {
		value = "";
	} else {
		const String encoded_value = SolanaUtils::bs58_encode(bytes);
		value = encoded_value;
	}

	// Print warnings if byte length is bad.
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.size() != PUBKEY_BYTES, "Pubkey must be 32 bytes");
}
PackedByteArray Pubkey::to_bytes() const {
	return bytes;
}

void Pubkey::set_type(const String &p_value) {
	type = p_value;
	notify_property_list_changed();
}

String Pubkey::get_type() const {
	return type;
}

void Pubkey::set_base(const Variant &p_value) {
	base = p_value;
}

Variant Pubkey::get_base() {
	return base;
}

void Pubkey::set_owner(const Variant &p_value) {
	owner = p_value;
}

Variant Pubkey::get_owner() {
	return owner;
}

void Pubkey::set_wallet_address(const Variant &p_value) {
	wallet_address = p_value;
}

Variant Pubkey::get_wallet_address() {
	return wallet_address;
}

void Pubkey::set_token_mint_address(const Variant &p_value) {
	token_mint_address = p_value;
}

Variant Pubkey::get_token_mint_address() {
	return token_mint_address;
}

void Pubkey::create_from_string(const String &from) {
	bytes = SolanaUtils::bs58_decode(from);
}

void Pubkey::create_from_array(const unsigned char *data) {
	bytes.resize(PUBKEY_BYTES);
	for (unsigned int i = 0; i < PUBKEY_BYTES; i++) {
		bytes[i] = data[i]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	}
}

Variant Pubkey::new_from_seed(const Variant &basePubkey, const String &seed, const Variant &owner_pubkey) {
	Variant res = memnew_custom(Pubkey);

	SHA256 hasher;

	const PackedByteArray base = Pubkey::bytes_from_variant(basePubkey);
	const PackedByteArray owner = Pubkey::bytes_from_variant(owner_pubkey);

	hasher.update(base.ptr(), base.size());
	hasher.update(seed.to_utf8_buffer().ptr(), seed.length());
	hasher.update(owner.ptr(), owner.size());

	uint8_t *sha256_hash = hasher.digest();

	Object::cast_to<Pubkey>(res)->bytes.resize(PUBKEY_BYTES);

	// NOLINTBEGIN(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-bounds-pointer-arithmetic)
	for (unsigned int i = 0; i < PUBKEY_BYTES; i++) {
		Object::cast_to<Pubkey>(res)->bytes[i] = sha256_hash[i];
	}

	delete[] sha256_hash;
	// NOLINTEND(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-bounds-pointer-arithmetic)

	Object::cast_to<Pubkey>(res)->from_bytes(Object::cast_to<Pubkey>(res)->to_bytes());

	return res;
}

Variant Pubkey::new_from_string(const String &from) {
	Variant res = memnew_custom(Pubkey);
	Object::cast_to<Pubkey>(res)->from_string(from);
	return res;
}

Variant Pubkey::new_from_bytes(const PackedByteArray &from) {
	Variant res = memnew_custom(Pubkey);
	Object::cast_to<Pubkey>(res)->from_bytes(from);
	return res;
}

Variant Pubkey::new_from_variant(const Variant &from) {
	if (from.get_type() == Variant::STRING) {
		return new_from_string(from);
	}
	if (from.get_type() == Variant::ARRAY) {
		return new_from_bytes(PackedByteArray(from));
	}
	if (from.get_type() == Variant::PACKED_BYTE_ARRAY) {
		return new_from_bytes(from);
	}
	if (from.get_type() != Variant::Type::OBJECT) {
		return nullptr;
	}

	if (Pubkey::is_pubkey(from)) {
		return from.duplicate(true);
	}
	if (Keypair::is_compatible_type(from)) {
		const Variant keypair = Keypair::new_from_variant(from);
		ERR_FAIL_COND_V_EDMSG_CUSTOM(keypair.get_type() != Variant::OBJECT, nullptr, "Bug: Keypair from variant and compatibility check mismatch");
		return new_from_bytes(Object::cast_to<Keypair>(keypair)->get_public_bytes());
	}
	if (AccountMeta::is_account_meta(from)) {
		return Object::cast_to<AccountMeta>(from)->get_key();
	}
	if (WalletAdapter::is_wallet_adapter(from)) {
		auto *phantom_controller = Object::cast_to<WalletAdapter>(from);
		if (phantom_controller->is_connected()) {
			return phantom_controller->get_connected_key();
		}
		// Append placeholder
		PackedByteArray empty_bytes;
		empty_bytes.resize(PUBKEY_LENGTH);
		return new_from_bytes(empty_bytes);
	}

	return nullptr;
}

Variant Pubkey::new_program_address(const PackedStringArray &seeds, const Variant &program_id) {
	Variant res = memnew_custom(Pubkey);
	Object::cast_to<Pubkey>(res)->create_program_address(seeds, program_id);
	return res;
}

Variant Pubkey::new_pda(const PackedStringArray &seeds, const Variant &program_id) {
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!Pubkey::is_variant_valid_key(program_id), nullptr, "program_id must be a compatible key type");
	TypedArray<PackedByteArray> arr;

	for (const auto &seed : seeds) {
		arr.append(seed.to_ascii_buffer());
	}

	arr.append(PackedByteArray());

	Variant res = memnew_custom(Pubkey);
	for (uint8_t i = UCHAR_MAX; i > 0; i--) {
		PackedByteArray bump_seed;
		bump_seed.push_back(i);
		arr[arr.size() - 1] = bump_seed;
		if (Object::cast_to<Pubkey>(res)->create_program_address_bytes(arr, program_id)) {
			return res;
		}
	}

	ERR_FAIL_V_EDMSG_CUSTOM(nullptr, "y points were not valid");
}

Variant Pubkey::new_pda_bytes(const Array &seeds, const Variant &program_id) {
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!Pubkey::is_variant_valid_key(program_id), nullptr, "program_id must be a compatible key type");
	TypedArray<PackedByteArray> arr;

	for (unsigned int i = 0; i < seeds.size(); i++) {
		arr.append(seeds[i]);
	}

	arr.append(PackedByteArray());

	Variant res = memnew_custom(Pubkey);
	for (uint8_t i = UCHAR_MAX; i > 0; i--) {
		PackedByteArray bump_seed;
		bump_seed.push_back(i);
		arr[arr.size() - 1] = bump_seed;
		if (Object::cast_to<Pubkey>(res)->create_program_address_bytes(arr, program_id)) {
			return res;
		}
	}

	ERR_FAIL_V_EDMSG_CUSTOM(nullptr, "y points were not valid");
}

Variant Pubkey::new_random() {
	PackedByteArray random_bytes;
	random_bytes.resize(PUBKEY_BYTES);
	const Variant rand = memnew_custom(RandomNumberGenerator);
	Object::cast_to<RandomNumberGenerator>(rand)->randomize();
	for (unsigned int i = 0; i < PUBKEY_BYTES; i++) {
		random_bytes[i] = Object::cast_to<RandomNumberGenerator>(rand)->randi_range(0, UCHAR_MAX);
	}
	return new_from_bytes(random_bytes);
}

bool Pubkey::is_pubkey(const Variant &object) {
	return (static_cast<Object *>(object))->is_class("Pubkey");
}

PackedByteArray Pubkey::bytes_from_variant(const Variant &other) {
	if (other.get_type() == Variant::STRING) {
		return SolanaUtils::bs58_decode(other);
	}
	if (other.get_type() == Variant::ARRAY) {
		return PackedByteArray(other);
	}
	if (other.get_type() == Variant::PACKED_BYTE_ARRAY) {
		return other;
	}
	if (other.get_type() != Variant::Type::OBJECT) {
		const Array params = Array::make(Variant::get_type_name(other.get_type()));
		ERR_FAIL_V_EDMSG_CUSTOM(PackedByteArray(), String("Bug: Unknown object: {0}. Please report.").format(params));
	}

	if (Pubkey::is_pubkey(other)) {
		return Object::cast_to<Pubkey>(other)->to_bytes();
	}
	if (Keypair::is_keypair(other)) {
		return Object::cast_to<Keypair>(other)->get_public_bytes();
	}
	if (AccountMeta::is_account_meta(other)) {
		return Pubkey::bytes_from_variant(Object::cast_to<AccountMeta>(other)->get_key());
	}
	if (WalletAdapter::is_wallet_adapter(other)) {
		auto *phantom_controller = Object::cast_to<WalletAdapter>(other);
		if (phantom_controller->is_connected()) {
			return Pubkey::bytes_from_variant(phantom_controller->get_connected_key());
		}
		// Return placeholder
		PackedByteArray bytes;
		bytes.resize(PUBKEY_BYTES);
		return bytes;
	}

	if (static_cast<Object *>(other)->get_class() == "JSON") {
		const Variant keypair = Keypair::new_from_bytes(PackedByteArray(Object::cast_to<JSON>(other)->get_data()));
		return Object::cast_to<Keypair>(keypair)->get_public_bytes();
	}

	const Array params = Array::make(static_cast<Object *>(other)->get_class());
	ERR_FAIL_V_EDMSG_CUSTOM(PackedByteArray(), String("Bug: Unknown object {0}. Please report.").format(params));
}

String Pubkey::string_from_variant(const Variant &other) {
	if (other.get_type() == Variant::STRING) {
		return other;
	}
	if (other.get_type() == Variant::ARRAY) {
		return SolanaUtils::bs58_encode(PackedByteArray(other));
	}
	if (other.get_type() == Variant::PACKED_BYTE_ARRAY) {
		return SolanaUtils::bs58_encode(other);
	}
	if (other.get_type() != Variant::Type::OBJECT) {
		ERR_FAIL_V_EDMSG_CUSTOM("", "Bug: Unknown object. Please report.");
	}

	if (Pubkey::is_pubkey(other)) {
		return Object::cast_to<Pubkey>(other)->to_string();
	}
	if (Keypair::is_keypair(other)) {
		return Object::cast_to<Keypair>(other)->get_public_string();
	}
	if (AccountMeta::is_account_meta(other)) {
		return Pubkey::string_from_variant(Object::cast_to<AccountMeta>(other)->get_key());
	}
	if (WalletAdapter::is_wallet_adapter(other)) {
		auto *phantom_controller = Object::cast_to<WalletAdapter>(other);
		if (phantom_controller->is_connected()) {
			return Pubkey::string_from_variant(phantom_controller->get_connected_key());
		}

		// Return placeholder
		return SolanaUtils::ZERO_ENCODED_32.c_str();
	}
	if (static_cast<Object *>(other)->get_class() == "JSON") {
		const Variant keypair = Keypair::new_from_bytes(PackedByteArray(Object::cast_to<JSON>(other)->get_data()));
		return Object::cast_to<Keypair>(keypair)->get_public_string();
	}

	ERR_FAIL_V_EDMSG_CUSTOM("", "Bug: Unknown object. Please report.");
}

Variant Pubkey::new_associated_token_address(const Variant &wallet_address, const Variant &token_mint_address, const Variant &token_program_id) {
	TypedArray<PackedByteArray> arr;

	arr.append(Pubkey::bytes_from_variant(wallet_address));
	arr.append(Pubkey::bytes_from_variant(token_program_id));
	arr.append(Pubkey::bytes_from_variant(token_mint_address));
	arr.append(PackedByteArray());

	const String pid = String(SolanaUtils::SPL_ASSOCIATED_TOKEN_ADDRESS.c_str());
	const Variant pid_key = Pubkey::new_from_string(pid);

	Variant res = memnew_custom(Pubkey);
	for (uint8_t i = UCHAR_MAX; i > 0; i--) {
		PackedByteArray bump_seed;
		bump_seed.push_back(i);
		arr[3] = bump_seed;
		if (Object::cast_to<Pubkey>(res)->create_program_address_bytes(arr, pid_key)) {
			return res;
		}
	}

	WARN_PRINT_ED_CUSTOM("y points were not valid");
	return nullptr;
}

bool Pubkey::create_program_address_bytes(const Array &seeds, const Variant &program_id) {
	// Perform seeds checks.
	ERR_FAIL_COND_V_EDMSG_CUSTOM(seeds.size() > MAX_SEEDS, false, "Too many seeds");
	for (unsigned int i = 0; i < seeds.size(); i++) {
		ERR_FAIL_COND_V_EDMSG_CUSTOM(static_cast<PackedByteArray>(seeds[i]).size() > MAX_SEED_LEN, false, "Seed is too long");
	}

	SHA256 hasher;

	for (unsigned int i = 0; i < seeds.size(); i++) {
		hasher.update(static_cast<PackedByteArray>(seeds[i]).ptr(), static_cast<PackedByteArray>(seeds[i]).size());
	}

	// Include program ID and PDA marker in hash.
	const PackedByteArray pid_bytes = Pubkey::bytes_from_variant(program_id);
	hasher.update(pid_bytes.ptr(), pid_bytes.size());
	hasher.update(static_cast<const uint8_t *>(PDA_MARKER), sizeof(PDA_MARKER) - 1);

	uint8_t *hash_ptr = hasher.digest();

	PackedByteArray new_bytes;
	new_bytes.resize(PUBKEY_BYTES);

	// NOLINTBEGIN(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-bounds-pointer-arithmetic)
	for (unsigned int i = 0; i < PUBKEY_BYTES; i++) {
		new_bytes[i] = hash_ptr[i];
	}

	// Remove this memory ASAP.
	delete[] hash_ptr;
	// NOLINTEND(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-bounds-pointer-arithmetic)

	if (is_y_point_valid(new_bytes.ptr())) {
		return false;
	}

	from_bytes(new_bytes);

	return true;
}

bool Pubkey::create_program_address(const PackedStringArray &seeds, const Variant &program_id) {
	// Perform seeds checks.
	ERR_FAIL_COND_V_EDMSG_CUSTOM(seeds.size() > MAX_SEEDS, false, "Too many seeds");
	for (const auto &seed : seeds) {
		ERR_FAIL_COND_V_EDMSG_CUSTOM(seed.length() > MAX_SEED_LEN, false, "Seed is too long");
	}

	SHA256 hasher;

	for (const auto &seed : seeds) {
		hasher.update(seed.to_utf8_buffer().ptr(), seed.length());
	}

	// Include program ID and PDA marker in hash.
	hasher.update(Pubkey::bytes_from_variant(program_id).ptr(), Pubkey::bytes_from_variant(program_id).size());
	hasher.update(static_cast<const uint8_t *>(PDA_MARKER), sizeof(PDA_MARKER) - 1);

	uint8_t *hash_ptr = hasher.digest();

	PackedByteArray new_bytes;
	new_bytes.resize(PUBKEY_BYTES);

	// NOLINTBEGIN(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-bounds-pointer-arithmetic)
	for (unsigned int i = 0; i < PUBKEY_BYTES; i++) {
		new_bytes[i] = hash_ptr[i];
	}

	// Remove this memory ASAP.
	delete[] hash_ptr;
	// NOLINTEND(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-bounds-pointer-arithmetic)

	ERR_FAIL_COND_V_EDMSG_CUSTOM(is_y_point_valid(new_bytes.ptr()), false, "y point is not valid");

	from_bytes(new_bytes);

	return true;
}

bool Pubkey::get_associated_token_address(const Variant &wallet_address, const Variant &token_mint_address) {
	PackedStringArray arr;
	arr.append(wallet_address);
	arr.append(token_mint_address);
	arr.append(String(SolanaUtils::SPL_TOKEN_ADDRESS.c_str()));

	const String pid = String(SolanaUtils::SPL_ASSOCIATED_TOKEN_ADDRESS.c_str());

	return create_program_address(arr, static_cast<Variant>(pid));
}

void Pubkey::_get_property_list(List<PropertyInfo> *p_list) const {
	PropertyUsageFlags seed_visibility = PROPERTY_USAGE_NO_EDITOR;
	PropertyUsageFlags custom_visibility = PROPERTY_USAGE_NO_EDITOR;
	PropertyUsageFlags atoken_visibility = PROPERTY_USAGE_NO_EDITOR;

	p_list->push_back(PropertyInfo(Variant::STRING, "type", PROPERTY_HINT_ENUM, "UNIQUE,SEED,CUSTOM,ASSOCIATED_TOKEN"));

	if (type == "SEED") {
		seed_visibility = PROPERTY_USAGE_DEFAULT;
	} else if (type == "CUSTOM") {
		custom_visibility = PROPERTY_USAGE_DEFAULT;
	} else if (type == "ASSOCIATED_TOKEN") {
		atoken_visibility = PROPERTY_USAGE_DEFAULT;
	}

	p_list->push_back(PropertyInfo(Variant::STRING, "seed", PROPERTY_HINT_NONE, "", seed_visibility, ""));
	p_list->push_back(PropertyInfo(Variant::OBJECT, "base", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", seed_visibility, ""));
	p_list->push_back(PropertyInfo(Variant::OBJECT, "owner", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", seed_visibility, ""));
	p_list->push_back(PropertyInfo(Variant::STRING, "value", PROPERTY_HINT_NONE, "", custom_visibility, ""));
	p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes", PROPERTY_HINT_NONE, "", custom_visibility, ""));
	p_list->push_back(PropertyInfo(Variant::OBJECT, "wallet_address", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", atoken_visibility, ""));
	p_list->push_back(PropertyInfo(Variant::OBJECT, "token_mint_address", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", atoken_visibility, ""));
}

Pubkey::Pubkey(const String &from) {
	from_string(from);
}

Pubkey::Pubkey(const Variant &from) {
	*this = from;
}

Pubkey &Pubkey::operator=(const Variant &other) {
	if (other.get_type() == Variant::STRING) {
		from_string(other);
		return *this;
	}
	if (other.get_type() == Variant::ARRAY) {
		from_bytes(PackedByteArray(other));
		return *this;
	}
	if (other.get_type() == Variant::PACKED_BYTE_ARRAY) {
		from_bytes(other);
		return *this;
	}
	if (other.get_type() != Variant::Type::OBJECT) {
		ERR_PRINT_ED("Assigning pubkey with an non-object type.");
		return *this;
	}

	if (Pubkey::is_pubkey(other)) {
		this->bytes = Object::cast_to<Pubkey>(other)->to_bytes();
		this->value = Object::cast_to<Pubkey>(other)->to_string();
		this->type = Object::cast_to<Pubkey>(other)->get_type();
	} else if (Keypair::is_keypair(other)) {
		this->from_bytes(Object::cast_to<Keypair>(other)->get_public_bytes());
	} else if (AccountMeta::is_account_meta(other)) {
		*this = Object::cast_to<AccountMeta>(other)->get_pubkey();
	} else if (WalletAdapter::is_wallet_adapter(other)) {
		auto *phantom_controller = Object::cast_to<WalletAdapter>(other);
		if (phantom_controller->is_connected()) {
			*this = phantom_controller->get_connected_key();
		} else {
			// Append placeholder
			this->bytes.resize(PUBKEY_BYTES);
		}
	}

	else {
		ERR_PRINT_ED("Assigning pubkey with an unexpected object.");
	}

	return *this;
}

bool Pubkey::operator==(const Pubkey &other) const {
	return (bytes == other.bytes);
}

} //namespace godot