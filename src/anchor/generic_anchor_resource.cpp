#include "anchor/generic_anchor_resource.hpp"

#include <cstdint>
#include <functional>

#include "gdextension_interface.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#include "anchor_program.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

// TODO(Virax): Delete this memory as well.
std::vector<StringName *> GenericAnchorResource::names;
std::string GenericAnchorResource::string_name = "GenericAnchorResource";
std::unordered_map<StringName, std::unordered_map<StringName, ResourcePropertyInfo>> GenericAnchorResource::property_database;
std::unordered_map<StringName, std::vector<StringName>> GenericAnchorResource::property_order;
std::unordered_map<StringName, Array> GenericAnchorResource::enum_field_map;
std::unordered_map<StringName, std::vector<StringName>> GenericAnchorResource::extra_props;
std::string prop_name = "aaa";
std::string instruction_name = "";

bool GenericAnchorResource::_set(const StringName &p_name, const Variant &p_value) { // NOLINT(bugprone-easily-swappable-parameters)
	const String name = p_name;

	if (properties.find(p_name) == properties.end()) {
		return false;
	}

	if (properties[p_name].property_info.hint == PROPERTY_HINT_ENUM) {
		const StringName class_hint_name = properties[p_name].property_info.class_name;
		if (enum_field_map.find(class_hint_name) != enum_field_map.end()) {
			const Dictionary disable_enum_variant = enum_field_map[class_hint_name][static_cast<int64_t>(properties[p_name].value)];
			if (extra_props.find(disable_enum_variant["name"]) != extra_props.end()) {
				const std::vector disable_props = extra_props[disable_enum_variant["name"]]; // get index (p_value) of enum variants and get string. value now disable all, value later enable
				for (auto disable_prop : disable_props) {
					properties[disable_prop].enabled = false;
				}
			}

			const Dictionary enable_enum_variant = enum_field_map[class_hint_name][static_cast<int64_t>(p_value)];
			if (extra_props.find(enable_enum_variant["name"]) != extra_props.end()) {
				const std::vector enable_props = extra_props[enable_enum_variant["name"]]; // get index (p_value) of enum variants and get string. value now disable all, value later enable
				for (auto enable_prop : enable_props) {
					properties[enable_prop].enabled = true;
				}
			}
			notify_property_list_changed();
		}
	}

	// If setting enabler property.
	properties[p_name].value = p_value;
	if (!properties[p_name].serialize && p_name.begins_with("enable_")) {
		if (properties.find(p_name.replace("enable_", "")) != properties.end()) {
			properties[p_name.replace("enable_", "")].enabled = p_value;
			notify_property_list_changed();
		}
	}

	// Check if has enabler that is not checked (setting from script).
	/*else if (properties.find("enable_" + p_name) != properties.end()) {
		if (!properties["enable_" + p_name].value) {
			properties["enable_" + p_name].value = true;
			properties[p_name].enabled = true;
		}
	}*/

	return true;
	//property_database[get_class_static()][p_name].value = p_value;
	/*return true;
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

	return true;*/
}

bool GenericAnchorResource::_get(const StringName &p_name, Variant &r_ret) const {
	const String name = p_name;

	//r_ret = false;
	//r_ret = property_database[get_class_static()][p_name].value;
	if (properties.find(p_name) == properties.end()) {
		return false;
	}

	r_ret = properties.at(p_name).value;
	return true;
	/*
		for (const auto &property : properties) {
			if (property.property_info.name == name) {
				r_ret = property.value;
				return true;
			}
		}

		return true;*/
}

