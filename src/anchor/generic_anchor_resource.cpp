#include "anchor/generic_anchor_resource.hpp"

#include <functional>

#include "gdextension_interface.h"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

// TODO(Virax): Delete this memory as well.
std::vector<String *> GenericAnchorResource::names;
std::string GenericAnchorResource::string_name = "GenericAnchorResource";
std::unordered_map<StringName, std::unordered_map<StringName, ResourcePropertyInfo>> GenericAnchorResource::property_database;
std::unordered_map<StringName, Array> GenericAnchorResource::enum_field_map;
std::string prop_name = "aaa";
std::string instruction_name = "";

bool GenericAnchorResource::_set(const StringName &p_name, const Variant &p_value) { // NOLINT(bugprone-easily-swappable-parameters)
	const String name = p_name;

	property_database[get_class_static()][p_name].value = p_value;
	return true;
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

	//r_ret = false;
	r_ret = property_database[get_class_static()][p_name].value;
	return true;

	for (const auto &property : properties) {
		if (property.property_info.name == name) {
			r_ret = property.value;
			return true;
		}
	}

	return true;
}
/*
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
*/
GDExtensionObjectPtr GenericAnchorResource::_create_instance_func(void *data) {
	const String instance_class = *(String *)data;
	ResourcePropertyInfo info{
		.property_info = PropertyInfo(Variant::STRING, instance_class),
		.value = "nullptr",
		.optional = false,
		.enabled = true,
	};
	set_class_name(instance_class);
	GenericAnchorResource *new_object = memnew(GenericAnchorResource);
	//new_object->properties.push_back(info);
	//new_object->notify_property_list_changed();
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
	Resource::initialize_class();
	if (GenericAnchorResource::_get_bind_methods() != Resource::_get_bind_methods()) {
		_bind_methods();
		Resource::register_virtuals<GenericAnchorResource, Resource>();
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

void GenericAnchorResource::bind_resource_class(const StringName &p_class_name, const StringName &parent_name) {
	GDExtensionClassCreationInfo3 class_info = {
		false, // GDExtensionBool is_virtual;
		false, // GDExtensionBool is_abstract;
		true, // GDExtensionBool is_exposed;
		false, // GDExtensionBool is_runtime;
		GenericAnchorResource::set_bind, // GDExtensionClassSet set_func;
		GenericAnchorResource::get_bind, // GDExtensionClassGet get_func;
		GenericAnchorResource::has_get_property_list() ? GenericAnchorResource::get_property_list_bind : nullptr, // GDExtensionClassGetPropertyList get_property_list_func;
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

	internal::gdextension_interface_classdb_register_extension_class3(internal::library, p_class_name._native_ptr(), parent_name._native_ptr(), &class_info);
}

void GenericAnchorResource::bind_resource_method(const StringName &p_class_name, const MethodDefinition &method_prototype, MethodBind *p_method) {
	p_method->set_name(method_prototype.name);

	std::vector<StringName> args(method_prototype.args.begin(), method_prototype.args.end());

	p_method->set_argument_names(args);

	const std::vector<PropertyInfo> return_value_and_arguments_info = p_method->get_arguments_info_list();
	std::vector<GDExtensionPropertyInfo> return_value_and_arguments_gdextension_info;
	return_value_and_arguments_gdextension_info.reserve(return_value_and_arguments_info.size());
	for (const auto &argument_info : return_value_and_arguments_info) {
		return_value_and_arguments_gdextension_info.push_back(
				GDExtensionPropertyInfo{
						static_cast<GDExtensionVariantType>(argument_info.type),
						argument_info.name._native_ptr(),
						argument_info.class_name._native_ptr(),
						argument_info.hint,
						argument_info.hint_string._native_ptr(),
						argument_info.usage,
				});
	}

	GDExtensionPropertyInfo *return_value_info = return_value_and_arguments_gdextension_info.data();

	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata = p_method->get_arguments_metadata_list();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata = return_value_and_arguments_metadata.data();
	GDExtensionPropertyInfo *arguments_info = return_value_and_arguments_gdextension_info.data() + 1;
	GDExtensionClassMethodArgumentMetadata *arguments_metadata = return_value_and_arguments_metadata.data() + 1;

	std::vector<GDExtensionVariantPtr> def_args;
	const std::vector<Variant> &def_args_val = p_method->get_default_arguments();
	def_args.resize(def_args_val.size());
	for (size_t i = 0; i < def_args_val.size(); i++) {
		def_args[i] = (GDExtensionVariantPtr)&def_args_val[i];
	}

	const StringName name = p_method->get_name();
	const GDExtensionClassMethodInfo method_info = {
		name._native_ptr(),
		p_method,
		MethodBind::bind_call,
		MethodBind::bind_ptrcall,
		p_method->get_hint_flags(),
		static_cast<GDExtensionBool>(p_method->has_return()),
		return_value_info,
		*return_value_metadata,
		static_cast<uint32_t>(p_method->get_argument_count()),
		arguments_info,
		arguments_metadata,
		static_cast<uint32_t>(p_method->get_default_argument_count()),
		def_args.data(),
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, p_class_name._native_ptr(), &method_info);
}

void GenericAnchorResource::bind_resource_getter(const StringName &p_class_name, const MethodDefinition &method_prototype, const StringName &property_name) {
	MethodBind *getter_bind = create_method_bind(&GenericAnchorResource::no_get);
	getter_bind->set_name(method_prototype.name);

	std::vector<StringName> args(method_prototype.args.begin(), method_prototype.args.end());

	getter_bind->set_argument_names(args);

	const std::vector<PropertyInfo> return_value_and_arguments_info = getter_bind->get_arguments_info_list();
	std::vector<GDExtensionPropertyInfo> return_value_and_arguments_gdextension_info;
	return_value_and_arguments_gdextension_info.reserve(return_value_and_arguments_info.size());
	for (const auto &argument_info : return_value_and_arguments_info) {
		return_value_and_arguments_gdextension_info.push_back(
				GDExtensionPropertyInfo{
						static_cast<GDExtensionVariantType>(argument_info.type),
						argument_info.name._native_ptr(),
						argument_info.class_name._native_ptr(),
						argument_info.hint,
						argument_info.hint_string._native_ptr(),
						argument_info.usage,
				});
	}

	GDExtensionPropertyInfo *return_value_info = return_value_and_arguments_gdextension_info.data();

	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata = getter_bind->get_arguments_metadata_list();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata = return_value_and_arguments_metadata.data();
	GDExtensionPropertyInfo *arguments_info = return_value_and_arguments_gdextension_info.data() + 1;
	GDExtensionClassMethodArgumentMetadata *arguments_metadata = return_value_and_arguments_metadata.data() + 1;

	std::vector<GDExtensionVariantPtr> def_args;
	const std::vector<Variant> &def_args_val = getter_bind->get_default_arguments();
	def_args.resize(def_args_val.size());
	for (size_t i = 0; i < def_args_val.size(); i++) {
		def_args[i] = (GDExtensionVariantPtr)&def_args_val[i];
	}

	prop_name = String(property_name).ascii();
	auto *lambda = new auto([](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstVariantPtr *p_args, GDExtensionInt p_argument_count, GDExtensionVariantPtr r_return, GDExtensionCallError *r_error) {
		static String abc = String(prop_name.c_str());
		static const String cla = GenericAnchorResource::get_class_static();
		if (p_instance == nullptr) {
			return;
		}
		Variant ret;
		GenericAnchorResource::set_class_name(cla);
		bool status = static_cast<GenericAnchorResource *>(p_instance)->_get(abc, ret);
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	});
	(*lambda)(nullptr, nullptr, nullptr, 0, nullptr, nullptr);

	auto *lambda2 = new auto([](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_return) {
		Variant ret = {};
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	});

	const StringName name = getter_bind->get_name();
	const GDExtensionClassMethodInfo method_info = {
		name._native_ptr(),
		getter_bind,
		*lambda,
		*lambda2,
		getter_bind->get_hint_flags(),
		static_cast<GDExtensionBool>(getter_bind->has_return()),
		return_value_info,
		*return_value_metadata,
		static_cast<uint32_t>(getter_bind->get_argument_count()),
		arguments_info,
		arguments_metadata,
		static_cast<uint32_t>(getter_bind->get_default_argument_count()),
		def_args.data(),
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, p_class_name._native_ptr(), &method_info);
}

void GenericAnchorResource::bind_resource_setter(const StringName &p_class_name, const MethodDefinition &method_prototype, const StringName &property_name) {
	MethodBind *setter_bind = create_method_bind(&GenericAnchorResource::no_set);
	setter_bind->set_name(method_prototype.name);

	std::vector<StringName> args(method_prototype.args.begin(), method_prototype.args.end());

	setter_bind->set_argument_names(args);

	const std::vector<PropertyInfo> return_value_and_arguments_info = setter_bind->get_arguments_info_list();
	std::vector<GDExtensionPropertyInfo> return_value_and_arguments_gdextension_info;
	return_value_and_arguments_gdextension_info.reserve(return_value_and_arguments_info.size());
	for (const auto &argument_info : return_value_and_arguments_info) {
		return_value_and_arguments_gdextension_info.push_back(
			GDExtensionPropertyInfo{
				static_cast<GDExtensionVariantType>(argument_info.type),
				argument_info.name._native_ptr(),
				argument_info.class_name._native_ptr(),
				argument_info.hint,
				argument_info.hint_string._native_ptr(),
				argument_info.usage,
			});
	}

	GDExtensionPropertyInfo *return_value_info = return_value_and_arguments_gdextension_info.data();

	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata = setter_bind->get_arguments_metadata_list();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata = return_value_and_arguments_metadata.data();
	GDExtensionPropertyInfo *arguments_info = return_value_and_arguments_gdextension_info.data() + 1;
	GDExtensionClassMethodArgumentMetadata *arguments_metadata = return_value_and_arguments_metadata.data() + 1;

	std::vector<GDExtensionVariantPtr> def_args;
	const std::vector<Variant> &def_args_val = setter_bind->get_default_arguments();
	def_args.resize(def_args_val.size());
	for (size_t i = 0; i < def_args_val.size(); i++) {
		def_args[i] = (GDExtensionVariantPtr)&def_args_val[i];
	}

	prop_name = String(property_name).ascii();
	auto *lambda = new auto([](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstVariantPtr *p_args, GDExtensionInt p_argument_count, GDExtensionVariantPtr r_return, GDExtensionCallError *r_error) {
		static String abc = String(prop_name.c_str());
		static const String cla = GenericAnchorResource::get_class_static();
		if (p_instance == nullptr) {
			return;
		}
		Variant arg = *p_args;
		GenericAnchorResource::set_class_name(cla);
		static_cast<GenericAnchorResource *>(p_instance)->_set(abc, arg);
		Variant ret = {};
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	});
	(*lambda)(nullptr, nullptr, nullptr, 0, nullptr, nullptr);
	auto *lambda2 = new auto([](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_return) {
		Variant ret = {};
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	});

	const StringName name = setter_bind->get_name();
	const GDExtensionClassMethodInfo method_info = {
		name._native_ptr(),
		setter_bind,
		*lambda,
		*lambda2,
		setter_bind->get_hint_flags(),
		static_cast<GDExtensionBool>(setter_bind->has_return()),
		return_value_info,
		*return_value_metadata,
		static_cast<uint32_t>(setter_bind->get_argument_count()),
		arguments_info,
		arguments_metadata,
		static_cast<uint32_t>(setter_bind->get_default_argument_count()),
		def_args.data(),
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, p_class_name._native_ptr(), &method_info);
}

void GenericAnchorResource::bind_resource_property(const StringName &p_class_name, const PropertyInfo &property_info, const StringName &setter_name, const StringName &getter_name) {
	const StringName new_setter_name = (setter_name.is_empty()) ? StringName("set_" + property_info.name) : setter_name;
	const StringName new_getter_name = (getter_name.is_empty()) ? StringName("get_" + property_info.name) : getter_name;

	const GDExtensionPropertyInfo prop_info = {
		static_cast<GDExtensionVariantType>(property_info.type), // GDExtensionVariantType type;
		property_info.name._native_ptr(), // GDExtensionStringNamePtr name;
		p_class_name._native_ptr(), // GDExtensionStringNamePtr class_name;
		property_info.hint, // NONE //uint32_t hint;
		property_info.hint_string._native_ptr(), // GDExtensionStringPtr hint_string;
		property_info.usage, // DEFAULT //uint32_t usage;
	};

	internal::gdextension_interface_classdb_register_extension_class_property_indexed(internal::library, p_class_name._native_ptr(), &prop_info, new_setter_name._native_ptr(), new_getter_name._native_ptr(), -1);
}

const StringName &GenericAnchorResource::get_class_static() {
	static StringName string_name_gd = "GenericAnchorResource";
	string_name_gd = *memnew(String(string_name.c_str()));
	return string_name_gd;
}

const StringName &GenericAnchorResource::get_parent_class_static() {
	return Resource::get_class_static();
}

void GenericAnchorResource::notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed) {
	if (p_instance && GenericAnchorResource::_get_notification()) {
		if (!p_reversed) {
			Resource::notification_bind(p_instance, p_what, p_reversed);
		}
		if (GenericAnchorResource::_get_notification() != Resource::_get_notification()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			cls->_notification(p_what);
		}
		if (p_reversed) {
			Resource::notification_bind(p_instance, p_what, p_reversed);
		}
	}
}

GDExtensionBool GenericAnchorResource::set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value) {
	if (p_instance) {
		if (Resource::set_bind(p_instance, p_name, p_value)) {
			return true;
		}
		if (GenericAnchorResource::_get_set() != Resource::_get_set()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			return cls->_set(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<const Variant *>(p_value));
		}
	}
	return false;
}

GDExtensionBool GenericAnchorResource::get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance) {
		if (Resource::get_bind(p_instance, p_name, r_ret)) {
			return true;
		}
		if (GenericAnchorResource::_get_get() != Resource::_get_get()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			return cls->_get(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<Variant *>(r_ret));
		}
	}
	return false;
}

