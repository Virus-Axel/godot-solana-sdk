#include "anchor/generic_anchor_resource.hpp"

#include <functional>

#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

// TODO(Virax): Delete this memory as well.
std::vector<String *> GenericAnchorResource::names;
std::string GenericAnchorResource::string_name = "GenericAnchorResourceaa";

bool GenericAnchorResource::_set(const StringName &p_name, const Variant &p_value) { // NOLINT(bugprone-easily-swappable-parameters)
	const String name = p_name;

	for (auto &property : properties) {
		if (property.property_info.name == name) {
			property.value = p_value;
			return true;
		}
	}

	// Not a property so a enable checkbox is being set.
	ERR_FAIL_COND_V_EDMSG_CUSTOM(name.begins_with(OPTIONAL_PROPERTY_PREFIX), false, "Could not find property");

	const String property_to_toggle = name.lstrip(OPTIONAL_PROPERTY_PREFIX);
	for (auto &property : properties) {
		if (property.property_info.name == property_to_toggle) {
			property.enabled = !property.enabled;
			return true;
		}
	}

	return true;
}

bool GenericAnchorResource::_get(const StringName &p_name, Variant &r_ret) const {
	const String name = p_name;

	for (const auto &property : properties) {
		if (property.property_info.name == name) {
			r_ret = property.value;
			return true;
		}
	}
	std::cout << "Bye" << std::endl;

	return true;
}

void GenericAnchorResource::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::BOOL, "HIIIIIIIIIIIIIIIIIIIII"));
	std::cout << "Searching props" << std::endl;
	for (const auto &property : properties) {
		std::cout << "Found one" << std::endl;
		if (property.optional) {
			p_list->push_back(PropertyInfo(Variant::BOOL, OPTIONAL_PROPERTY_PREFIX + property.property_info.name));
		}
		if (property.enabled) {
			std::cout << "found enabled" << std::endl;
			p_list->push_back(property.property_info);
		}
	}
}

GDExtensionObjectPtr GenericAnchorResource::_create_instance_func(void *data) {
	std::cout << "INST" << std::endl;
	const String instance_class = *(String *)data;
	ResourcePropertyInfo info{
		.property_info = PropertyInfo(Variant::STRING ,instance_class),
		.value = "nullptr",
		.optional = false,
		.enabled = true,
	};
	std::cout << "AAAAAa " << instance_class.ascii() << std::endl;
	GenericAnchorResource *new_object = memnew(GenericAnchorResource);
	new_object->properties.push_back(info);
	new_object->notify_property_list_changed();
	//new_object->_bind_methods();
	return new_object->_owner;
}

bool GenericAnchorResource::_is_extension_class() const {
	return true;
}

const StringName *GenericAnchorResource::_get_extension_class_name() {
	const StringName &string_name = get_class_static();
	return &string_name;
}

void (*GenericAnchorResource::_get_bind_methods())() {
	return &GenericAnchorResource::_bind_methods;
}

void (Wrapped::*GenericAnchorResource::_get_notification())(int) {
	return (void(::godot::Wrapped::*)(int)) & GenericAnchorResource::_notification;
}

bool (Wrapped::*GenericAnchorResource::_get_set())(const StringName &p_name, const Variant &p_property) {
	return (bool(Wrapped::*)(const StringName &p_name, const Variant &p_property)) & GenericAnchorResource::_set;
}

bool (Wrapped::*GenericAnchorResource::_get_get())(const StringName &p_name, Variant &r_ret) const {
	return (bool(Wrapped::*)(const StringName &p_name, Variant &r_ret) const) & GenericAnchorResource::_get;
}

void (Wrapped::*GenericAnchorResource::_get_get_property_list())(List<PropertyInfo> *p_list) const {
	return (void(Wrapped::*)(List<PropertyInfo> * p_list) const) & GenericAnchorResource::_get_property_list;
}

bool (Wrapped::*GenericAnchorResource::_get_property_can_revert())(const StringName &p_name) const {
	return (bool(Wrapped::*)(const StringName &p_name) const) & GenericAnchorResource::_property_can_revert;
}