GDExtensionObjectPtr GenericAnchorResource::_create_instance_func(void *data) {
	const String instance_class = *(StringName *)data;

	set_class_name(instance_class);
	GenericAnchorResource *new_object = memnew(GenericAnchorResource);
	new_object->local_name = instance_class;
	new_object->properties = property_database[instance_class];
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

BindMethodFunc GenericAnchorResource::_get_bind_methods() {
	return &GenericAnchorResource::_bind_methods;
}

NotificationMethod GenericAnchorResource::_get_notification() {
	return (void(::godot::Wrapped::*)(int)) & GenericAnchorResource::_notification;
}

SetMethod GenericAnchorResource::_get_set() {
	return (bool(Wrapped::*)(const StringName &p_name, const Variant &p_property)) & GenericAnchorResource::_set;
}

GetMethod GenericAnchorResource::_get_get() {
	return (bool(Wrapped::*)(const StringName &p_name, Variant &r_ret) const) & GenericAnchorResource::_get;
}

GetPropertyListMethod GenericAnchorResource::_get_get_property_list() {
	return (void(Wrapped::*)(List<PropertyInfo> * p_list) const) & GenericAnchorResource::_get_property_list;
}

PropertyCanRevertMethod GenericAnchorResource::_get_property_can_revert() {
	return (bool(Wrapped::*)(const StringName &p_name) const) & GenericAnchorResource::_property_can_revert;
}

PropertyGetRevertMethod GenericAnchorResource::_get_property_get_revert(){
	return (bool(Wrapped::*)(const StringName &p_name, Variant &) const) & GenericAnchorResource::_property_get_revert;
}

ValidatePropertyMethod GenericAnchorResource::_get_validate_property() {
	return (void(Wrapped::*)(PropertyInfo & p_property) const) & GenericAnchorResource::_validate_property;
}

ToStringMethod GenericAnchorResource::_get_to_string() {
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
		&ClassDB::get_virtual_func, // GDExtensionClassGetVirtual get_virtual_func;
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

String GenericAnchorResource::get_enum_hint(const StringName &enum_type) {
	ERR_FAIL_COND_V(enum_field_map.find(enum_type) == enum_field_map.end(), "");
	const Array enum_variants = enum_field_map[enum_type];

	String result = "";
	for (unsigned int i = 0; i < enum_variants.size(); i++) {
		const Dictionary enum_name = enum_variants[i];
		result += String(String(enum_type) + String(enum_name["name"])) + ",";
	}
	if (!result.is_empty()) {
		return result.substr(0, result.length() - 1);
	}
	return result;
}

Variant GenericAnchorResource::godot_type_defval(const Variant::Type type_name) {
	switch (type_name) {
		case Variant::INT:
			return 0;
			break;
		case Variant::FLOAT:
			return 0.0;
			break;
		case Variant::BOOL:
			return false;
			break;
		case Variant::STRING:
			return "";
			break;
		case Variant::PACKED_BYTE_ARRAY:
		case Variant::PACKED_STRING_ARRAY:
		case Variant::ARRAY:
			return {};
			break;
		default:
			return Variant();
			break;
	}
}

const StringName &GenericAnchorResource::get_class_static() {
	static StringName string_name_gd = "GenericAnchorResource";
	string_name_gd = *memnew_custom(String(string_name.c_str()));
	return string_name_gd;
}

const StringName &GenericAnchorResource::get_parent_class_static() {
	return Resource::get_class_static();
}

void GenericAnchorResource::notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed) {
	if ((p_instance != nullptr) && (GenericAnchorResource::_get_notification() != nullptr)) {
		if (p_reversed == 0U) {
			Resource::notification_bind(p_instance, p_what, p_reversed);
		}
		if (GenericAnchorResource::_get_notification() != Resource::_get_notification()) {
			auto *cls = static_cast<GenericAnchorResource *>(p_instance);
			cls->_notification(p_what);
		}
		if (p_reversed != 0U) {
			Resource::notification_bind(p_instance, p_what, p_reversed);
		}
	}
}

GDExtensionBool GenericAnchorResource::set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value) {
	if (p_instance != nullptr) {
		if (Resource::set_bind(p_instance, p_name, p_value) != 0U) {
			return static_cast<GDExtensionBool>(true);
		}
		if (GenericAnchorResource::_get_set() != Resource::_get_set()) {
			auto *cls = static_cast<GenericAnchorResource *>(p_instance);
			return static_cast<GDExtensionBool>(cls->_set(*static_cast<const StringName *>(p_name), *static_cast<const Variant *>(p_value)));
		}
	}
	return 0U;
}

