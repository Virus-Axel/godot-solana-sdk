#include "anchor/idl_utils.hpp"

#include <cstdint>

#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "anchor/anchor_program.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

bool IdlUtils::is_typed_primitive(const Dictionary &dict) {
	return (dict.has("dataType") && dict.has("value") && (dict.keys().size() > 0) && (dict["dataType"] != String("option")));
}

Variant::Type IdlUtils::godot_type_from_dictionary(const Dictionary &anchor_type_dict) {
	if (anchor_type_dict.has("option")) {
		return get_godot_type(anchor_type_dict["option"]);
	}
	if (anchor_type_dict.has("kind")) {
		// Kind can be enum (integer) or another object.
		return anchor_type_dict["kind"] == "enum" ? Variant::INT : Variant::OBJECT;
	}
	if (anchor_type_dict.has("defined") || anchor_type_dict.has("type")) {
		return Variant::OBJECT;
	}
	if (anchor_type_dict.has("array") || anchor_type_dict.has("vec")) {
		return Variant::ARRAY;
	}

	return Variant::NIL;
}

Variant::Type IdlUtils::godot_type_from_string(const String &anchor_type_string) {
	if (anchor_type_string == "publicKey") {
		return Variant::OBJECT;
	}
	if (anchor_type_string == "string") {
		return Variant::STRING;
	}
	if (anchor_type_string == "u8" || anchor_type_string == "u16" || anchor_type_string == "u32" || anchor_type_string == "u64" ||
			anchor_type_string == "i8" || anchor_type_string == "i16" || anchor_type_string == "i32" || anchor_type_string == "i64") {
		return Variant::INT;
	}
	if (anchor_type_string == "f32" || anchor_type_string == "f64") {
		return Variant::FLOAT;
	}
	if (anchor_type_string == "bool") {
		return Variant::BOOL;
	}
	if (anchor_type_string == "bytes") {
		return Variant::PACKED_BYTE_ARRAY;
	}

	return Variant::NIL;
}

PackedByteArray IdlUtils::serialize_int(const int64_t value) {
	PackedByteArray result;
	result.resize(sizeof(int64_t));
	result.encode_s64(0, value);

	return result;
}

PackedByteArray IdlUtils::serialize_float(const double value) {
	PackedByteArray result;
	result.resize(sizeof(double));
	result.encode_double(0, value);

	return result;
}

PackedByteArray IdlUtils::serialize_string(const String &value) {
	PackedByteArray result;
	result.resize(sizeof(uint32_t));

	result.encode_u32(0, value.length());
	result.append_array(value.to_ascii_buffer());

	return result;
}

PackedByteArray IdlUtils::serialize_byte_array(const PackedByteArray &value) {
	PackedByteArray result;
	result.resize(sizeof(uint32_t));

	result.encode_u32(0, value.size());
	result.append_array(value);

	return result;
}

PackedByteArray IdlUtils::serialize_variant_array(const Array &value) {
	PackedByteArray result;
	result.resize(4);
	result.encode_u32(0, value.size());
	for (unsigned int i = 0; i < value.size(); i++) {
		result.append_array(serialize_variant(value[i]));
	}

	return result;
}

bool IdlUtils::is_option(const Variant &var) {
	if (var.get_type() == Variant::FLOAT) {
        WARN_PRINT_ED("OOOOOOOOOO");
		return true;
	}
	if (var.get_type() == Variant::DICTIONARY) {
		if (static_cast<Dictionary>(var).has("dataType")) {
			const String data_type = static_cast<Dictionary>(var)["dataType"];
			return (data_type == "option");
		}
	}
	return false;
}

PackedByteArray IdlUtils::serialize_dictionary(const Dictionary &value) {
	if (is_typed_primitive(value)) {
		return serialize_typed_primitive(value);
	}
	if (is_option(value)) {
		return serialize_option(value);
	}

    PackedByteArray result;
    const Array entries = value.values();
	for (unsigned int i = 0; i < entries.size(); i++) {
		result.append_array(serialize_variant(entries[i]));
	}
	return result;
}

PackedByteArray IdlUtils::serialize_object(const Variant &value) {
	if (Pubkey::is_pubkey(value)) {
		return Pubkey::bytes_from_variant(value);
	}
	if (static_cast<Object *>(value)->has_method("serialize")) {
		return static_cast<Object *>(value)->callv("serialize", Array());
	}

	ERR_FAIL_V_EDMSG_CUSTOM({}, "Unsupported Object");
}

Variant::Type IdlUtils::get_godot_type(const Variant &anchor_type) {
	if (anchor_type.get_type() == Variant::DICTIONARY) {
		return godot_type_from_dictionary(static_cast<Dictionary>(anchor_type));
	}
	if (anchor_type.get_type() == Variant::STRING) {
		return godot_type_from_string(static_cast<String>(anchor_type));
	}

    WARN_PRINT_ED("NIL");

	return Variant::NIL;
}