bool (Wrapped::*GenericAnchorResource::_get_property_get_revert())(const StringName &p_name, Variant &) const {
	return (bool(Wrapped::*)(const StringName &p_name, Variant &) const) & GenericAnchorResource::_property_get_revert;
}

void (Wrapped::*GenericAnchorResource::_get_validate_property())(PropertyInfo &p_property) const {
	return (void(Wrapped::*)(PropertyInfo & p_property) const) & GenericAnchorResource::_validate_property;
}

String (Wrapped::*GenericAnchorResource::_get_to_string())() const {
	return (String(Wrapped::*)() const)&GenericAnchorResource::_to_string;
}

void GenericAnchorResource::initialize_class() {
	static bool initialized = false;
	if (initialized) {
		return;
	}
	Node::initialize_class();
	if (GenericAnchorResource::_get_bind_methods() != Node::_get_bind_methods()) {
		_bind_methods();
		std::cout << "virt" << std::endl;
		Node::register_virtuals<GenericAnchorResource, Node>();
		std::cout << "uals" << std::endl;
	}
	initialized = true;
}

GDExtensionClassInstancePtr GenericAnchorResource::_recreate_instance_func(void *data, GDExtensionObjectPtr obj) {
#ifdef HOT_RELOAD_ENABLED
	GenericAnchorResource *new_instance = (GenericAnchorResource *)memalloc(sizeof(GenericAnchorResource));
	Wrapped::RecreateInstance recreate_data = { new_instance, obj, Wrapped::recreate_instance };
	Wrapped::recreate_instance = &recreate_data;
	memnew_placement(new_instance, GenericAnchorResource);
	return new_instance;
#else
	return nullptr;
#endif
}

const StringName &GenericAnchorResource::get_class_static() {
	static StringName string_name_gd = "GenericAnchorResource";
	string_name_gd = *memnew(String(string_name.c_str()));
	return string_name_gd;
}

const StringName &GenericAnchorResource::get_parent_class_static() {
	return Node::get_class_static();
}

void GenericAnchorResource::notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed) {
	if (p_instance && GenericAnchorResource::_get_notification()) {
		if (!p_reversed) {
			Node::notification_bind(p_instance, p_what, p_reversed);
		}
		if (GenericAnchorResource::_get_notification() != Node::_get_notification()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			cls->_notification(p_what);
		}
		if (p_reversed) {
			Node::notification_bind(p_instance, p_what, p_reversed);
		}
	}
}

GDExtensionBool GenericAnchorResource::set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value) {
	if (p_instance) {
		if (Node::set_bind(p_instance, p_name, p_value)) {
			return true;
		}
		if (GenericAnchorResource::_get_set() != Node::_get_set()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			return cls->_set(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<const Variant *>(p_value));
		}
	}
	return false;
}

GDExtensionBool GenericAnchorResource::get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance) {
		if (Node::get_bind(p_instance, p_name, r_ret)) {
			return true;
		}
		if (GenericAnchorResource::_get_get() != Node::_get_get()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			return cls->_get(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<Variant *>(r_ret));
		}
	}
	return false;
}

bool GenericAnchorResource::has_get_property_list() {
	return GenericAnchorResource::_get_get_property_list() && GenericAnchorResource::_get_get_property_list() != Node::_get_get_property_list();
}

const GDExtensionPropertyInfo *GenericAnchorResource::get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count) {
	std::cout << "HERERRRRRRRRRRRRRRRRRRRRRRRRR" << std::endl;
	if (!p_instance) {
		if (r_count)
			*r_count = 0;
		return nullptr;
	}
	GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
	List<PropertyInfo> &plist_cpp = cls->plist_owned;
	ERR_FAIL_COND_V_MSG(!plist_cpp.is_empty(), nullptr, "Internal error, property list was not freed by engine!");
	cls->_get_property_list(&plist_cpp);
	std::cout << "LIST size " << plist_cpp.size() << std::endl;
	for(unsigned int i = 0; i < plist_cpp.size(); i++){
		std::cout << "naamaa " << String(plist_cpp[i].name).ascii() <<" - " << String(plist_cpp[i].class_name).ascii() << std::endl;
	}
	return internal::create_c_property_list(plist_cpp, r_count);
}