GDExtensionBool GenericAnchorResource::get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance != nullptr) {
		if (Resource::get_bind(p_instance, p_name, r_ret) != 0U) {
			return 1U;
		}
		if (GenericAnchorResource::_get_get() != Resource::_get_get()) {
			auto *cls = static_cast<GenericAnchorResource *>(p_instance);
			return static_cast<GDExtensionBool>(cls->_get(*static_cast<const StringName *>(p_name), *static_cast<Variant *>(r_ret)));
		}
	}
	return 0U;
}

bool GenericAnchorResource::has_get_property_list() {
	return ((GenericAnchorResource::_get_get_property_list() != nullptr) && GenericAnchorResource::_get_get_property_list() != Resource::_get_get_property_list());
}

const GDExtensionPropertyInfo *GenericAnchorResource::get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count) {
	if (p_instance == nullptr) {
		if (r_count != nullptr) {
			*r_count = 0;
		}
		return nullptr;
	}
	auto *cls = static_cast<GenericAnchorResource *>(p_instance);
	List<PropertyInfo> &plist_cpp = cls->plist_owned;
	ERR_FAIL_COND_V_MSG_CUSTOM(!plist_cpp.is_empty(), nullptr, "Internal error, property list was not freed by engine!");
	cls->_get_property_list(&plist_cpp);
	return internal::create_c_property_list(plist_cpp, r_count);
}

void GenericAnchorResource::free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/) {
	if (p_instance != nullptr) {
		auto *cls = static_cast<GenericAnchorResource *>(p_instance);
		cls->plist_owned.clear();
		internal::free_c_property_list(const_cast<GDExtensionPropertyInfo *>(p_list));
	}
}

GDExtensionBool GenericAnchorResource::property_can_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name) {
	if ((p_instance != nullptr) && (GenericAnchorResource::_get_property_can_revert() != nullptr)) {
		if (GenericAnchorResource::_get_property_can_revert() != Resource::_get_property_can_revert()) {
			auto *cls = static_cast<GenericAnchorResource *>(p_instance);
			return static_cast<GDExtensionBool>(cls->_property_can_revert(*static_cast<const StringName *>(p_name)));
		}
		return Resource::property_can_revert_bind(p_instance, p_name);
	}
	return 0U;
}

GDExtensionBool GenericAnchorResource::property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if ((p_instance != nullptr) && (GenericAnchorResource::_get_property_get_revert() != nullptr)) {
		if (GenericAnchorResource::_get_property_get_revert() != Resource::_get_property_get_revert()) {
			auto *cls = static_cast<GenericAnchorResource *>(p_instance);
			return static_cast<GDExtensionBool>(cls->_property_get_revert(*static_cast<const StringName *>(p_name), *static_cast<Variant *>(r_ret)));
		}
		return Resource::property_get_revert_bind(p_instance, p_name, r_ret);
	}
	return 0U;
}

GDExtensionBool GenericAnchorResource::validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property) {
	bool ret = false;
	if ((p_instance != nullptr) && (GenericAnchorResource::_get_validate_property() != nullptr)) {
		ret = (Resource::validate_property_bind(p_instance, p_property) != 0);
		if (GenericAnchorResource::_get_validate_property() != Resource::_get_validate_property()) {
			auto *cls = static_cast<GenericAnchorResource *>(p_instance);
			PropertyInfo info(p_property);
			cls->_validate_property(info);
			info._update(p_property);
			return 1U;
		}
	}
	return static_cast<GDExtensionBool>(ret);
}

void GenericAnchorResource::to_string_bind(GDExtensionClassInstancePtr p_instance, GDExtensionBool *r_is_valid, GDExtensionStringPtr r_out) {
	if ((p_instance != nullptr) && (GenericAnchorResource::_get_to_string() != nullptr)) {
		if (GenericAnchorResource::_get_to_string() != Resource::_get_to_string()) {
			auto *cls = static_cast<GenericAnchorResource *>(p_instance);
			*static_cast<String *>(r_out) = cls->_to_string();
			*r_is_valid = 1U;
			return;
		}
		Resource::to_string_bind(p_instance, r_is_valid, r_out);
	}
}