Dictionary IdlUtils::get_decorated_integer(const String &anchor_int_type, const Variant &integer_value) {
	ERR_FAIL_COND_V_CUSTOM(integer_value.get_type() != Variant::INT, {});

	if (anchor_int_type == "u8") {
		return AnchorProgram::u8(integer_value);
	}
	if (anchor_int_type == "u16") {
		return AnchorProgram::u16(integer_value);
	}
	if (anchor_int_type == "u32") {
		return AnchorProgram::u32(integer_value);
	}
	if (anchor_int_type == "u64") {
		return AnchorProgram::u64(integer_value);
	}
	if (anchor_int_type == "s8") {
		return AnchorProgram::s8(integer_value);
	}
	if (anchor_int_type == "s16") {
		return AnchorProgram::s16(integer_value);
	}
	if (anchor_int_type == "s32") {
		return AnchorProgram::s32(integer_value);
	}
	if (anchor_int_type == "s64") {
		return AnchorProgram::s64(integer_value);
	}

	return {};
}

PackedByteArray IdlUtils::serialize_typed_primitive(const Dictionary &dict) {
	ERR_FAIL_COND_V_CUSTOM(!dict.has("dataType"), {});
	ERR_FAIL_COND_V_CUSTOM(!dict.has("value"), {});
	ERR_FAIL_COND_V_CUSTOM(((dict["value"].get_type() != Variant::INT) && (dict["value"].get_type() != Variant::FLOAT)), {});

	PackedByteArray result;
	if (dict["dataType"] == "u8") {
		result.resize(sizeof(uint8_t));
		result.encode_u8(0, dict["value"]);
	} else if (dict["dataType"] == "u16") {
		result.resize(sizeof(uint16_t));
		result.encode_u16(0, dict["value"]);
	} else if (dict["dataType"] == "u32") {
		result.resize(sizeof(uint32_t));
		result.encode_u32(0, dict["value"]);
	} else if (dict["dataType"] == "u64") {
		result.resize(sizeof(uint64_t));
		result.encode_u64(0, dict["value"]);
	} else if (dict["dataType"] == "i8") {
		result.resize(sizeof(int8_t));
		result.encode_s8(0, dict["value"]);
	} else if (dict["dataType"] == "i16") {
		result.resize(sizeof(int16_t));
		result.encode_s16(0, dict["value"]);
	} else if (dict["dataType"] == "i32") {
		result.resize(sizeof(int32_t));
		result.encode_s32(0, dict["value"]);
	} else if (dict["dataType"] == "i64") {
		result.resize(sizeof(int64_t));
		result.encode_s64(0, dict["value"]);
	} else if (dict["dataType"] == "f32") {
		result.resize(sizeof(float));
		result.encode_half(0, dict["value"]);
	} else if (dict["dataType"] == "f64") {
		result.resize(sizeof(double));
		result.encode_double(0, dict["value"]);
	}

	return result;
}

PackedByteArray IdlUtils::serialize_option(const Dictionary &value) {
	ERR_FAIL_COND_V_CUSTOM(!value.has("value"), {});

	PackedByteArray result;
	const Variant val = value["value"];
	if (val.get_type() == Variant::NIL) {
		result.append(0);
	} else {
		result.append(1);
		result.append_array(serialize_variant(val));
	}

	return result;
}

Dictionary IdlUtils::get_decorated_float(const String &anchor_float_type, const Variant &float_value) {
	ERR_FAIL_COND_V_CUSTOM(float_value.get_type() != Variant::FLOAT, Dictionary{});

	if (anchor_float_type == "f32") {
		return AnchorProgram::f32(float_value);
	}
	if (anchor_float_type == "f64") {
		return AnchorProgram::f64(float_value);
	}

	return {};
}

bool IdlUtils::type_contains_any(const Dictionary &anchor_type, const PackedStringArray &data_type_strings) {
	ERR_FAIL_COND_V_CUSTOM(!anchor_type.has("dataType"), false);

	const String data_type = anchor_type["dataType"];

	return data_type_strings.has(data_type);
}

Dictionary IdlUtils::find_name_in_key_array(const Dictionary &idl, const String &idl_key, const String &name) {
	ERR_FAIL_COND_V_CUSTOM(!idl.has(idl_key), {});
	ERR_FAIL_COND_V_CUSTOM(idl[idl_key].get_type() != Variant::ARRAY, {});

	Array instructions = idl[idl_key];
	for (unsigned int i = 0; i < instructions.size(); i++) {
		const Dictionary instruction = instructions[i];
		if (instruction["name"] == name) {
			return instruction;
		}
	}

	return {};
}

PackedByteArray IdlUtils::serialize_variant(const Variant &variant) {
	switch (variant.get_type()) {
		case Variant::INT:
			return serialize_int(static_cast<int64_t>(variant));
			break;
		case Variant::BOOL:
			return { static_cast<uint8_t>(variant) };
			break;
		case Variant::FLOAT:
			return serialize_float(static_cast<double>(variant));
			break;

		case Variant::STRING:
			return serialize_string(static_cast<String>(variant));
			break;

		case Variant::PACKED_BYTE_ARRAY:
			return serialize_byte_array(static_cast<PackedByteArray>(variant));
			break;

		case Variant::ARRAY:
			return serialize_variant_array(static_cast<Array>(variant));
			break;

		case Variant::DICTIONARY:
			return serialize_dictionary(static_cast<Dictionary>(variant));
			break;

		case Variant::OBJECT:
			return serialize_object(variant);
			break;

		case Variant::NIL:
			return {};
			break;

		default:
			ERR_FAIL_COND_V_CUSTOM("Bug: Unsupported variant serialization, please report.", {});
			break;
	}
}

} //namespace godot