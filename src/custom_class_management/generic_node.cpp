#include "custom_class_management/generic_node.hpp"

#include <string>
#include <unordered_map>

#include "gdextension_interface.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string_name.hpp"

namespace godot {

template class GenericType<Node>;
template class GenericType<Resource>;

template <typename BaseType>
std::string GenericType<BaseType>::current_class_name;

template <typename BaseType>
std::unordered_map<StringName, GDExtensionClassCallVirtual> GenericType<BaseType>::virtual_methods;

PackedStringArray GenericNode::_get_configuration_warnings() const{
    return {};
}

void GenericNode::initialize_class(){
    static bool initialized = false;

	if (initialized) {
		return;
	}

    virtual_methods["_process"] = GET_VIRTUAL_METHOD(GenericNode, _process);
	virtual_methods["_ready"] = GET_VIRTUAL_METHOD(GenericNode, _ready);
	virtual_methods["_enter_tree"] = GET_VIRTUAL_METHOD(GenericNode, _enter_tree);
	virtual_methods["_exit_tree"] = GET_VIRTUAL_METHOD(GenericNode, _exit_tree);
	virtual_methods["_get_configuration_warnings"] = GET_VIRTUAL_METHOD(GenericNode, _get_configuration_warnings);
	virtual_methods["_input"] = GET_VIRTUAL_METHOD(GenericNode, _input);
	virtual_methods["_shortcut_input"] = GET_VIRTUAL_METHOD(GenericNode, _shortcut_input);
	virtual_methods["_unhandled_input"] = GET_VIRTUAL_METHOD(GenericNode, _unhandled_input);
	virtual_methods["_unhandled_key_input"] = GET_VIRTUAL_METHOD(GenericNode, _unhandled_key_input);
	virtual_methods["_physics_process"] = GET_VIRTUAL_METHOD(GenericNode, _physics_process);

	GenericType::initialize_class();
	if (_get_bind_methods() != GenericType::_get_bind_methods()) {
		_get_bind_methods();
	}
	initialized = true;
}

} //namespace godot