void GenericAnchorResource::free(void * /*data*/, GDExtensionClassInstancePtr ptr) {
	if (ptr != nullptr) {
		auto *cls = static_cast<GenericAnchorResource *>(ptr);
		cls->~GenericAnchorResource();
		Memory::free_static(cls);
	}
}

void *GenericAnchorResource::_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/) {
	return nullptr;
}

void GenericAnchorResource::_gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/) {}

GDExtensionBool GenericAnchorResource::_gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/) {
	return 1U;
}

void GenericAnchorResource::_notificationv(int32_t p_what, bool p_reversed) {
	GenericAnchorResource::notification_bind(this, p_what, static_cast<GDExtensionBool>(p_reversed));
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
			internal::gdextension_interface_classdb_register_extension_class_integer_constant(internal::library, p_class_name._native_ptr(), p_enum_name._native_ptr(), p_constant_name._native_ptr(), i, 0U);
		}
	}
}

void GenericAnchorResource::bind_anchor_resource(const Dictionary &resource) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!resource.has("name"), "Resource struct does not contain a name.");
	ERR_FAIL_COND_CUSTOM(resource.has("vec"));
	ERR_FAIL_COND_CUSTOM(resource.has("array"));

	String class_name = resource["name"];
	//const String class_name = "GenericAnchorResource";

	const Dictionary struct_info = resource["type"];

	ERR_FAIL_COND_EDMSG_CUSTOM(!struct_info.has("fields"), "Resource struct does not contain any fields");

	const Array fields = struct_info["fields"];

	// TODO(Virax): Fix this temporary fix for dodging duplicate classes.

	if (class_name == "ConfigLine" || class_name == "CandyMachineData") {
		class_name += "2";
	}

	if (property_database.find(class_name) != property_database.end()) {
		uint32_t suffix = 2;
		const String original_class_name = class_name;
		class_name = class_name + String::num_int64(suffix);
		while (property_database.find(class_name) != property_database.end()) {
			for (unsigned int i = 0; i < fields.size(); i++) {
				class_name = original_class_name + String::num_int64(suffix);
			}
			++suffix;
		}
	}

	set_class_name(class_name);

	names.push_back(memnew_custom(StringName(class_name)));
	bind_resource_class(*names[names.size() - 1], "Resource");
	initialize_class();

	for (unsigned int i = 0; i < fields.size(); i++) {
		add_property(fields[i]);
	}

	MethodBind *method_bind = create_method_bind(&GenericAnchorResource::serialize);
	bind_resource_method(get_class_static(), D_METHOD("serialize"), method_bind);
}

void GenericAnchorResource::bind_virtual_method(const StringName &class_name, const StringName &method_name) {
	/*
		BIND_VIRTUAL_METHOD(T, _process);
				BIND_VIRTUAL_METHOD(T, _physics_process);
				BIND_VIRTUAL_METHOD(T, _enter_tree);
				BIND_VIRTUAL_METHOD(T, _exit_tree);
				BIND_VIRTUAL_METHOD(T, _ready);
				BIND_VIRTUAL_METHOD(T, _get_configuration_warnings);
				BIND_VIRTUAL_METHOD(T, _input);
				BIND_VIRTUAL_METHOD(T, _shortcut_input);
				BIND_VIRTUAL_METHOD(T, _unhandled_input);
				BIND_VIRTUAL_METHOD(T, _unhandled_key_input);*/

	//	auto _call_ = [](GDExtensionObjectPtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr p_ret) -> void {
	//		call_with_ptr_args(reinterpret_cast<GenericAnchorResource *>(p_instance), &Node::_process, p_args, p_ret);
	//	};
}

void GenericAnchorResource::add_enum_properties(const Dictionary &property_data) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!property_data.has("name"), "Enum property does not have name.");
}