bool GenericAnchorResource::has_get_property_list() {
	return GenericAnchorResource::_get_get_property_list() && GenericAnchorResource::_get_get_property_list() != Resource::_get_get_property_list();
}

const GDExtensionPropertyInfo *GenericAnchorResource::get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count) {
	if (!p_instance) {
		if (r_count)
			*r_count = 0;
		return nullptr;
	}
	GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
	List<PropertyInfo> &plist_cpp = cls->plist_owned;
	ERR_FAIL_COND_V_MSG(!plist_cpp.is_empty(), nullptr, "Internal error, property list was not freed by engine!");
	cls->_get_property_list(&plist_cpp);
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
		if (GenericAnchorResource::_get_property_can_revert() != Resource::_get_property_can_revert()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			return cls->_property_can_revert(*reinterpret_cast<const StringName *>(p_name));
		}
		return Resource::property_can_revert_bind(p_instance, p_name);
	}
	return false;
}

GDExtensionBool GenericAnchorResource::property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance && GenericAnchorResource::_get_property_get_revert()) {
		if (GenericAnchorResource::_get_property_get_revert() != Resource::_get_property_get_revert()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			return cls->_property_get_revert(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<Variant *>(r_ret));
		}
		return Resource::property_get_revert_bind(p_instance, p_name, r_ret);
	}
	return false;
}

