#ifndef GODOT_SOLANA_SDK_GENERIC_ANCHOR_RESOURCE_HPP
#define GODOT_SOLANA_SDK_GENERIC_ANCHOR_RESOURCE_HPP

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "anchor/anchor_program.hpp"
#include "custom_class_management/generic_type.hpp"

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

/**
 * @brief Keeps value and properties of a custom property.
 */
typedef struct {
	PropertyInfo property_info; ///< Property info.
	Variant value; ///< Property value.
	bool optional; ///< Determines if property is optional.
	bool enabled; ///< Determines if property is enabled.
	bool serialize; ///< Determines if property is used in serialization.
	String type_info; ///< Type info string.
} ResourcePropertyInfo;

/**
 * @brief A generic node that can take form of any type from an anchor IDL.
 */
class GenericAnchorResource : public GenericType<Resource> {
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

	static GDExtensionObjectPtr _create_instance_func(void *data, GDExtensionBool p_notify_postinitialize);
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
	/**
	 * @brief Check if class is an extension class.
	 *
	 * @return true If class is an extension class.
	 * @return false Otherwise.
	 */
	virtual bool _is_extension_class() const override;

	/**
	 * @brief Get extension class name.
	 *
	 * @return const StringName* Pointer to class name.
	 */
	static const StringName *_get_extension_class_name();

	/**
	 * @brief Get _bind_methods method.
	 *
	 * @return BindMethodFunc _bind_methods method.
	 */
	static BindMethodFunc _get_bind_methods();

	/**
	 * @brief Get _notification method.
	 *
	 * @return NotificationMethod _notification method.
	 */
	static NotificationMethod _get_notification();

	/**
	 * @brief Get _set method.
	 *
	 * @return SetMethod _set method.
	 */
	static SetMethod _get_set();

	/**
	 * @brief Get _get method.
	 *
	 * @return GetMethod _get method.
	 */
	static GetMethod _get_get();

	/**
	 * @brief Get _get_property_list method.
	 *
	 * @return GetPropertyListMethod _get_property_list method.
	 */
	static GetPropertyListMethod _get_get_property_list();

	/**
	 * @brief Get _property_can_revert method.
	 *
	 * @return PropertyCanRevertMethod _property_can_revert method.
	 */
	static PropertyCanRevertMethod _get_property_can_revert();

	/**
	 * @brief Get _property_get_revert method.
	 *
	 * @return PropertyGetRevertMethod _property_get_revert method.
	 */
	static PropertyGetRevertMethod _get_property_get_revert();

	/**
	 * @brief Get _validate_property method.
	 *
	 * @return ValidatePropertyMethod _validate_property method.
	 */
	static ValidatePropertyMethod _get_validate_property();

	/**
	 * @brief Get _to_string method.
	 *
	 * @return ToStringMethod _to_string method.
	 */
	static ToStringMethod _get_to_string();

	/**
	 * @brief Register virtual methods.
	 *
	 * @tparam T This type.
	 * @tparam B Parent type.
	 */
	template <typename T, typename B>
	static void register_virtuals() {
		Resource::register_virtuals<T, B>();
	}

	/**
	 * @brief Call bound notification function.
	 *
	 * @param p_what Notification type.
	 * @param p_reversed Reversed.
	 */
	virtual void _notificationv(int32_t p_what, bool p_reversed = false);

	/**
	 * @brief Bind methods of GenericAnchorNode Node.
	 */
	static void _bind_methods();

public:
	typedef GenericAnchorResource self_type; ///< This type.
	typedef Resource parent_type; ///< Parent type.

	/**
	 * @brief Initializes a class
	 *
	 * Exposed method for increased control over godot bindings.
	 */
	static void initialize_class();

	/**
	 * @brief Get the class name of the Resource.
	 *
	 * @return const StringName& Reference to class name.
	 */
	static const StringName &get_class_static();

	/**
	 * @brief Get the class name of the Resource.
	 *
	 * @return StringName Class name.
	 */
	static const StringName &get_parent_class_static();

