#include "anchor/idl_utils.hpp"

#include "anchor/anchor_program.hpp"
#include "solana_utils.hpp"

namespace godot {

Variant::Type IdlUtils::godot_type_from_dictionary(const Dictionary anchor_type_dict){
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

Variant::Type IdlUtils::godot_type_from_string(const String anchor_type_string){
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

Variant::Type IdlUtils::get_godot_type(const Variant &anchor_type) {
    if (anchor_type.get_type() == Variant::DICTIONARY) {
		return godot_type_from_dictionary(static_cast<Dictionary>(anchor_type));
	}
    if (anchor_type.get_type() == Variant::STRING) {
		return godot_type_from_string(static_cast<String>(anchor_type));
	}

	return Variant::NIL;
}

Dictionary IdlUtils::get_decorated_integer(const String &anchor_int_type, const Variant &integer_value){
    ERR_FAIL_COND_V_CUSTOM(integer_value.get_type() != Variant::INT, Dictionary{});

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

    return Dictionary();
}

Dictionary IdlUtils::get_decorated_float(const String &anchor_float_type, const Variant &float_value){
    ERR_FAIL_COND_V_CUSTOM(float_value.get_type() != Variant::FLOAT, Dictionary{});

    if (anchor_float_type == "f32") {
        return AnchorProgram::f32(float_value);
    }
    if (anchor_float_type == "f64") {
        return AnchorProgram::f64(float_value);
    }

    return Dictionary();
}

} //namespace godot