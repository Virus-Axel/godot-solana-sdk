#include "hash.hpp"

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

namespace godot {

void Hash::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_value"), &Hash::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "p_value"), &Hash::set_value);
	ClassDB::bind_method(D_METHOD("get_bytes"), &Hash::get_bytes);
	ClassDB::bind_method(D_METHOD("set_bytes", "p_value"), &Hash::set_bytes);
	ClassDB::bind_method(D_METHOD("get_unique"), &Hash::get_unique);
	ClassDB::bind_method(D_METHOD("set_unique", "p_value"), &Hash::set_unique);
}

bool Hash::_set(const StringName &p_name, const Variant &p_value) {
	const String name = p_name;
	if (name == "value") {
		set_value(p_value);
		return true;
	}
	if (name == "bytes") {
		set_bytes(p_value);
		return true;
	}
	if (name == "unique") {
		set_unique(p_value);
		return true;
	}

	return false;
}

bool Hash::_get(const StringName &p_name, Variant &r_ret) const {
	const String name = p_name;
	if (name == "value") {
		r_ret = value;
		return true;
	}
	if (name == "bytes") {
		r_ret = bytes;
		return true;
	}
	if (name == "unique") {
		r_ret = unique;
		return true;
	}

	return false;
}

void Hash::set_value(const String &p_value) {
	value = p_value;

	// Hash is not unique if value is set.
	unique = false;

	// Update byte representation accordingly.
	const PackedByteArray decoded_value = SolanaUtils::bs58_decode(value);
	bytes = decoded_value;

	// Print warnings if bytes are bad.
	if (decoded_value.is_empty() && value.length() != 0) {
		WARN_PRINT_ED_CUSTOM("Value contains non-base58 characters");
	} else if (decoded_value.size() != HASH_LENGTH) {
		WARN_PRINT_ED_CUSTOM("Hash must be 32 bytes");
	}
}

String Hash::get_value() {
	return value;
}

void Hash::set_bytes(const PackedByteArray &p_value) {
	bytes = p_value;

	// Hash is not unique anymore.
	unique = false;

	// Empty byte array could give 1 as encoded string. Avoid this.
	if (bytes.size() == 0) {
		value = "";
	} else {
		value = SolanaUtils::bs58_encode(bytes);
	}

	// Print warning if key is bad
	if (bytes.size() != HASH_LENGTH) {
		WARN_PRINT_ED_CUSTOM("Hash must be 32 bytes");
	}
}

PackedByteArray Hash::get_bytes() {
	return bytes;
}

void Hash::set_unique(const bool p_value) {
	unique = p_value;
	notify_property_list_changed();
}

bool Hash::get_unique() const {
	return unique;
}

void Hash::_get_property_list(List<PropertyInfo> *p_list) const {
	PropertyUsageFlags visibility = PROPERTY_USAGE_DEFAULT;

	// Make other properties invisible if key is unique
	if (unique) {
		visibility = PROPERTY_USAGE_NO_EDITOR;
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "unique"));
	p_list->push_back(PropertyInfo(Variant::STRING, "value", PROPERTY_HINT_NONE, "", visibility, ""));
	p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes", PROPERTY_HINT_NONE, "", visibility, ""));
}

} //namespace godot