	/**
	 * @brief Bind notification function
	 *
	 * Exposed bind notification function for extra control over class methods.
	 *
	 * @param p_instance Pointer to instance.
	 * @param p_what Notification type.
	 * @param p_reversed reversed.
	 */
	static void notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed);

	/**
	 * @brief Set the bind method function.
	 *
	 * @param p_instance Instance pointer.
	 * @param p_name Property name.
	 * @param p_value Property value.
	 * @return GDExtensionBool 0 on success.
	 * @return GDExtensionBool 1 on failure.
	 */
	static GDExtensionBool set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value);

	/**
	 * @brief Get the bind method function.
	 *
	 * @param p_instance Instance pointer
	 * @param p_name Name of property.
	 * @param r_ret Return variant pointer.
	 * @return GDExtensionBool 0 on success.
	 * @return GDExtensionBool 1 on failure.
	 */
	static GDExtensionBool get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret);

	/**
	 * @brief Checks if get_property_list function has been overridden.
	 *
	 * @return true if get_property_list has been overridden.
	 * @return false otherwise.
	 */
	static bool has_get_property_list();

	/**
	 * @brief Get the property list bind method function.
	 *
	 * @param p_instance Instance pointer.
	 * @param r_count Return counter.
	 * @return const GDExtensionPropertyInfo* Property list.
	 * @return const GDExtensionPropertyInfo* Null on failure.
	 */
	static const GDExtensionPropertyInfo *get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count);

	/**
	 * @brief Free property list bind.
	 */
	static void free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/);

	/**
	 * @brief Check if property_can_revert_bind was overridden.
	 *
	 * @param p_instance Instance pointer.
	 * @param p_name Name of property.
	 * @return GDExtensionBool true if property_can_revert_bind is overridden.
	 * @return GDExtensionBool false otherwise.
	 */
	static GDExtensionBool property_can_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name);

	/**
	 * @brief Check if property_get_revert is overridden.
	 *
	 * @param p_instance Instance pointer.
	 * @param p_name Name of pointer.
	 * @param r_ret Pointer to property.
	 * @return GDExtensionBool true if property_get_revert is overridden.
	 * @return GDExtensionBool false otherwise.
	 */
	static GDExtensionBool property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret);

	/**
	 * @brief Check if validate_property has been overridden.
	 *
	 * @param p_instance Instance pointer.
	 * @param p_property Pointer to property.
	 * @return GDExtensionBool true if validate_property is overridden.
	 * @return GDExtensionBool false otherwise.
	 */
	static GDExtensionBool validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property);

	/**
	 * @brief Check if property is enabled.
	 *
	 * @param property_name Property name.
	 * @return true If property is enabled.
	 * @return false If property is disabled.
	 */
	bool is_property_enabled(const StringName &property_name);

	/**
	 * @brief Get to_string return.
	 *
	 * @param p_instance Instance pointer.
	 * @param r_is_valid Indicates that to_string is overridden.
	 * @param r_out to_string output.
	 */
	static void to_string_bind(GDExtensionClassInstancePtr p_instance, GDExtensionBool *r_is_valid, GDExtensionStringPtr r_out);

	/**
	 * @brief Free static data.
	 */
	static void free(void * /*data*/, GDExtensionClassInstancePtr ptr);

	/**
	 * @brief Placeholder method.
	 */
	static void *_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/);

	/**
	 * @brief Placeholder method.
	 */
	static void _gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/);

	/**
	 * @brief Placeholder method.
	 *
	 * @return GDExtensionBool 1.
	 */
	static GDExtensionBool _gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/);

	/**
	 * @brief Binding callbacks structure.
	 */
	static constexpr GDExtensionInstanceBindingCallbacks _gde_binding_callbacks = {
		_gde_binding_create_callback,
		_gde_binding_free_callback,
		_gde_binding_reference_callback,
	};

	GenericAnchorResource() = default;

	/**
	 * @brief Bind an anchor enum to godot.
	 *
	 * @param enum_data Anchor specification of an enum.
	 */
	static void bind_anchor_enum(const Dictionary &enum_data);

	/**
	 * @brief Bind anchor resource to godot.
	 *
	 * @param resource Anchor resource specification.
	 */
	static void bind_anchor_resource(const Dictionary &resource);

	/**
	 * @brief Bind special mint arg serialization methods.
	 *
	 * @param class_name Name of class to bind to.
	 */
	static void bind_mint_methods(const StringName &class_name);

	/**
	 * @brief Binds virtual methods
	 *
	 * @warning This method is disabled.
	 *
	 * @param class_name Name of class to bind to.
	 * @param method_name Not used.
	 */
	static void bind_virtual_method(const StringName &class_name, const StringName &method_name);

	/**
	 * @brief Set the class name of particular Resource.
	 *
	 * @param name Class name to set.
	 */
	static void set_class_name(const String &name) {
		string_name = name.ascii();
	}

	/**
	 * @brief Placeholder method
	 *
	 * @param property_data Not used.
	 */
	static void add_enum_properties(const Dictionary &property_data);

	/**
	 * @brief Add property to class.
	 *
	 * Binds property and automatically created setters and getters.
	 *
	 * @param property_data Property info.
	 */
	static void add_property(const Dictionary &property_data);

	/**
	 * @brief Get all property values.
	 *
	 * @param class_name Name of class to get values of.
	 * @return Array Property values.
	 */
	static Array get_property_values(const StringName &class_name);

	/**
	 * @brief Serializes properties.
	 *
	 * @return PackedByteArray Serialized bytes.
	 */
	PackedByteArray serialize();

	/**
	 * @brief Template method prototype.
	 */
	void say_hi() {
		std::cout << "HI" << std::endl;
	}

	/**
	 * @brief Template method prototype.
	 */
	void say_hii(const Variant &other) {
		std::cout << "HI" << std::endl;
	}

	// TODO(Virax): Add support for generic getters and setters.

	/**
	 * @brief Template method prototype.
	 *
	 * @return Variant nullptr.
	 */
	Variant no_get() {
		WARN_PRINT_ED("Getter is undefined, read property directly instead");
		return nullptr;
	}

	/**
	 * @brief Template method prototype.
	 *
	 * @param value Not used.
	 */
	void no_set(const Variant &value) {
		WARN_PRINT_ED("Setter is undefined, assign property directly instead");
	}

	/**
	 * @brief Get the extra account metas for certain Candy Guard instructions.
	 *
	 * @return Array List of extra account metas.
	 */
	Array get_extra_account_metas();

	/**
	 * @brief Serialize core mint args for certain Candy Guard instructions.
	 *
	 * @return PackedByteArray Serialized bytes.
	 */
	PackedByteArray serialize_core_mint_args();

	~GenericAnchorResource() = default;
};
} //namespace godot

#endif