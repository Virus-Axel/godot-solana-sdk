#include "anchor/generic_anchor_node.hpp"

#include <functional>

#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string_name.hpp"

#include "anchor/generic_anchor_resource.hpp"
#include "pubkey.hpp"

namespace godot {

// TODO(Virax): Delete this memory as well.
Array *GenericAnchorNode::loaded_idls = nullptr;
std::vector<String *> GenericAnchorNode::names;

void GenericAnchorNode::bind_resources(const Array &resources, const String &class_name) {
	for (uint32_t i = 0; i < resources.size(); i++) {
	}
}

GDExtensionObjectPtr GenericAnchorNode::_create_instance_func(void *data) {
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

bool (Wrapped::*GenericAnchorNode::_get_set())(const StringName &p_name, const Variant &p_property) {
	return (bool(Wrapped::*)(const StringName &p_name, const Variant &p_property)) & GenericAnchorNode::_set;
}

bool (Wrapped::*GenericAnchorNode::_get_get())(const StringName &p_name, Variant &r_ret) const {
	return (bool(Wrapped::*)(const StringName &p_name, Variant &r_ret) const) & GenericAnchorNode::_get;
}

void (Wrapped::*GenericAnchorNode::_get_get_property_list())(List<PropertyInfo> *p_list) const {
	return (void(Wrapped::*)(List<PropertyInfo> * p_list) const) & GenericAnchorNode::_get_property_list;
}

bool (Wrapped::*GenericAnchorNode::_get_property_can_revert())(const StringName &p_name) const {
	return (bool(Wrapped::*)(const StringName &p_name) const) & GenericAnchorNode::_property_can_revert;
}

bool (Wrapped::*GenericAnchorNode::_get_property_get_revert())(const StringName &p_name, Variant &) const {
	return (bool(Wrapped::*)(const StringName &p_name, Variant &) const) & GenericAnchorNode::_property_get_revert;
}

void (Wrapped::*GenericAnchorNode::_get_validate_property())(PropertyInfo &p_property) const {
	return (void(Wrapped::*)(PropertyInfo & p_property) const) & GenericAnchorNode::_validate_property;
}

String (Wrapped::*GenericAnchorNode::_get_to_string())() const {
	return (String(Wrapped::*)() const)&GenericAnchorNode::_to_string;
}

void GenericAnchorNode::initialize_class() {
	static bool initialized = false;
	if (initialized) {
		return;
	}
	Node::initialize_class();
	if (GenericAnchorNode::_get_bind_methods() != Node::_get_bind_methods()) {
		_bind_methods();
		Node::register_virtuals<GenericAnchorNode, Node>();
	}
	initialized = true;
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

const StringName &GenericAnchorNode::get_parent_class_static() {
	return Node::get_class_static();
}

void GenericAnchorNode::notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed) {
	if (p_instance && GenericAnchorNode::_get_notification()) {
		if (!p_reversed) {
			Node::notification_bind(p_instance, p_what, p_reversed);
		}
		if (GenericAnchorNode::_get_notification() != Node::_get_notification()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			cls->_notification(p_what);
		}
		if (p_reversed) {
			Node::notification_bind(p_instance, p_what, p_reversed);
		}
	}
}

GDExtensionBool GenericAnchorNode::set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value) {
	if (p_instance) {
		if (Node::set_bind(p_instance, p_name, p_value)) {
			return true;
		}
		if (GenericAnchorNode::_get_set() != Node::_get_set()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			return cls->_set(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<const Variant *>(p_value));
		}
	}
	return false;
}

GDExtensionBool GenericAnchorNode::get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance) {
		if (Node::get_bind(p_instance, p_name, r_ret)) {
			return true;
		}
		if (GenericAnchorNode::_get_get() != Node::_get_get()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			return cls->_get(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<Variant *>(r_ret));
		}
	}
	return false;
}

bool GenericAnchorNode::has_get_property_list() {
	return GenericAnchorNode::_get_get_property_list() && GenericAnchorNode::_get_get_property_list() != Node::_get_get_property_list();
}

const GDExtensionPropertyInfo *GenericAnchorNode::get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count) {
	if (!p_instance) {
		if (r_count)
			*r_count = 0;
		return nullptr;
	}
	GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
	List<PropertyInfo> &plist_cpp = cls->plist_owned;
	ERR_FAIL_COND_V_MSG(!plist_cpp.is_empty(), nullptr, "Internal error, property list was not freed by engine!");
	cls->_get_property_list(&plist_cpp);
	return internal::create_c_property_list(plist_cpp, r_count);
}

void GenericAnchorNode::free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/) {
	if (p_instance) {
		GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
		cls->plist_owned.clear();
		internal::free_c_property_list(const_cast<GDExtensionPropertyInfo *>(p_list));
	}
}

GDExtensionBool GenericAnchorNode::property_can_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name) {
	if (p_instance && GenericAnchorNode::_get_property_can_revert()) {
		if (GenericAnchorNode::_get_property_can_revert() != Node::_get_property_can_revert()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			return cls->_property_can_revert(*reinterpret_cast<const StringName *>(p_name));
		}
		return Node::property_can_revert_bind(p_instance, p_name);
	}
	return false;
}

GDExtensionBool GenericAnchorNode::property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance && GenericAnchorNode::_get_property_get_revert()) {
		if (GenericAnchorNode::_get_property_get_revert() != Node::_get_property_get_revert()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			return cls->_property_get_revert(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<Variant *>(r_ret));
		}
		return Node::property_get_revert_bind(p_instance, p_name, r_ret);
	}
	return false;
}

GDExtensionBool GenericAnchorNode::validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property) {
	bool ret = false;
	if (p_instance && GenericAnchorNode::_get_validate_property()) {
		ret = Node::validate_property_bind(p_instance, p_property);
		if (GenericAnchorNode::_get_validate_property() != Node::_get_validate_property()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			PropertyInfo info(p_property);
			cls->_validate_property(info);
			info._update(p_property);
			return true;
		}
	}
	return ret;
}

void GenericAnchorNode::to_string_bind(GDExtensionClassInstancePtr p_instance, GDExtensionBool *r_is_valid, GDExtensionStringPtr r_out) {
	if (p_instance && GenericAnchorNode::_get_to_string()) {
		if (GenericAnchorNode::_get_to_string() != Node::_get_to_string()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			*reinterpret_cast<String *>(r_out) = cls->_to_string();
			*r_is_valid = true;
			return;
		}
		Node::to_string_bind(p_instance, r_is_valid, r_out);
	}
}

void GenericAnchorNode::free(void * /*data*/, GDExtensionClassInstancePtr ptr) {
	if (ptr) {
		GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(ptr);
		cls->~GenericAnchorNode();
		Memory::free_static(cls);
	}
}

void *GenericAnchorNode::_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/) {
	return nullptr;
}

void GenericAnchorNode::_gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/) {}

GDExtensionBool GenericAnchorNode::_gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/) {
	return true;
}

void GenericAnchorNode::_notificationv(int32_t p_what, bool p_reversed) {
	GenericAnchorNode::notification_bind(this, p_what, p_reversed);
}

void GenericAnchorNode::_bind_methods() {
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

	//ClassDB::register_class<GenericAnchorResource>();

	if (idl.has("types")) {
		const Array types = idl["types"];
		for (unsigned int i = 0; i < types.size(); i++) {
			GenericAnchorResource::bind_anchor_resource(types[i]);
		}
	}

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
		&_create_instance_func, // GDExtensionClassCreateInstance create_instance_func; /* this one is mandatory */
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