#ifndef GODOT_SOLANA_SDK_GENERIC_ANCHOR_RESOURCE_HPP
#define GODOT_SOLANA_SDK_GENERIC_ANCHOR_RESOURCE_HPP

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "anchor_program.hpp"

namespace godot {

typedef void (*BindMethodFunc)();
using NotificationMethod = void (Wrapped::*)(int);
typedef bool (Wrapped::*SetMethod)(const StringName &p_name, const Variant &p_property);
typedef bool (Wrapped::*GetMethod)(const StringName &p_name, Variant &r_ret) const;
typedef void (Wrapped::*GetPropertyListMethod)(List<PropertyInfo> *p_list) const;
typedef bool (Wrapped::*PropertyCanRevertMethod)(const StringName &p_name) const;
typedef bool (Wrapped::*PropertyGetRevertMethod)(const StringName &p_name, Variant &) const;
typedef void (Wrapped::*ValidatePropertyMethod)(PropertyInfo &p_property) const;
typedef String (Wrapped::*ToStringMethod)() const;

typedef struct {
	PropertyInfo property_info;
	Variant value;
	bool optional;
	bool enabled;
	bool serialize;
	String type_info;
} ResourcePropertyInfo;

class GenericAnchorResource : public Resource {
private:
	const String OPTIONAL_PROPERTY_PREFIX = "enable_";

	static std::vector<StringName *> names;
	static std::vector<Callable> static_class_names;
	static std::string string_name;
	static std::unordered_map<StringName, std::unordered_map<StringName, ResourcePropertyInfo>> property_database;
	static std::unordered_map<StringName, std::vector<StringName>> property_order;
	static std::unordered_map<StringName, Array> enum_field_map;
	static std::unordered_map<StringName, GDExtensionClassCallVirtual> virtual_methods;
	static std::unordered_map<StringName, std::vector<StringName>> extra_props;

	std::unordered_map<StringName, ResourcePropertyInfo> properties;

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	//void _get_property_list(List<PropertyInfo> *p_list) const;
	void _validate_property(PropertyInfo &p_property) const {
		if (properties.find(p_property.name) == properties.end()) {
			return;
		}
		if (properties.at(p_property.name).enabled) {
			p_property.usage = PROPERTY_USAGE_DEFAULT;
		} else {
			p_property.usage = PROPERTY_USAGE_NO_EDITOR;
		}
	}

	static GDExtensionObjectPtr _create_instance_func(void *data);
	static GDExtensionClassInstancePtr _recreate_instance_func(void *data, GDExtensionObjectPtr obj);

	static void bind_resource_class(const StringName &p_class_name, const StringName &parent_name);
	static void bind_resource_method(const StringName &p_class_name, const MethodDefinition &method_prototype, MethodBind *p_method);
	static void bind_resource_getter(const StringName &p_class_name, const MethodDefinition &method_prototype, const StringName &property_name);
	static void bind_resource_setter(const StringName &p_class_name, const MethodDefinition &method_prototype, const StringName &property_name);
	static void bind_resource_property(const StringName &p_class_name, const PropertyInfo &property_info, const StringName &setter_name = "", const StringName &getter_name = "");

	static String get_enum_hint(const StringName &enum_type);
	static Variant godot_type_defval(const Variant::Type type_name);
	static PackedStringArray names_from_array(const Array fields);

	void operator=(const GenericAnchorResource & /*p_rval*/) {}
	friend class ClassDB;
	friend class Wrapped;

	String local_name = "";

protected:
	virtual bool _is_extension_class() const override;
	static const StringName *_get_extension_class_name();

	static BindMethodFunc _get_bind_methods();
	static NotificationMethod _get_notification();
	static SetMethod _get_set();
	static GetMethod _get_get();
	static GetPropertyListMethod _get_get_property_list();
	static PropertyCanRevertMethod _get_property_can_revert();
	static PropertyGetRevertMethod _get_property_get_revert();
	static ValidatePropertyMethod _get_validate_property();
	static ToStringMethod _get_to_string();

	template <typename T, typename B>
	static void register_virtuals() {
		Resource::register_virtuals<T, B>();
	}

public:
	typedef GenericAnchorResource self_type;
	typedef Resource parent_type;

	static void initialize_class();
	static const StringName &get_class_static();
	static const StringName &get_parent_class_static();

	static void notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed);

	static GDExtensionBool set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value);
	static GDExtensionBool get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret);

	static bool has_get_property_list();
	static const GDExtensionPropertyInfo *get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count);
	static void free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/);

	static GDExtensionBool property_can_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name);
	static GDExtensionBool property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret);
	static GDExtensionBool validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property);
	bool is_property_enabled(const StringName& property_name);

	static void to_string_bind(GDExtensionClassInstancePtr p_instance, GDExtensionBool *r_is_valid, GDExtensionStringPtr r_out);
	static void free(void * /*data*/, GDExtensionClassInstancePtr ptr);
	static void *_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/);
	static void _gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/);

	static GDExtensionBool _gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/);

	static constexpr GDExtensionInstanceBindingCallbacks _gde_binding_callbacks = {
		_gde_binding_create_callback,
		_gde_binding_free_callback,
		_gde_binding_reference_callback,
	};

protected:
	virtual void _notificationv(int32_t p_what, bool p_reversed = false) override;

private:
protected:
	static void _bind_methods();

public:
	GenericAnchorResource() = default;
	static void bind_anchor_enum(const Dictionary &enum_data);
	static void bind_anchor_resource(const Dictionary &resource);
	static void bind_mint_methods(const StringName &class_name);
	static void bind_virtual_method(const StringName &class_name, const StringName &method_name);
	static void set_class_name(const String &name) {
		string_name = name.ascii();
	}
	static void add_enum_properties(const Dictionary &property_data);
	static void add_property(const Dictionary &property_data);
	static Array get_property_values(const StringName &class_name);
	PackedByteArray serialize();
	void say_hi() {
		std::cout << "HI" << std::endl;
	}
	void say_hii(const Variant &other) {
		std::cout << "HI" << std::endl;
	}

	// TODO(Virax): Add support for generic getters and setters.
	Variant no_get() {
		WARN_PRINT_ED("Getter is undefined, read property directly instead");
		return nullptr;
	}
	void no_set(const Variant &value) {
		WARN_PRINT_ED("Setter is undefined, assign property directly instead");
	}

	Array get_extra_account_metas();
	PackedByteArray serialize_core_mint_args();

	~GenericAnchorResource() = default;
};
} //namespace godot

#endif