void GenericAnchorResource::add_property(const Dictionary &property_data) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!property_data.has("name"), "Property does not have name.");

	const String property_name = property_data["name"];

	const String object_type = AnchorProgram::get_object_name(property_data["type"]);

	if (enum_field_map.find(object_type) != enum_field_map.end()) {
		const Variant::Type property_godot_type = AnchorProgram::is_vec_type(object_type) ? Variant::Type::ARRAY : Variant::Type::INT;
		const String hint_string = get_enum_hint(object_type);
		const PropertyHint hint = AnchorProgram::is_vec_type(object_type) ? PROPERTY_HINT_ARRAY_TYPE : PROPERTY_HINT_ENUM;

		bool optional = false;
		if (property_data["type"].get_type() == Variant::DICTIONARY) {
			optional = static_cast<Dictionary>(property_data["type"]).has("option");
		}

		const ResourcePropertyInfo prop_info = ResourcePropertyInfo{
			.property_info = PropertyInfo(property_godot_type, property_name, hint, hint_string, PROPERTY_USAGE_DEFAULT, object_type),
			.value = 0,
			.optional = optional,
			.enabled = !optional,
			.serialize = true,
			.type_info = "",
		};

		if (optional) {
			const ResourcePropertyInfo enabler_prop_info = ResourcePropertyInfo{
				.property_info = PropertyInfo(Variant::BOOL, "enable_" + property_name),
				.value = false,
				.optional = false,
				.enabled = true,
				.serialize = false,
				.type_info = "",
			};
			property_database[get_class_static()]["enable_" + property_name] = enabler_prop_info;
			property_order[get_class_static()].push_back("enable_" + property_name);

			bind_resource_setter(get_class_static(), D_METHOD("set_enable_" + property_name, "value"), "enable_" + property_name);
			bind_resource_getter(get_class_static(), D_METHOD("get_enable_" + property_name), "enable_" + property_name);

			bind_resource_property(get_class_static(), PropertyInfo(Variant::BOOL, "enable_" + property_name));
		}

		property_database[get_class_static()][property_name] = prop_info;
		property_order[get_class_static()].push_back(property_name);

		bind_resource_setter(*names[names.size() - 1], D_METHOD("set_" + property_name, "value"), property_name);
		bind_resource_getter(*names[names.size() - 1], D_METHOD("get_" + property_name), property_name);

		bind_resource_property(get_class_static(), PropertyInfo(property_godot_type, property_name, hint, hint_string));

		// Extra props
		const Array field_types = enum_field_map[object_type];
		for (unsigned int i = 0; i < field_types.size(); i++) {
			const Dictionary field_type = field_types[i];
			if (!field_type.has("fields")) {
				continue;
			}

			const String value_field_name = field_type["name"];
			const Array value_field_types = field_type["fields"];

			for (unsigned int j = 0; j < value_field_types.size(); j++) {
				const Dictionary value_field_type = value_field_types[j];
				const String enum_value_name = value_field_name + String("_value_") + String::num_uint64(j);
				if (property_database[get_class_static()].find(enum_value_name) != property_database[get_class_static()].end()) {
					continue;
				}

				const Variant::Type enum_value_godot_type = AnchorProgram::get_godot_type(value_field_type);
				const String enum_value_hint_string = AnchorProgram::get_godot_class_hint(value_field_type);
				const PropertyHint enum_value_hint = AnchorProgram::get_godot_hint(value_field_type);
				const String extra_type_info = AnchorProgram::get_int_type_info(value_field_type);

				const ResourcePropertyInfo enum_value_info = ResourcePropertyInfo{
					.property_info = PropertyInfo(enum_value_godot_type, enum_value_name, enum_value_hint, enum_value_hint_string),
					.value = godot_type_defval(enum_value_godot_type),
					.optional = false,
					.enabled = i == 0,
					.serialize = true,
					.type_info = extra_type_info,
				};
				property_database[get_class_static()][enum_value_name] = enum_value_info;
				property_order[get_class_static()].push_back(enum_value_name);
				extra_props[value_field_name].push_back(enum_value_name);

				bind_resource_setter(get_class_static(), D_METHOD("set_" + enum_value_name, "value"), enum_value_name);
				bind_resource_getter(get_class_static(), D_METHOD("get_" + enum_value_name), enum_value_name);

				bind_resource_property(get_class_static(), PropertyInfo(enum_value_godot_type, enum_value_name, enum_value_hint, enum_value_hint_string));
			}
		}
	} else {
		const Variant::Type property_godot_type = AnchorProgram::get_godot_type(property_data["type"]);
		const String hint_string = AnchorProgram::get_godot_class_hint(property_data["type"]);
		const PropertyHint hint = AnchorProgram::get_godot_hint(property_data["type"]);
		const String type_info = AnchorProgram::get_int_type_info(property_data["type"]);

		bool optional = false;
		if (property_data["type"].get_type() == Variant::DICTIONARY) {
			optional = static_cast<Dictionary>(property_data["type"]).has("option");
		}

		const ResourcePropertyInfo prop_info = ResourcePropertyInfo{
			.property_info = PropertyInfo(property_godot_type, property_name, hint, hint_string),
			.value = godot_type_defval(property_godot_type),
			.optional = optional,
			.enabled = !optional,
			.serialize = true,
			.type_info = type_info,
		};

		if (optional) {
			const ResourcePropertyInfo enabler_prop_info = ResourcePropertyInfo{
				.property_info = PropertyInfo(Variant::BOOL, "enable_" + property_name),
				.value = false,
				.optional = false,
				.enabled = true,
				.serialize = false,
				.type_info = "",
			};
			property_database[get_class_static()]["enable_" + property_name] = enabler_prop_info;
			property_order[get_class_static()].push_back("enable_" + property_name);

			bind_resource_setter(get_class_static(), D_METHOD("set_enable_" + property_name, "value"), "enable_" + property_name);
			bind_resource_getter(get_class_static(), D_METHOD("get_enable_" + property_name), "enable_" + property_name);

			bind_resource_property(get_class_static(), PropertyInfo(Variant::BOOL, "enable_" + property_name));
		}

		property_database[get_class_static()][property_name] = prop_info;
		property_order[get_class_static()].push_back(property_name);

		bind_resource_setter(get_class_static(), D_METHOD("set_" + property_name, "value"), property_name);
		bind_resource_getter(get_class_static(), D_METHOD("get_" + property_name), property_name);

		bind_resource_property(get_class_static(), PropertyInfo(property_godot_type, property_name, hint, hint_string));
	}

	//properties.push_back(abc);
}