void GenericAnchorResource::free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/) {
	if (p_instance) {
		GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
		cls->plist_owned.clear();
		internal::free_c_property_list(const_cast<GDExtensionPropertyInfo *>(p_list));
	}
}

GDExtensionBool GenericAnchorResource::property_can_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name) {
	if (p_instance && GenericAnchorResource::_get_property_can_revert()) {
		if (GenericAnchorResource::_get_property_can_revert() != Node::_get_property_can_revert()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			return cls->_property_can_revert(*reinterpret_cast<const StringName *>(p_name));
		}
		return Node::property_can_revert_bind(p_instance, p_name);
	}
	return false;
}

GDExtensionBool GenericAnchorResource::property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance && GenericAnchorResource::_get_property_get_revert()) {
		if (GenericAnchorResource::_get_property_get_revert() != Node::_get_property_get_revert()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			return cls->_property_get_revert(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<Variant *>(r_ret));
		}
		return Node::property_get_revert_bind(p_instance, p_name, r_ret);
	}
	return false;
}

GDExtensionBool GenericAnchorResource::validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property) {
	bool ret = false;
	if (p_instance && GenericAnchorResource::_get_validate_property()) {
		ret = Node::validate_property_bind(p_instance, p_property);
		if (GenericAnchorResource::_get_validate_property() != Node::_get_validate_property()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			PropertyInfo info(p_property);
			cls->_validate_property(info);
			info._update(p_property);
			return true;
		}
	}
	return ret;
}

void GenericAnchorResource::to_string_bind(GDExtensionClassInstancePtr p_instance, GDExtensionBool *r_is_valid, GDExtensionStringPtr r_out) {
	if (p_instance && GenericAnchorResource::_get_to_string()) {
		if (GenericAnchorResource::_get_to_string() != Node::_get_to_string()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			*reinterpret_cast<String *>(r_out) = cls->_to_string();
			*r_is_valid = true;
			return;
		}
		Node::to_string_bind(p_instance, r_is_valid, r_out);
	}
}

void GenericAnchorResource::free(void * /*data*/, GDExtensionClassInstancePtr ptr) {
	if (ptr) {
		GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(ptr);
		cls->~GenericAnchorResource();
		Memory::free_static(cls);
	}
}

void *GenericAnchorResource::_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/) {
	return nullptr;
}

void GenericAnchorResource::_gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/) {}

GDExtensionBool GenericAnchorResource::_gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/) {
	return true;
}

void GenericAnchorResource::_notificationv(int32_t p_what, bool p_reversed) {
	GenericAnchorResource::notification_bind(this, p_what, p_reversed);
}

void GenericAnchorResource::_bind_methods() {
}

