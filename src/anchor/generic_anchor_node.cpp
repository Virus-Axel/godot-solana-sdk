#include "anchor/generic_anchor_node.hpp"

#include <functional>

#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string_name.hpp"

#include "pubkey.hpp"

namespace godot {

// TODO(Virax): Delete this memory as well.
Array *GenericAnchorNode::loaded_idls = nullptr;
std::vector<String *> GenericAnchorNode::names;

void GenericAnchorNode::bind_resources(const Array &resources, const String &class_name) {
	for (uint32_t i = 0; i < resources.size(); i++) {
	}
}

GDExtensionObjectPtr GenericAnchorNode::_create_instance_trampoline(void *data) {
	std::cout << "INST" << std::endl;
	const String instance_class = *(String *)data;
	std::cout << "AAAAAa " << instance_class.ascii() << std::endl;
	GenericAnchorNode *new_object = memnew(GenericAnchorNode);
	return new_object->_owner;
}

bool GenericAnchorNode::_is_extension_class() const {
	return true;
}

const StringName *GenericAnchorNode::_get_extension_class_name() {
	const StringName &string_name = get_class_static();
	return &string_name;
}

void (*GenericAnchorNode::_get_bind_methods())() {
	return &GenericAnchorNode::_bind_methods;
}

void (Wrapped::*GenericAnchorNode::_get_notification())(int) {
	return (void(::godot::Wrapped::*)(int)) & GenericAnchorNode::_notification;
}

GDExtensionClassInstancePtr GenericAnchorNode::_recreate_instance_func(void *data, GDExtensionObjectPtr obj) {
#ifdef HOT_RELOAD_ENABLED
	GenericAnchorNode *new_instance = (GenericAnchorNode *)memalloc(sizeof(GenericAnchorNode));
	Wrapped::RecreateInstance recreate_data = { new_instance, obj, Wrapped::recreate_instance };
	Wrapped::recreate_instance = &recreate_data;
	memnew_placement(new_instance, GenericAnchorNode);
	return new_instance;
#else
	return nullptr;
#endif
}

const StringName &GenericAnchorNode::get_class_static() {
	static StringName string_name = "GenericAnchorNode";
	return string_name;
}

void GenericAnchorNode::_ready() {
	std::cout << String(get_name()).ascii() << std::endl;
}

void GenericAnchorNode::bind_anchor_node(const Dictionary &idl) {
	ERR_FAIL_COND_EDMSG(!idl.has("name"), "IDL does not contain a name.");

	const Variant custom_pid = AnchorProgram::get_address_from_idl(idl);

	ERR_FAIL_COND_EDMSG(custom_pid.get_type() != Variant::OBJECT, "IDL does not contain a PID.");
	const Variant parsed_program = memnew(AnchorProgram);
	Object::cast_to<AnchorProgram>(parsed_program)->set_idl(idl);
	Object::cast_to<AnchorProgram>(parsed_program)->set_pid(Object::cast_to<Pubkey>(custom_pid)->to_string());

	if (loaded_idls == nullptr) {
		loaded_idls = memnew(Array);
	}
	const int64_t index = loaded_idls->size();
	loaded_idls->append(idl);
	const String class_name = idl["name"];

	names.push_back(memnew(String(class_name)));

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
		&_create_instance_trampoline, // GDExtensionClassCreateInstance create_instance_func; /* this one is mandatory */
		free, // GDExtensionClassFreeInstance free_instance_func; /* this one is mandatory */
		&_recreate_instance_func, // GDExtensionClassRecreateInstance recreate_instance_func;
		&ClassDB::get_virtual_func, // GDExtensionClassGetVirtual get_virtual_func;
		nullptr, // GDExtensionClassGetVirtualCallData get_virtual_call_data_func;
		nullptr, // GDExtensionClassCallVirtualWithData call_virtual_func;
		nullptr, // GDExtensionClassGetRID get_rid;
		(void *)names[names.size() - 1], // void *class_userdata;
	};

	StringName name = String(class_name);
	StringName parent_name = "Node";
	internal::gdextension_interface_classdb_register_extension_class3(internal::library, name._native_ptr(), parent_name._native_ptr(), &class_info);

	// call bind_methods etc. to register all members of the class
	initialize_class();
}

} //namespace godot