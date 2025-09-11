#include "keypair.hpp"

#include "ed25519.h"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

void Keypair::_bind_methods() {
	ClassDB::bind_static_method("Keypair", D_METHOD("new_from_seed", "seed"), static_cast<Variant (*)(const PackedByteArray &)>(&Keypair::new_from_seed));
	ClassDB::bind_static_method("Keypair", D_METHOD("new_random"), &Keypair::new_random);
	ClassDB::bind_static_method("Keypair", D_METHOD("new_from_bytes", "bytes"), &Keypair::new_from_bytes);
	ClassDB::bind_static_method("Keypair", D_METHOD("new_from_file", "filename"), &Keypair::new_from_file);

	ClassDB::bind_method(D_METHOD("get_public_string"), &Keypair::get_public_string);
	ClassDB::bind_method(D_METHOD("set_public_string", "p_value"), &Keypair::set_public_string);
	ClassDB::bind_method(D_METHOD("get_public_bytes"), &Keypair::get_public_bytes);
	ClassDB::bind_method(D_METHOD("set_public_bytes", "p_value"), &Keypair::set_public_bytes);
	ClassDB::bind_method(D_METHOD("get_private_string"), &Keypair::get_private_string);
	ClassDB::bind_method(D_METHOD("set_private_string", "p_value"), &Keypair::set_private_string);
	ClassDB::bind_method(D_METHOD("get_private_bytes"), &Keypair::get_private_bytes);
	ClassDB::bind_method(D_METHOD("set_private_bytes", "p_value"), &Keypair::set_private_bytes);
	ClassDB::bind_method(D_METHOD("get_unique"), &Keypair::get_unique);
	ClassDB::bind_method(D_METHOD("set_unique", "p_value"), &Keypair::set_unique);
	ClassDB::bind_method(D_METHOD("get_seed"), &Keypair::get_seed);
	ClassDB::bind_method(D_METHOD("set_seed", "p_value"), &Keypair::set_seed);

	ClassDB::bind_method(D_METHOD("to_pubkey"), &Keypair::to_pubkey);

	ClassDB::bind_method(D_METHOD("sign_message", "message"), &Keypair::sign_message);
	ClassDB::bind_method(D_METHOD("save_to_file", "filename"), &Keypair::save_to_file);
	ClassDB::bind_method(D_METHOD("verify_signature", "signature", "message"), &Keypair::verify_signature);
}

void Keypair::_get_property_list(List<PropertyInfo> *p_list) const {
	PropertyUsageFlags visibility = PROPERTY_USAGE_DEFAULT;

	// Hide other properties if unique is true.
	if (unique) {
		visibility = PROPERTY_USAGE_NO_EDITOR;
	} else {
		p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "seed"));
	}
	p_list->push_back(PropertyInfo(Variant::BOOL, "unique"));
	p_list->push_back(PropertyInfo(Variant::STRING, "public_string", PROPERTY_HINT_NONE, "", visibility, ""));
	p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "public_bytes", PROPERTY_HINT_NONE, "", visibility, ""));
	p_list->push_back(PropertyInfo(Variant::STRING, "private_string", PROPERTY_HINT_NONE, "", visibility, ""));
	p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "private_bytes", PROPERTY_HINT_NONE, "", visibility, ""));
}

bool Keypair::_set(const StringName &p_name, const Variant &p_value) {
	const String name = p_name;
	if (name == "public_string") {
		set_public_string(p_value);
		return true;
	}
	if (name == "public_bytes") {
		set_public_bytes(p_value);
		return true;
	}
	if (name == "private_string") {
		set_private_string(p_value);
		return true;
	}
	if (name == "private_bytes") {
		set_private_bytes(p_value);
		return true;
	}
	if (name == "unique") {
		set_unique(p_value);
		return true;
	}
	if (name == "seed") {
		set_seed(p_value);
		return true;
	}

	return false;
}

bool Keypair::_get(const StringName &p_name, Variant &r_ret) const {
	const String name = p_name;
	if (name == "public_string") {
		r_ret = public_string;
		return true;
	}
	if (name == "public_bytes") {
		r_ret = public_bytes;
		return true;
	}
	if (name == "private_string") {
		r_ret = private_string;
		return true;
	}
	if (name == "private_bytes") {
		r_ret = private_bytes;
		return true;
	}
	if (name == "unique") {
		r_ret = unique;
		return true;
	}
	if (name == "seed") {
		r_ret = seed;
		return true;
	}

	return false;
}

Keypair::Keypair() {
	seed.resize(SEED_LENGTH);
	private_bytes.resize(PRIVATE_KEY_LENGTH);
	public_bytes.resize(PUBLIC_KEY_LENGTH);
	from_seed();
}