void GenericAnchorResource::bind_anchor_resource(const Dictionary &resource) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!resource.has("name"), "Resource struct does not contain a name.");
	ERR_FAIL_COND_CUSTOM(resource.has("vec"));
	ERR_FAIL_COND_CUSTOM(resource.has("array"));

	StringName *namn = memnew(StringName("GenericAnchorResource"));
	const String class_name = resource["name"];
	//const String class_name = "GenericAnchorResource";

	names.push_back(memnew(String(class_name)));

	const Dictionary struct_info = resource["type"];
	UtilityFunctions::print(struct_info);
	ERR_FAIL_COND_EDMSG_CUSTOM(!struct_info.has("fields"), "Resource struct does not contain any fields");

	const Array fields = struct_info["fields"];
	for (unsigned int i = 0; i < fields.size(); i++) {
		//add_property(fields[i]);
	}

	//std::cout << "HAAAS " << (int)GenericAnchorResource::has_get_property_list() << std::endl;

	// Register this class with Godot
	GDExtensionClassCreationInfo3 class_info = {
		false, // GDExtensionBool is_virtual;
		false, // GDExtensionBool is_abstract;
		true, // GDExtensionBool is_exposed;
		false, // GDExtensionBool is_runtime;
		GenericAnchorResource::set_bind, // GDExtensionClassSet set_func;
		GenericAnchorResource::get_bind, // GDExtensionClassGet get_func;
		GenericAnchorResource::get_property_list_bind, // GDExtensionClassGetPropertyList get_property_list_func;
		GenericAnchorResource::free_property_list_bind, // GDExtensionClassFreePropertyList2 free_property_list_func;
		GenericAnchorResource::property_can_revert_bind, // GDExtensionClassPropertyCanRevert property_can_revert_func;
		GenericAnchorResource::property_get_revert_bind, // GDExtensionClassPropertyGetRevert property_get_revert_func;
		GenericAnchorResource::validate_property_bind, // GDExtensionClassValidateProperty validate_property_func;
		GenericAnchorResource::notification_bind, // GDExtensionClassNotification2 notification_func;
		GenericAnchorResource::to_string_bind, // GDExtensionClassToString to_string_func;
		nullptr, // GDExtensionClassReference reference_func;
		nullptr, // GDExtensionClassUnreference unreference_func;
		&GenericAnchorResource::_create_instance_func, // GDExtensionClassCreateInstance create_instance_func; /* this one is mandatory */
		GenericAnchorResource::free, // GDExtensionClassFreeInstance free_instance_func; /* this one is mandatory */
		&GenericAnchorResource::_recreate_instance_func, // GDExtensionClassRecreateInstance recreate_instance_func;
		&GenericAnchorResource::get_virtual_func, // GDExtensionClassGetVirtual get_virtual_func;
		nullptr, // GDExtensionClassGetVirtualCallData get_virtual_call_data_func;
		nullptr, // GDExtensionClassCallVirtualWithData call_virtual_func;
		nullptr, // GDExtensionClassGetRID get_rid;
		//(void*)&GenericAnchorResource::get_class_static(),
		static_cast<void *>(names[names.size() - 1]), // void *class_userdata;
	};

	StringName* sname = memnew(StringName("aaaaaaaaaaaaaaaa"));
	StringName* class_nam = memnew(StringName("GenericAnchorResource"));
	StringName* snone = memnew(StringName(""));
	StringName* setter = memnew(StringName("say_hii"));
	StringName* getter = memnew(StringName("say_hi"));

	GDExtensionPropertyInfo prop_info = {
		static_cast<GDExtensionVariantType>(Variant::BOOL), // GDExtensionVariantType type;
		sname->_native_ptr(), // GDExtensionStringNamePtr name;
		class_nam->_native_ptr(), // GDExtensionStringNamePtr class_name;
		godot::PROPERTY_HINT_NONE, // NONE //uint32_t hint;
		snone->_native_ptr(), // GDExtensionStringPtr hint_string;
		6U, // DEFAULT //uint32_t usage;
	};


	MethodBind *getter_bind = create_method_bind(&GenericAnchorResource::say_hi);
	MethodBind *setter_bind = create_method_bind(&GenericAnchorResource::say_hii);
	getter_bind->set_instance_class(*namn);
	getter_bind->set_name(*getter);
	std::cout << String(getter_bind->get_name()).ascii() << std::endl;
	std::cout << String(setter_bind->get_name()).ascii() << std::endl;


	/*	 This is what needs to happen I think.
	BIND_VIRTUAL_METHOD(T, _process);
	BIND_VIRTUAL_METHOD(T, _physics_process);
	BIND_VIRTUAL_METHOD(T, _enter_tree);
	BIND_VIRTUAL_METHOD(T, _exit_tree);
	BIND_VIRTUAL_METHOD(T, _ready);
	BIND_VIRTUAL_METHOD(T, _get_configuration_warnings);
	BIND_VIRTUAL_METHOD(T, _input);
	BIND_VIRTUAL_METHOD(T, _shortcut_input);
	BIND_VIRTUAL_METHOD(T, _unhandled_input);
	BIND_VIRTUAL_METHOD(T, _unhandled_key_input);
	*/

	const StringName name = String(class_name);
	const StringName parent_name = "Node";
	std::cout << "AH" << std::endl;
	internal::gdextension_interface_classdb_register_extension_class3(internal::library, name._native_ptr(), parent_name._native_ptr(), &class_info);
	std::cout << "aAH" << std::endl;

	// call bind_methods etc. to register all members of the class
	initialize_class();

	std::vector<PropertyInfo> return_value_and_arguments_info = getter_bind->get_arguments_info_list();
	std::vector<GDExtensionPropertyInfo> return_value_and_arguments_gdextension_info;
	return_value_and_arguments_gdextension_info.reserve(return_value_and_arguments_info.size());
	for (std::vector<PropertyInfo>::iterator it = return_value_and_arguments_info.begin(); it != return_value_and_arguments_info.end(); it++) {
		return_value_and_arguments_gdextension_info.push_back(
			GDExtensionPropertyInfo{
				static_cast<GDExtensionVariantType>(it->type), // GDExtensionVariantType type;
				it->name._native_ptr(), // GDExtensionStringNamePtr name;
				it->class_name._native_ptr(), // GDExtensionStringNamePtr class_name;
				it->hint, // uint32_t hint;
				it->hint_string._native_ptr(), // GDExtensionStringPtr hint_string;
				it->usage, // uint32_t usage;
			});
	}

	std::vector<PropertyInfo> return_value_and_arguments_info2 = setter_bind->get_arguments_info_list();
	std::vector<GDExtensionPropertyInfo> return_value_and_arguments_gdextension_info2;
	return_value_and_arguments_gdextension_info2.reserve(return_value_and_arguments_info2.size());
	for (std::vector<PropertyInfo>::iterator it = return_value_and_arguments_info2.begin(); it != return_value_and_arguments_info2.end(); it++) {
		return_value_and_arguments_gdextension_info2.push_back(
			GDExtensionPropertyInfo{
				static_cast<GDExtensionVariantType>(it->type), // GDExtensionVariantType type;
				it->name._native_ptr(), // GDExtensionStringNamePtr name;
				it->class_name._native_ptr(), // GDExtensionStringNamePtr class_name;
				it->hint, // uint32_t hint;
				it->hint_string._native_ptr(), // GDExtensionStringPtr hint_string;
				it->usage, // uint32_t usage;
			});
	}

	GDExtensionPropertyInfo *return_value_info = return_value_and_arguments_gdextension_info.data();
	GDExtensionPropertyInfo *return_value_info2 = return_value_and_arguments_gdextension_info2.data();

	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata = getter_bind->get_arguments_metadata_list();
	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata2 = setter_bind->get_arguments_metadata_list();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata = return_value_and_arguments_metadata.data();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata2 = return_value_and_arguments_metadata2.data();
	GDExtensionPropertyInfo *arguments_info = return_value_and_arguments_gdextension_info.data() + 1;
	GDExtensionPropertyInfo *arguments_info2 = return_value_and_arguments_gdextension_info2.data() + 1;
	GDExtensionClassMethodArgumentMetadata *arguments_metadata = return_value_and_arguments_metadata.data() + 1;
	GDExtensionClassMethodArgumentMetadata *arguments_metadata2 = return_value_and_arguments_metadata2.data() + 1;

	std::vector<GDExtensionVariantPtr> def_args;
	const std::vector<Variant> &def_args_val = getter_bind->get_default_arguments();
	def_args.resize(def_args_val.size());
	for (size_t i = 0; i < def_args_val.size(); i++) {
		def_args[i] = (GDExtensionVariantPtr)&def_args_val[i];
	}
	std::vector<GDExtensionVariantPtr> def_args2;
	const std::vector<Variant> &def_args_val2 = setter_bind->get_default_arguments();
	def_args2.resize(def_args_val2.size());
	for (size_t i = 0; i < def_args_val2.size(); i++) {
		def_args2[i] = (GDExtensionVariantPtr)&def_args_val2[i];
	}

	GDExtensionClassMethodInfo method_info = {
		getter->_native_ptr(), // GDExtensionStringNamePtr;
		getter_bind, // void *method_userdata;
		MethodBind::bind_call, // GDExtensionClassMethodCall call_func;
		MethodBind::bind_ptrcall, // GDExtensionClassMethodPtrCall ptrcall_func;
		getter_bind->get_hint_flags(), // uint32_t method_flags; /* GDExtensionClassMethodFlags */
		(GDExtensionBool)getter_bind->has_return(), // GDExtensionBool has_return_value;
		return_value_info, // GDExtensionPropertyInfo *
		*return_value_metadata, // GDExtensionClassMethodArgumentMetadata *
		(uint32_t)getter_bind->get_argument_count(), // uint32_t argument_count;
		arguments_info, // GDExtensionPropertyInfo *
		arguments_metadata, // GDExtensionClassMethodArgumentMetadata *
		(uint32_t)getter_bind->get_default_argument_count(), // uint32_t default_argument_count;
		def_args.data(), // GDExtensionVariantPtr *default_arguments;
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, class_nam->_native_ptr(), &method_info);

	GDExtensionClassMethodInfo method_info2 = {
		setter->_native_ptr(), // GDExtensionStringNamePtr;
		setter_bind, // void *method_userdata;
		MethodBind::bind_call, // GDExtensionClassMethodCall call_func;
		MethodBind::bind_ptrcall, // GDExtensionClassMethodPtrCall ptrcall_func;
		setter_bind->get_hint_flags(), // uint32_t method_flags; /* GDExtensionClassMethodFlags */
		(GDExtensionBool)setter_bind->has_return(), // GDExtensionBool has_return_value;
		return_value_info2, // GDExtensionPropertyInfo *
		*return_value_metadata2, // GDExtensionClassMethodArgumentMetadata *
		(uint32_t)setter_bind->get_argument_count(), // uint32_t argument_count;
		arguments_info2, // GDExtensionPropertyInfo *
		arguments_metadata2, // GDExtensionClassMethodArgumentMetadata *
		(uint32_t)setter_bind->get_default_argument_count(), // uint32_t default_argument_count;
		def_args2.data(), // GDExtensionVariantPtr *default_arguments;
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, class_nam->_native_ptr(), &method_info2);
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, class_nam->_native_ptr(), &method_info2);
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, class_nam->_native_ptr(), &method_info);
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, class_nam->_native_ptr(), &method_info);

	internal::gdextension_interface_classdb_register_extension_class_property_indexed(internal::library, class_nam->_native_ptr(), &prop_info, setter->_native_ptr(), getter->_native_ptr(), -1);
}

