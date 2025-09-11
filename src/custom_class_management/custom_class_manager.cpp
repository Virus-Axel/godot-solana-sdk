#include "custom_class_management/custom_class_manager.hpp"

#include <unordered_map>

#include "gdextension_interface.h"
#include "godot_cpp/classes/ref_counted.hpp" // NOLINT(misc-include-cleaner,misc-header-include-cycle)
#include "godot_cpp/variant/string_name.hpp"

namespace godot {

std::unordered_map<StringName, GDExtensionClassCallVirtual> CustomClassManager::virtual_methods;

void CustomClassManager::register_virtual_methods(const StringName &class_name) {
	/*virtual_methods["_process"] = GET_VIRTUAL_METHOD(GenericNode, _process);
	virtual_methods["_ready"] = GET_VIRTUAL_METHOD(GenericNode, _ready);
	virtual_methods["_enter_tree"] = GET_VIRTUAL_METHOD(GenericNode, _enter_tree);
	virtual_methods["_exit_tree"] = GET_VIRTUAL_METHOD(GenericNode, _exit_tree);
	virtual_methods["_get_configuration_warnings"] = GET_VIRTUAL_METHOD(GenericNode, _get_configuration_warnings);
	virtual_methods["_input"] = GET_VIRTUAL_METHOD(GenericNode, _input);
	virtual_methods["_shortcut_input"] = GET_VIRTUAL_METHOD(GenericNode, _shortcut_input);
	virtual_methods["_unhandled_input"] = GET_VIRTUAL_METHOD(GenericNode, _unhandled_input);
	virtual_methods["_unhandled_key_input"] = GET_VIRTUAL_METHOD(GenericNode, _unhandled_key_input);
	virtual_methods["_physics_process"] = GET_VIRTUAL_METHOD(GenericNode, _physics_process);*/
}
} //namespace godot