void Keypair::from_seed() {
	ed25519_create_keypair(public_bytes.ptrw(), private_bytes.ptrw(), seed.ptr());

	private_string = SolanaUtils::bs58_encode(private_bytes);
	public_string = SolanaUtils::bs58_encode(public_bytes);
}

void Keypair::save_to_file(const String &filename) {
	ERR_FAIL_COND_EDMSG_CUSTOM(seed.size() != 32, "Keypair seed is not known, unable to store.");

	const Ref<FileAccess> file = FileAccess::open(filename, FileAccess::WRITE);
	ERR_FAIL_COND_EDMSG_CUSTOM(!file->is_open(), "Failed to open file " + filename);

	PackedByteArray bytes;

	bytes.append_array(get_seed());
	bytes.append_array(get_public_bytes());
	const String line = JSON::stringify(bytes);
	file->store_line(line.replace("\"", ""));
	file->close();
}

void Keypair::random() {
	RandomNumberGenerator *rand = memnew_custom(RandomNumberGenerator);
	rand->randomize();

	private_bytes.resize(PRIVATE_KEY_LENGTH);
	public_bytes.resize(PUBLIC_KEY_LENGTH);

	// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays,clang-diagnostic-vla-cxx-extension,cppcoreguidelines-pro-bounds-constant-array-index)
	unsigned char random_seed[SEED_LENGTH];
	seed.resize(SEED_LENGTH);
	for (unsigned int i = 0; i < SEED_LENGTH; i++) {
		random_seed[i] = rand->randi();
		seed[i] = random_seed[i];
	}
	// NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays,clang-diagnostic-vla-cxx-extension,cppcoreguidelines-pro-bounds-constant-array-index)
	memfree(rand);

	ed25519_create_keypair(public_bytes.ptrw(), private_bytes.ptrw(), random_seed); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

	private_string = SolanaUtils::bs58_encode(private_bytes);
	public_string = SolanaUtils::bs58_encode(public_bytes);
}

Keypair::Keypair(const PackedByteArray &seed) {
	ERR_FAIL_COND_EDMSG_CUSTOM(seed.size() != SEED_LENGTH, "Seed must be 32 bytes");

	private_bytes.resize(PRIVATE_KEY_LENGTH);
	public_bytes.resize(PUBLIC_KEY_LENGTH);

	ed25519_create_keypair(public_bytes.ptrw(), private_bytes.ptrw(), seed.ptr());

	private_string = SolanaUtils::bs58_encode(private_bytes);
	public_string = SolanaUtils::bs58_encode(public_bytes);
}

Variant Keypair::new_from_seed(const String &seed) {
	Keypair *res = memnew_custom(Keypair);
	res->set_seed(seed.to_ascii_buffer());
	return res;
}

Variant Keypair::new_from_seed(const PackedByteArray &seed) {
	Keypair *res = memnew_custom(Keypair);
	res->set_seed(seed);
	return res;
}

Variant Keypair::new_from_bytes(const Variant &bytes) {
	PackedByteArray byte_array;
	switch (bytes.get_type()) {
		case Variant::Type::STRING:
			byte_array = SolanaUtils::bs58_decode(bytes);
			break;

		case Variant::Type::PACKED_BYTE_ARRAY:
			byte_array = bytes;
			break;

		default:
			ERR_FAIL_V_EDMSG(nullptr, "Expected a string or a byte array.");
			break;
	}

	ERR_FAIL_COND_V_EDMSG_CUSTOM(byte_array.size() != PRIVATE_KEY_LENGTH, nullptr, "Expects 64 bytes input");

	return new_from_seed(byte_array.slice(0, SEED_LENGTH));
}

Variant Keypair::new_from_file(const String &filename) {
	const Ref<FileAccess> file = FileAccess::open(filename, FileAccess::READ);
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!file.is_valid(), nullptr, "Failed to open file " + filename);
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!file->is_open(), nullptr, "Failed to open file " + filename);

	const String content = file->get_as_text();
	file->close();

	const Array bytes = JSON::parse_string(content);

	return new_from_bytes(PackedByteArray(bytes));
}

Variant Keypair::new_from_variant(const Variant &variant) {
	if (variant.get_type() == Variant::Type::OBJECT) {
		const auto *object = static_cast<const Object *>(variant);
		if (object->is_class("Keypair")) {
			return variant;
		}
		if (object->is_class("JSON")) {
			const PackedByteArray bytes = Object::cast_to<JSON>(object)->get_data();
			return new_from_bytes(bytes);
		}
		return nullptr;
	}

	if (variant.get_type() == Variant::Type::PACKED_BYTE_ARRAY) {
		return new_from_bytes(variant);
	}
	if (variant.get_type() == Variant::Type::STRING) {
		const PackedByteArray bytes = SolanaUtils::bs58_decode(variant);
		return new_from_bytes(bytes);
	}

	return nullptr;
}

