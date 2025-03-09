#ifndef GODOT_SOLANA_SDK_CUSTOM_CLASS_MANAGER_HPP
#define GODOT_SOLANA_SDK_CUSTOM_CLASS_MANAGER_HPP

#include <unordered_map>

#include "gdextension_interface.h"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/variant/string_name.hpp"

namespace godot {
class CustomClassManager {
private:
	static std::unordered_map<StringName, GDExtensionClassCallVirtual> virtual_methods;

public:
	CustomClassManager() = default;

	void register_virtual_methods(const StringName &class_name);

	~CustomClassManager() = default;
};
} //namespace godot

#endif