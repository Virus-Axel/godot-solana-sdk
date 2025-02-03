#include "anchor/generic_anchor_node.hpp"

#include <functional>

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string_name.hpp"

namespace godot {

// TODO(Virax): Delete this memory as well.
Array *GenericAnchorNode::loaded_idls = nullptr;
std::string GenericAnchorNode::class_name = "GenericAnchorNode";
std::vector<std::function<const StringName &()>> GenericAnchorNode::class_name_func;
std::function<void *(void *)> GenericAnchorNode::stored_func;

GDExtensionObjectPtr GenericAnchorNode::_create_instance_trampoline(void *data) {
	std::cout << "BBBBBBBBBBBB" << std::endl;
	std::function<const StringName &()> ud = *(std::function<const godot::StringName &()>*)data;
	std::cout << "CC " << String(ud()).ascii() << std::endl;
	return stored_func(data);
}

GDExtensionObjectPtr GenericAnchorNode::_create_instance_func2(void *data, int32_t index) {
	std::cout << "AAAAAAAAAAAAAAAAAAAA " << index << std::endl;
	GenericAnchorNode *new_object = memnew(GenericAnchorNode);
	return new_object->_owner;
}

GDExtensionClassInstancePtr GenericAnchorNode::_recreate_instance_func(void *data, GDExtensionObjectPtr obj) {
}

const StringName &GenericAnchorNode::get_class_static() {
	static StringName string_name = StringName(class_name.c_str());
	string_name = StringName(class_name.c_str());
	return string_name;
}

const StringName &GenericAnchorNode::get_class_static2(const String &override_name) {
	static StringName string_name = StringName(override_name);
	string_name = StringName(override_name);
	return string_name;
}

void GenericAnchorNode::_ready() {
	std::cout << String(get_name()).ascii() << std::endl;
}

void GenericAnchorNode::bind_anchor_node(const Dictionary &idl) {
	if (loaded_idls == nullptr) {
		loaded_idls = memnew(Array);
	}
	const int64_t index = loaded_idls->size();
	loaded_idls->append(idl);
	class_name = "ABC" + std::to_string(index);

	std::function<const StringName &()> aaa = std::bind(&GenericAnchorNode::get_class_static2, String(class_name.c_str()));
	class_name_func.push_back(std::bind(&GenericAnchorNode::get_class_static2, String(class_name.c_str())));
	//class_name_func.push_back([&]() { GenericAnchorNode::get_class_static2("HI")};);

	//auto inst_func = &_create_instance_func;
	stored_func = std::bind(&GenericAnchorNode::_create_instance_func2, std::placeholders::_1, index);

	auto *inst_func = &_create_instance_trampoline;
    auto aab = class_name_func[0];

	// Register this class with Godot
	GDExtensionClassCreationInfo3 class_info = {
		false, // GDExtensionBool is_virtual;
		false, // GDExtensionBool is_abstract;
		true, // GDExtensionBool is_exposed;
		true, // GDExtensionBool is_runtime;
		set_bind, // GDExtensionClassSet set_func;
		get_bind, // GDExtensionClassGet get_func;
		has_get_property_list() ? get_property_list_bind : nullptr, // GDExtensionClassGetPropertyList get_property_list_func;
		free_property_list_bind, // GDExtensionClassFreePropertyList2 free_property_list_func;
		property_can_revert_bind, // GDExtensionClassPropertyCanRevert property_can_revert_func;
		property_get_revert_bind, // GDExtensionClassPropertyGetRevert property_get_revert_func;
		validate_property_bind, // GDExtensionClassValidateProperty validate_property_func;
		notification_bind, // GDExtensionClassNotification2 notification_func;
		to_string_bind, // GDExtensionClassToString to_string_func;
		nullptr, // GDExtensionClassReference reference_func;
		nullptr, // GDExtensionClassUnreference unreference_func;
		inst_func, // GDExtensionClassCreateInstance create_instance_func; /* this one is mandatory */
		free, // GDExtensionClassFreeInstance free_instance_func; /* this one is mandatory */
		&_recreate_instance_func, // GDExtensionClassRecreateInstance recreate_instance_func;
		&ClassDB::get_virtual_func, // GDExtensionClassGetVirtual get_virtual_func;
		nullptr, // GDExtensionClassGetVirtualCallData get_virtual_call_data_func;
		nullptr, // GDExtensionClassCallVirtualWithData call_virtual_func;
		nullptr, // GDExtensionClassGetRID get_rid;
		(void *)&class_name_func[0], // void *class_userdata;
	};

	StringName name = String(class_name.c_str());
	StringName parent_name = "Node";
	internal::gdextension_interface_classdb_register_extension_class3(internal::library, name._native_ptr(), parent_name._native_ptr(), &class_info);

	// call bind_methods etc. to register all members of the class
	initialize_class();
	class_name = "GenericAnchorNode";
}

} //namespace godot