Variant Keypair::new_random() {
	Keypair *res = memnew_custom(Keypair);
	res->random();
	return res;
}

bool Keypair::is_keypair(const Variant &object) {
	ERR_FAIL_COND_V_CUSTOM(object.get_type() != Variant::OBJECT, false);
	return (static_cast<Object *>(object))->is_class("Keypair");
}

bool Keypair::is_compatible_type(const Variant &object) {
	if (object.get_type() == Variant::Type::OBJECT) {
		const auto *object_ptr = static_cast<const Object *>(object);
		return object_ptr->is_class("Keypair") ||
				object_ptr->is_class("JSON");
	}
	return false;
}

Variant Keypair::to_pubkey() {
	return Pubkey::new_from_bytes(public_bytes);
}

void Keypair::set_public_string(const String &p_value) {
	public_string = p_value;

	// Keypair is not unique anymore.
	unique = false;

	// Update public bytes accordingly.
	const PackedByteArray decoded_value = SolanaUtils::bs58_decode(public_string);
	public_bytes = decoded_value;

	// Print warnings if key length is bad.
	ERR_FAIL_COND_EDMSG_CUSTOM((decoded_value.is_empty() && public_string.length() != 0), "Value contains non-base58 characters");
	ERR_FAIL_COND_EDMSG_CUSTOM(decoded_value.size() != PUBLIC_KEY_LENGTH, "Public key must be 32 bytes");
}

String Keypair::get_public_string() {
	return public_string;
}

void Keypair::set_public_bytes(const PackedByteArray &p_value) {
	public_bytes = p_value;

	// Keypair is not unique.
	unique = false;

	// Do not feed empty value to encode function.
	if (public_bytes.size() == 0) {
		public_string = "";
	} else {
		const String encoded_value = SolanaUtils::bs58_encode(public_bytes);
		public_string = encoded_value;
	}

	// Print warning if key length is bad.
	ERR_FAIL_COND_EDMSG_CUSTOM(public_bytes.size() != PUBLIC_KEY_LENGTH, "Public key must be 32 bytes.");
}
PackedByteArray Keypair::get_public_bytes() const {
	return public_bytes;
}

void Keypair::set_private_string(const String &p_value) {
	private_string = p_value;
	unique = false;

	// Update private bytes accordingly.
	const PackedByteArray decoded_value = SolanaUtils::bs58_decode(private_string);
	private_bytes = decoded_value;

	// Print warnings if key length is bad.
	ERR_FAIL_COND_EDMSG_CUSTOM(decoded_value.is_empty() && !private_string.is_empty(), "Value contains non-base58 characters.");
	ERR_FAIL_COND_EDMSG_CUSTOM(decoded_value.size() != PRIVATE_KEY_LENGTH, "Private key must be 64 bytes.");
}

String Keypair::get_private_string() {
	return private_string;
}

void Keypair::set_private_bytes(const PackedByteArray &p_value) {
	private_bytes = p_value;
	unique = false;

	// Do not feed 0 bytes to encode algorithm.
	if (private_bytes.size() == 0) {
		private_string = "";
	} else {
		const String encoded_value = SolanaUtils::bs58_encode(private_bytes);
		private_string = encoded_value;
	}

	// Print warnings if key length is bad.
	ERR_FAIL_COND_EDMSG_CUSTOM(private_bytes.size() != PRIVATE_KEY_LENGTH, "Private key must be 64 bytes.");
}

PackedByteArray Keypair::get_private_bytes() {
	return private_bytes;
}

void Keypair::set_unique(const bool p_value) {
	unique = p_value;
	notify_property_list_changed();
}

bool Keypair::get_unique() const {
	return unique;
}

void Keypair::set_seed(const PackedByteArray &p_value) {
	ERR_FAIL_COND_EDMSG_CUSTOM(p_value.size() != 32, "Seed must be 32 bytes");

	seed = p_value;
	from_seed();
}

PackedByteArray Keypair::get_seed() {
	return seed;
}

PackedByteArray Keypair::sign_message(const PackedByteArray &message) {
	PackedByteArray result;
	result.resize(SIGNATURE_LENGTH);

	ed25519_sign(result.ptrw(), message.ptr(), message.size(), public_bytes.ptr(), private_bytes.ptr());

	return result;
}

bool Keypair::verify_signature(const PackedByteArray &signature, const PackedByteArray &message) {
	return ed25519_verify(signature.ptr(), message.ptr(), message.size(), public_bytes.ptr()) == 1;
}

} //namespace godot