GDExtensionClassCallVirtual GenericAnchorResource::get_virtual_func(void *p_userdata, GDExtensionConstStringNamePtr p_name) {
	// This is called by Godot the first time it calls a virtual function, and it caches the result, per object instance.
	// Because of this, it can happen from different threads at once.
	// It should be ok not using any mutex as long as we only READ data.
	const StringName *class_name = reinterpret_cast<const StringName *>(p_userdata);
	const StringName *name = reinterpret_cast<const StringName *>(p_name);
/*
	if constexpr (!std::is_same_v<decltype(&GenericAnchorResource::_process),decltype(&Node::_process)>) {
		auto _call_process = [](GDExtensionObjectPtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr p_ret) -> void {
			call_with_ptr_args(reinterpret_cast<GenericAnchorResource *>(p_instance), &GenericAnchorResource::_process, p_args, p_ret);
		};

		if((*name) == String("_process")){
			return _call_process;
		}
	}*/
	std::cout << "AAAAAAAAAAAAAAAAAfix " <<  String((*name)).ascii() << std::endl;
	return nullptr;
}

void GenericAnchorResource::add_property(const Dictionary &property_data) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!property_data.has("name"), "Property does not have name.");

	const String property_name = property_data["name"];
	std::cout << "TYPE " << AnchorProgram::get_godot_type(property_data["type"]);
	ResourcePropertyInfo abc = { 0 };
	//properties.push_back(abc);
}

PackedByteArray GenericAnchorResource::serialize() {
}

} //namespace godot