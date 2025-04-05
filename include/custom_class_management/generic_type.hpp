#ifndef CUSTOM_CLASS_MANAGEMENT_GENERIC_TYPE_HPP
#define CUSTOM_CLASS_MANAGEMENT_GENERIC_TYPE_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

#include "gdextension_interface.h"
#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string_name.hpp"

#include "solana_utils.hpp"

#define GET_VIRTUAL_METHOD(m_class, m_method)                                                                      \
	[](GDExtensionObjectPtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr p_ret) -> void { \
		call_with_ptr_args(static_cast<m_class *>(p_instance), &m_class::m_method, p_args, p_ret);                 \
	}

namespace godot {

using BindMethodFunc = void (*)();
using NotificationMethod = void (Wrapped::*)(int);
using SetMethod = bool (Wrapped::*)(const godot::StringName &, const godot::Variant &);
using GetMethod = bool (Wrapped::*)(const godot::StringName &, godot::Variant &) const;
using GetPropertyListMethod = void (Wrapped::*)(List<PropertyInfo> *) const;
using PropertyCanRevertMethod = bool (Wrapped::*)(const godot::StringName &) const;
using PropertyGetRevertMethod = bool (Wrapped::*)(const StringName &, Variant &) const;
using ValidatePropertyMethod = void (Wrapped::*)(godot::PropertyInfo &) const;
using ToStringMethod = godot::String (Wrapped::*)() const;

/**
 * @brief Extensible type that can be modified with custom behavior.
 * 
 * @tparam BaseType Type that will be inherited from.
 */
template <typename BaseType>
class GenericType : public BaseType {
private:
	static std::string current_class_name;

	void operator=(const GenericType &other) {} // NOLINT(cert-oop54-cpp) - Handled by godot

	friend class Wrapped;

protected:
	static std::unordered_map<StringName, GDExtensionClassCallVirtual> virtual_methods; ///< Virtual function map.

	/**
	 * @brief Call bound notification function.
	 * 
	 * @param p_what Notification type.
	 * @param p_reversed Reversed.
	 */
	void _notificationv(int32_t p_what, bool p_reversed) {
		GenericType::notification_bind(this, p_what, static_cast<GDExtensionBool>(p_reversed));
	}

	/**
	 * @brief Check if class is an extension class.
	 * 
	 * @return true If class is an extension class.
	 * @return false Otherwise.
	 */
	bool _is_extension_class() const override {
		return true;
	}

	/**
	 * @brief Get extension class name.
	 * 
	 * @return const StringName* Pointer to class name.
	 */
	static const StringName *_get_extension_class_name() {
		const StringName &string_name = get_class_static();
		return &string_name;
	}

	/**
	 * @brief Get the virtual methods mapped to class name.
	 * 
	 * @param p_userdata Pointer to class name.
	 * @param p_name Virtual function name.
	 * @param p_hash Not used.
	 * @return GDExtensionClassCallVirtual 
	 */
	static GDExtensionClassCallVirtual get_virtual_func(void *p_userdata, GDExtensionConstStringNamePtr p_name, uint32_t p_hash) {
		// This is called by Godot the first time it calls a virtual function, and it caches the result, per object instance.
		// Because of this, it can happen from different threads at once.
		// It should be ok not using any mutex as long as we only READ data.
		const StringName *class_name = reinterpret_cast<const StringName *>(p_userdata);
		const StringName *name = reinterpret_cast<const StringName *>(p_name);

		ERR_FAIL_COND_V_MSG(virtual_methods.find(*name) == virtual_methods.end(), nullptr, String("Virtual method '{0}' doesn't exist.").format(Array::make(*name)));

		return virtual_methods[*name];
	}

	/**
	 * @brief Get _bind_methods method.
	 * 
	 * @return BindMethodFunc _bind_methods method.
	 */
	static BindMethodFunc _get_bind_methods() {
		return &GenericType::_bind_methods;
	}