GDExtensionBool GenericAnchorResource::validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property) {
	bool ret = false;
	if (p_instance && GenericAnchorResource::_get_validate_property()) {
		ret = Resource::validate_property_bind(p_instance, p_property);
		if (GenericAnchorResource::_get_validate_property() != Resource::_get_validate_property()) {
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
		if (GenericAnchorResource::_get_to_string() != Resource::_get_to_string()) {
			GenericAnchorResource *cls = reinterpret_cast<GenericAnchorResource *>(p_instance);
			*reinterpret_cast<String *>(r_out) = cls->_to_string();
			*r_is_valid = true;
			return;
		}
		Resource::to_string_bind(p_instance, r_is_valid, r_out);
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

void GenericAnchorResource::bind_anchor_enum(const Dictionary &enum_data) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!enum_data.has("name"), "Enum data struct does not contain a name.");
	ERR_FAIL_COND_EDMSG_CUSTOM(!enum_data.has("type"), "Enum data struct does not contain a type.");

	const String enum_name = enum_data["name"];
	const Dictionary enum_type = enum_data["type"];

	ERR_FAIL_COND_EDMSG_CUSTOM(!enum_type.has("kind"), "Enum type does not have kind field");
	ERR_FAIL_COND_EDMSG_CUSTOM(enum_type["kind"] != "enum", "Kind is not enum");
	ERR_FAIL_COND_EDMSG_CUSTOM(!enum_type.has("variants"), "Enum type does not have variants field");

	const Array variants = enum_type["variants"];
	enum_field_map[enum_name] = variants;

	for (unsigned int i = 0; i < variants.size(); i++) {
		const Dictionary variant = variants[i];
		if (variant.has("name")) {
			const StringName p_class_name = "GenericAnchorResource";
			const StringName p_enum_name = enum_name;
			const StringName p_constant_name = enum_name + String(variant["name"]);
			internal::gdextension_interface_classdb_register_extension_class_integer_constant(internal::library, p_class_name._native_ptr(), p_enum_name._native_ptr(), p_constant_name._native_ptr(), i, false);

			//enum_field_map[p_constant_name] = Array();
			//std::cout << "ADDING TO DB " << String(p_constant_name).ascii() << std::endl;
			//ClassDB::bind_integer_constant("GenericAnchorResource", _gde_constant_get_enum_name(i, enum_name), variant["name"], i);
		}
		/*if(variant.has("fields")){
			const StringName p_constant_name = enum_name + String(variant["name"]);
			const Array variant_fields = variant["fields"];
			if(enum_field_map.find(p_constant_name) == enum_field_map.end()){
				enum_field_map[p_constant_name] = variant_fields;
			}
		}
		
		if(variant.has("fields")){
			const StringName p_constant_name = enum_name + String(variant["name"]);
			Array map_array;
			if(enum_field_map.find(p_constant_name) != enum_field_map.end()){
				map_array = enum_field_map[p_constant_name];
			}

			const Array variant_fields = variant["fields"];
			for(unsigned int j = 0; j < variant_fields.size(); j++){
				const Dictionary field_type = variant_fields[j];
				const Variant::Type godot_type = AnchorProgram::get_godot_type(field_type);
				const PropertyHint godot_hint = AnchorProgram::get_godot_hint(field_type);
				const String godot_class_hint = AnchorProgram::get_godot_class_hint(field_type);

				Dictionary field_property_info;
				field_property_info["name"] = variant["name"] + "_value_" + String::num_uint64(j);
				field_property_info["type"] = field_type;

				add_property(field_property_info);
			}

			enum_field_map[enum_name];
		}*/
	}
}

void GenericAnchorResource::bind_anchor_resource(const Dictionary &resource) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!resource.has("name"), "Resource struct does not contain a name.");
	ERR_FAIL_COND_CUSTOM(resource.has("vec"));
	ERR_FAIL_COND_CUSTOM(resource.has("array"));

	const String class_name = resource["name"];
	set_class_name(class_name);
	//const String class_name = "GenericAnchorResource";

	names.push_back(memnew(String(class_name)));

	const Dictionary struct_info = resource["type"];
	UtilityFunctions::print(struct_info);

	ERR_FAIL_COND_EDMSG_CUSTOM(!struct_info.has("fields"), "Resource struct does not contain any fields");

	const Array fields = struct_info["fields"];
	bind_resource_class(*names[names.size() - 1], "Resource");
	initialize_class();

	for (unsigned int i = 0; i < fields.size(); i++) {
		add_property(fields[i]);
	}

	MethodBind *method_bind = create_method_bind(&GenericAnchorResource::serialize);
	bind_resource_method(get_class_static(), D_METHOD("serialize"), method_bind);
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
	return nullptr;
}