Array GenericAnchorResource::get_property_values(const StringName &class_name) {
	Array result;
	for (const auto &pair : property_database[class_name]) {
		result.append(pair.second.value);
	}
	return result;
}

PackedByteArray GenericAnchorResource::serialize() {
	PackedByteArray result;

	for (const auto &property_name : property_order[local_name]) {
		const ResourcePropertyInfo property = properties[property_name];
		if (!property.serialize) {
			continue;
		}
		if (property.optional) {
			const ResourcePropertyInfo enabler_info = properties["enable_" + property.property_info.name];
			/*if(enabler_info.value){
				result.append_array({1,0,0,0});
			}
			else{
				result.append_array({0,0,0,0});
			}*/
			result.append_array(AnchorProgram::serialize_variant(enabler_info.value));
		}
		if (property.enabled) {
			if (property.property_info.hint != PROPERTY_HINT_ENUM) {
				if (property.value.get_type() == Variant::INT || property.value.get_type() == Variant::FLOAT) {
					if (property.type_info == "u8") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::u8(property.value)));
					} else if (property.type_info == "u16") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::u16(property.value)));
					} else if (property.type_info == "u32") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::u32(property.value)));
					} else if (property.type_info == "u64") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::u64(property.value)));
					} else if (property.type_info == "s8") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::s8(property.value)));
					} else if (property.type_info == "s16") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::s16(property.value)));
					} else if (property.type_info == "s32") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::s32(property.value)));
					} else if (property.type_info == "s64") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::s64(property.value)));
					} else if (property.type_info == "f32") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::f32(property.value)));
					} else if (property.type_info == "f64") {
						result.append_array(AnchorProgram::serialize_variant(AnchorProgram::f64(property.value)));
					}
				} else {
					result.append_array(AnchorProgram::serialize_variant(property.value));
				}
			} else {
				result.append_array(AnchorProgram::serialize_variant(AnchorProgram::u8(property.value)));
			}
		}
	}

	return result;
}

} //namespace godot