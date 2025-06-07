#ifndef IDL_UTILS_HPP
#define IDL_UTILS_HPP

#include "godot_cpp/variant/variant.hpp"

namespace godot {

class IdlUtils {
private:
	static Variant::Type godot_type_from_dictionary(const Dictionary anchor_type_dict);
	static Variant::Type godot_type_from_string(const String anchor_type_string);

public:
	/**
	 * @brief Get the godot type of an anchor type.
	 *
	 * @param anchor_type Anchor type information.
	 * @return Variant::Type Corresponding godot type.
	 */
	static Variant::Type get_godot_type(const Variant &anchor_type);

    static Dictionary get_decorated_integer(const String &anchor_int_type, const Variant &integer_value);
    static Dictionary get_decorated_float(const String &anchor_float_type, const Variant &float_value);
};

} //namespace godot

#endif