	/**
	 * @brief Get _notification method.
	 * 
	 * @return NotificationMethod _notification method.
	 */
	static NotificationMethod _get_notification() {
		return static_cast<void (::godot::Wrapped::*)(int)>(&GenericType::_notification);
	}

	/**
	 * @brief Get _set method.
	 * 
	 * @return SetMethod _set method.
	 */
	static SetMethod _get_set() {
		return static_cast<bool (Wrapped::*)(const StringName &p_name, const Variant &p_property)>(&GenericType::_set);
	}

	/**
	 * @brief Get _get method.
	 * 
	 * @return GetMethod _get method.
	 */
	static GetMethod _get_get() {
		return static_cast<bool (Wrapped::*)(const StringName &p_name, Variant &r_ret) const>(&GenericType::_get);
	}

	/**
	 * @brief Get _get_property_list method.
	 * 
	 * @return GetPropertyListMethod _get_property_list method.
	 */
	static GetPropertyListMethod _get_get_property_list() {
		return static_cast<void (Wrapped::*)(List<PropertyInfo> *p_list) const>(&GenericType::_get_property_list);
	}

	/**
	 * @brief Get _property_can_revert method.
	 * 
	 * @return PropertyCanRevertMethod _property_can_revert method.
	 */
	static PropertyCanRevertMethod _get_property_can_revert() {
		return static_cast<bool (Wrapped::*)(const StringName &p_name) const>(&GenericType::_property_can_revert);
	}

	/**
	 * @brief Get _property_get_revert method.
	 * 
	 * @return PropertyGetRevertMethod _property_get_revert method.
	 */
	static PropertyGetRevertMethod _get_property_get_revert() {
		return static_cast<bool (Wrapped::*)(const StringName &p_name, Variant &) const>(&GenericType::_property_get_revert);
	}

	/**
	 * @brief Get _validate_property method.
	 * 
	 * @return ValidatePropertyMethod _validate_property method.
	 */
	static ValidatePropertyMethod _get_validate_property() {
		return static_cast<void (Wrapped::*)(PropertyInfo &p_property) const>(&GenericType::_validate_property);
	}

	/**
	 * @brief Get _to_string method.
	 * 
	 * @return ToStringMethod _to_string method.
	 */
	static ToStringMethod _get_to_string() {
		return static_cast<String (Wrapped::*)() const>(&GenericType::_to_string);
	}

public:
	GenericType() = default;

	using self_type = GenericType; ///< This type.
	using parent_type = BaseType; ///< Parent type.

	/**
	 * @brief Initializes a class
	 * 
	 * Exposed method for increased control over godot bindings.
	 */
	static void initialize_class() {
		static bool initialized = false;

		if (initialized) {
			return;
		}

		BaseType::initialize_class();
		if (_get_bind_methods() != BaseType::_get_bind_methods()) {
			_get_bind_methods();
		}
		initialized = true;
	}

	/**
	 * @brief Get the class name of the generic type.
	 * 
	 * @return const StringName& Class name.
	 */
	static const StringName &get_class_static() {
		static const StringName string_name = StringName("GenericType");
		return string_name;
	}

	/**
	 * @brief Set the class name of a particular type.
	 * 
	 * @param name class name to set.
	 */
	static void set_class_name(const String &name) {
		current_class_name = name.ascii();
	}

	/**
	 * @brief Get the parent class name.
	 * 
	 * @return const StringName& Parent class name.
	 */
	static const StringName &get_parent_class_static() {
		return BaseType::get_class_static();
	}