void GenericAnchorResource::add_enum_properties(const Dictionary &property_data) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!property_data.has("name"), "Enum property does not have name.");
}

void GenericAnchorResource::add_property(const Dictionary &property_data) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!property_data.has("name"), "Property does not have name.");

	const String property_name = property_data["name"];

	const String object_type = AnchorProgram::get_object_name(property_data["type"]);

	if (enum_field_map.find(object_type) != enum_field_map.end()) {
		const Variant::Type property_godot_type = Variant::Type::INT;
		const String hint_string = "";//object_type;
		const PropertyHint hint = PROPERTY_HINT_NONE;

		ResourcePropertyInfo prop_info = ResourcePropertyInfo{
			.property_info = PropertyInfo(property_godot_type, property_name, hint, hint_string),
			.value = Variant(),
			.optional = false,
			.enabled = true,
		};
		property_database[get_class_static()][property_name] = prop_info;

		bind_resource_setter(*names[names.size() - 1], D_METHOD("set_" + property_name, "value"), property_name);
		bind_resource_getter(*names[names.size() - 1], D_METHOD("get_" + property_name), property_name);

		bind_resource_property(get_class_static(), PropertyInfo(property_godot_type, property_name, hint, hint_string));

		// Extra props
		const Array field_types = enum_field_map[object_type];
		for(unsigned int i = 0; i < field_types.size(); i++){
			const Dictionary field_type = field_types[i];
			if(!field_type.has("fields")){
				continue;
			}

			const String value_field_name = field_type["name"];
			const Array value_field_types = field_type["fields"];

			for(unsigned int j = 0; j < value_field_types.size(); j++){
				const Dictionary value_field_type = value_field_types[j];
				const String enum_value_name = value_field_name + String("_value_") + String::num_uint64(j);
				if (property_database[get_class_static()].find(enum_value_name) != property_database[get_class_static()].end()){
					continue;
				}
				const Variant::Type enum_value_godot_type = AnchorProgram::get_godot_type(value_field_type);
				const String enum_value_hint_string = AnchorProgram::get_godot_class_hint(value_field_type);
				const PropertyHint enum_value_hint = AnchorProgram::get_godot_hint(value_field_type);

				ResourcePropertyInfo enum_value_info = ResourcePropertyInfo{
					.property_info = PropertyInfo(enum_value_godot_type, enum_value_name, enum_value_hint, enum_value_hint_string),
					.value = Variant(),
					.optional = false,
					.enabled = true,
				};
				property_database[get_class_static()][enum_value_name] = enum_value_info;
		
				std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< " << enum_value_name.ascii() << std::endl;
				bind_resource_setter(get_class_static(), D_METHOD("set_" + enum_value_name, "value"), enum_value_name);
				bind_resource_getter(get_class_static(), D_METHOD("get_" + enum_value_name), enum_value_name);
		
				bind_resource_property(get_class_static(), PropertyInfo(enum_value_godot_type, enum_value_name, enum_value_hint, enum_value_hint_string));
			}
		}
	} else {
		const Variant::Type property_godot_type = AnchorProgram::get_godot_type(property_data["type"]);
		const String hint_string = AnchorProgram::get_godot_class_hint(property_data["type"]);
		const PropertyHint hint = AnchorProgram::get_godot_hint(property_data["type"]);

		ResourcePropertyInfo prop_info = ResourcePropertyInfo{
			.property_info = PropertyInfo(property_godot_type, property_name, hint, hint_string),
			.value = Variant(),
			.optional = false,
			.enabled = true,
		};
		property_database[get_class_static()][property_name] = prop_info;

		bind_resource_setter(get_class_static(), D_METHOD("set_" + property_name, "value"), property_name);
		bind_resource_getter(get_class_static(), D_METHOD("get_" + property_name), property_name);

		bind_resource_property(get_class_static(), PropertyInfo(property_godot_type, property_name, hint, hint_string));
	}

	//properties.push_back(abc);
}

PackedByteArray GenericAnchorResource::serialize() {
	return PackedByteArray();
}

} //namespace godot