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

template <typename BaseType>
class GenericType : public BaseType {
private:
	static std::string current_class_name;

	void operator=(const GenericType &other) {} // NOLINT(cert-oop54-cpp) - Handled by godot

	friend class Wrapped;

protected:
	static std::unordered_map<StringName, GDExtensionClassCallVirtual> virtual_methods;

	void _notificationv(int32_t p_what, bool p_reversed) {
		GenericType::notification_bind(this, p_what, static_cast<GDExtensionBool>(p_reversed));
	}

	bool _is_extension_class() const override {
		return true;
	}
	static const StringName *_get_extension_class_name() {
		const StringName &string_name = get_class_static();
		return &string_name;
	}
	static GDExtensionClassCallVirtual get_virtual_func(void *p_userdata, GDExtensionConstStringNamePtr p_name, uint32_t p_hash) {
		// This is called by Godot the first time it calls a virtual function, and it caches the result, per object instance.
		// Because of this, it can happen from different threads at once.
		// It should be ok not using any mutex as long as we only READ data.
		const StringName *class_name = reinterpret_cast<const StringName *>(p_userdata);
		const StringName *name = reinterpret_cast<const StringName *>(p_name);

		ERR_FAIL_COND_V_MSG(virtual_methods.find(*name) == virtual_methods.end(), nullptr, String("Virtual method '{0}' doesn't exist.").format(Array::make(*name)));

		return virtual_methods[*name];
	}

	static BindMethodFunc _get_bind_methods() {
		return &GenericType::_bind_methods;
	}
	static NotificationMethod _get_notification() {
		return static_cast<void (::godot::Wrapped::*)(int)>(&GenericType::_notification);
	}
	static SetMethod _get_set() {
		return static_cast<bool (Wrapped::*)(const StringName &p_name, const Variant &p_property)>(&GenericType::_set);
	}
	static GetMethod _get_get() {
		return static_cast<bool (Wrapped::*)(const StringName &p_name, Variant &r_ret) const>(&GenericType::_get);
	}
	static GetPropertyListMethod _get_get_property_list() {
		return static_cast<void (Wrapped::*)(List<PropertyInfo> *p_list) const>(&GenericType::_get_property_list);
	}
	static PropertyCanRevertMethod _get_property_can_revert() {
		return static_cast<bool (Wrapped::*)(const StringName &p_name) const>(&GenericType::_property_can_revert);
	}
	static PropertyGetRevertMethod _get_property_get_revert() {
		return static_cast<bool (Wrapped::*)(const StringName &p_name, Variant &) const>(&GenericType::_property_get_revert);
	}
	static ValidatePropertyMethod _get_validate_property() {
		return static_cast<void (Wrapped::*)(PropertyInfo &p_property) const>(&GenericType::_validate_property);
	}
	static ToStringMethod _get_to_string() {
		return static_cast<String (Wrapped::*)() const>(&GenericType::_to_string);
	}

public:
	GenericType() = default;

	using self_type = GenericType;
	using parent_type = BaseType;

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
	static const StringName &get_class_static() {
		static const StringName string_name = StringName("GenericType");
		return string_name;
	}
	static void set_class_name(const String &name) {
		current_class_name = name.ascii();
	}
	static const StringName &get_parent_class_static() {
		return BaseType::get_class_static();
	}

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

	static bool has_get_property_list() {
		return (_get_get_property_list() != nullptr) && (_get_get_property_list() != BaseType::_get_get_property_list());
	}

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
	static void free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/) {
		if (p_instance != nullptr) {
			auto *cls = static_cast<GenericType *>(p_instance);
			cls->plist_owned.clear();
			internal::free_c_property_list(const_cast<GDExtensionPropertyInfo *>(p_list));
		}
	}

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
	static void free(void * /*data*/, GDExtensionClassInstancePtr ptr) {
		if (ptr != nullptr) {
			auto *cls = static_cast<GenericType *>(ptr);
			cls->~GenericType();
			Memory::free_static(cls);
		}
	}
	static void *_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/) {
		return nullptr;
	}
	static void _gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/) {}

	static GDExtensionBool _gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/) {
		return 1U;
	}

	static constexpr GDExtensionInstanceBindingCallbacks _gde_binding_callbacks = {
		_gde_binding_create_callback,
		_gde_binding_free_callback,
		_gde_binding_reference_callback,
	};
};

} //namespace godot

#endif