	/**
	 * @brief Bind notification function
	 * 
	 * Exposed bind notification function for extra control over class methods.
	 * 
	 * @param p_instance Pointer to instance.
	 * @param p_what Notification type.
	 * @param p_reversed reversed.
	 */
	static void notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed) {
		if ((p_instance != nullptr) && (_get_notification() != nullptr)) {
			if (p_reversed == 0U) {
				BaseType::notification_bind(p_instance, p_what, p_reversed);
			}
			if (_get_notification() != BaseType::_get_notification()) {
				auto *cls = static_cast<GenericType *>(p_instance);
				cls->_notification(p_what);
			}
			if (p_reversed != 0U) {
				BaseType::notification_bind(p_instance, p_what, p_reversed);
			}
		}
	}

	/**
	 * @brief Set the bind method function.
	 * 
	 * @param p_instance Instance pointer.
	 * @param p_name Property name.
	 * @param p_value Property value.
	 * @return GDExtensionBool 0 on success.
	 * @return GDExtensionBool 1 on failure.
	 */
	static GDExtensionBool set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value) {
		if (p_instance != nullptr) {
			if (BaseType::set_bind(p_instance, p_name, p_value) != 0U) {
				return 1U;
			}
			if (_get_set() != BaseType::_get_set()) {
				auto *cls = static_cast<GenericType *>(p_instance);
				return static_cast<GDExtensionBool>(cls->_set(*static_cast<const StringName *>(p_name), *static_cast<const Variant *>(p_value)));
			}
		}
		return 0U;
	}

	/**
	 * @brief Get the bind method function.
	 * 
	 * @param p_instance Instance pointer
	 * @param p_name Name of property.
	 * @param r_ret Return variant pointer. 
	 * @return GDExtensionBool 0 on success.
	 * @return GDExtensionBool 1 on failure.
	 */
	static GDExtensionBool get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
		if (p_instance != nullptr) {
			if (BaseType::get_bind(p_instance, p_name, r_ret) != 0U) {
				return 1U;
			}
			if (_get_get() != BaseType::_get_get()) {
				auto *cls = static_cast<GenericType *>(p_instance);
				return static_cast<GDExtensionBool>(cls->_get(*static_cast<const StringName *>(p_name), *static_cast<Variant *>(r_ret)));
			}
		}
		return 0U;
	}

	/**
	 * @brief Checks if get_property_list function has been overridden.
	 * 
	 * @return true if get_property_list has been overridden.
	 * @return false otherwise.
	 */
	static bool has_get_property_list() {
		return (_get_get_property_list() != nullptr) && (_get_get_property_list() != BaseType::_get_get_property_list());
	}

	/**
	 * @brief Get the property list bind method function.
	 * 
	 * @param p_instance Instance pointer.
	 * @param r_count Return counter.
	 * @return const GDExtensionPropertyInfo* Property list.
	 * @return const GDExtensionPropertyInfo* Null on failure.
	 */
	static const GDExtensionPropertyInfo *get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count) {
		if (p_instance == nullptr) {
			if (r_count != nullptr) {
				*r_count = 0;
			}
			return nullptr;
		}
		auto *cls = static_cast<GenericType *>(p_instance);
		List<PropertyInfo> &plist_cpp = cls->plist_owned;
		ERR_FAIL_COND_V_MSG_CUSTOM(!plist_cpp.is_empty(), nullptr, "Internal error, property list was not freed by engine!");
		cls->_get_property_list(&plist_cpp);
		return internal::create_c_property_list(plist_cpp, r_count);
	}

	/**
	 * @brief Free property list bind.
	 */
	static void free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/) {
		if (p_instance != nullptr) {
			auto *cls = static_cast<GenericType *>(p_instance);
			cls->plist_owned.clear();
			internal::free_c_property_list(const_cast<GDExtensionPropertyInfo *>(p_list));
		}
	}

	/**
	 * @brief Check if property_can_revert_bind was overridden.
	 * 
	 * @param p_instance Instance pointer.
	 * @param p_name Name of property.
	 * @return GDExtensionBool true if property_can_revert_bind is overridden.
	 * @return GDExtensionBool false otherwise.
	 */
	static GDExtensionBool property_can_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name) {
		if ((p_instance != nullptr) && (_get_property_can_revert() != nullptr)) {
			if (_get_property_can_revert() != BaseType::_get_property_can_revert()) {
				auto *cls = static_cast<GenericType *>(p_instance);
				return static_cast<GDExtensionBool>(cls->_property_can_revert(*static_cast<const StringName *>(p_name)));
			}
			return BaseType::property_can_revert_bind(p_instance, p_name);
		}
		return 0U;
	}

	/**
	 * @brief Check if property_get_revert is overridden.
	 * 
	 * @param p_instance Instance pointer.
	 * @param p_name Name of pointer.
	 * @param r_ret Pointer to property.
	 * @return GDExtensionBool true if property_get_revert is overridden.
	 * @return GDExtensionBool false otherwise.
	 */
	static GDExtensionBool property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
		if ((p_instance != nullptr) && (_get_property_get_revert() != nullptr)) {
			if (_get_property_get_revert() != BaseType::_get_property_get_revert()) {
				auto *cls = static_cast<GenericType *>(p_instance);
				return static_cast<GDExtensionBool>(cls->_property_get_revert(*static_cast<const StringName *>(p_name), *static_cast<Variant *>(r_ret)));
			}
			return BaseType::property_get_revert_bind(p_instance, p_name, r_ret);
		}
		return 0U;
	}

	/**
	 * @brief Check if validate_property has been overridden.
	 * 
	 * @param p_instance Instance pointer.
	 * @param p_property Pointer to property.
	 * @return GDExtensionBool true if validate_property is overridden.
	 * @return GDExtensionBool false otherwise.
	 */
	static GDExtensionBool validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property) {
		bool ret = false;
		if ((p_instance != nullptr) && (_get_validate_property() != nullptr)) {
			ret = BaseType::validate_property_bind(p_instance, p_property) != 0U;
			if (_get_validate_property() != BaseType::_get_validate_property()) {
				auto *cls = static_cast<GenericType *>(p_instance);
				PropertyInfo info(p_property);
				cls->_validate_property(info);
				info._update(p_property);
				return 1U;
			}
		}
		return static_cast<GDExtensionBool>(ret);
	}

	/**
	 * @brief Get to_string return.
	 * 
	 * @param p_instance Instance pointer.
	 * @param r_is_valid Indicates that to_string is overridden.
	 * @param r_out to_string output.
	 */
	static void to_string_bind(GDExtensionClassInstancePtr p_instance, GDExtensionBool *r_is_valid, GDExtensionStringPtr r_out) {
		if ((p_instance != nullptr) && (_get_to_string() != nullptr)) {
			if (_get_to_string() != BaseType::_get_to_string()) {
				auto *cls = static_cast<GenericType *>(p_instance);
				*static_cast<String *>(r_out) = cls->_to_string();
				*r_is_valid = 1U;
				return;
			}
			BaseType::to_string_bind(p_instance, r_is_valid, r_out);
		}
	}

	/**
	 * @brief Free static data.
	 */
	static void free(void * /*data*/, GDExtensionClassInstancePtr ptr) {
		if (ptr != nullptr) {
			auto *cls = static_cast<GenericType *>(ptr);
			cls->~GenericType();
			Memory::free_static(cls);
		}
	}

	/**
	 * @brief Placeholder method.
	 */
	static void *_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/) {
		return nullptr;
	}
	/**
	 * @brief Placeholder method.
	 */
	static void _gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/) {}

	/**
	 * @brief Placeholder method.
	 * 
	 * @return GDExtensionBool 1.
	 */
	static GDExtensionBool _gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/) {
		return 1U;
	}

	/**
	 * @brief Binding callbacks structure.
	 */
	static constexpr GDExtensionInstanceBindingCallbacks _gde_binding_callbacks = {
		_gde_binding_create_callback,
		_gde_binding_free_callback,
		_gde_binding_reference_callback,
	};
};

} //namespace godot

#endif