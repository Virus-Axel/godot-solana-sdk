#ifndef GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP
#define GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/variant.hpp"

#include <functional>

#include "anchor_program.hpp"

namespace godot {
class GenericAnchorNode : public Node {
private:
	static Array *loaded_idls;
	static std::string class_name;
	static std::vector<std::function<const StringName &()>> class_name_func;

	Variant anchor_program = memnew(AnchorProgram);

	static GDExtensionObjectPtr _create_instance_func(void *data);
    static GDExtensionObjectPtr _create_instance_func2(void *data, int32_t index);
	static GDExtensionClassInstancePtr _recreate_instance_func(void *data, GDExtensionObjectPtr obj);

	void operator=(const GenericAnchorNode & /*p_rval*/) {}
	friend class ::godot::ClassDB;
	friend class ::godot::Wrapped;

protected:
	virtual bool _is_extension_class() const override { return true; }

	static const ::godot::StringName *_get_extension_class_name() {
		const ::godot::StringName &string_name = get_class_static();
		return &string_name;
	}

	static void (*_get_bind_methods())() {
		return &GenericAnchorNode::_bind_methods;
	}

	static void (::godot::Wrapped::*_get_notification())(int) {
		return (void(::godot::Wrapped::*)(int)) & GenericAnchorNode::_notification;
	}

	static bool (::godot::Wrapped::*_get_set())(const ::godot::StringName &p_name, const ::godot::Variant &p_property) {
		return (bool(::godot::Wrapped::*)(const ::godot::StringName &p_name, const ::godot::Variant &p_property)) & GenericAnchorNode::_set;
	}

	static bool (::godot::Wrapped::*_get_get())(const ::godot::StringName &p_name, ::godot::Variant &r_ret) const {
		return (bool(::godot::Wrapped::*)(const ::godot::StringName &p_name, ::godot::Variant &r_ret) const) & GenericAnchorNode::_get;
	}

	static void (::godot::Wrapped::*_get_get_property_list())(::godot::List<::godot::PropertyInfo> *p_list) const {
		return (void(::godot::Wrapped::*)(::godot::List<::godot::PropertyInfo> * p_list) const) & GenericAnchorNode::_get_property_list;
	}

	static bool (::godot::Wrapped::*_get_property_can_revert())(const ::godot::StringName &p_name) const {
		return (bool(::godot::Wrapped::*)(const ::godot::StringName &p_name) const) & GenericAnchorNode::_property_can_revert;
	}

	static bool (::godot::Wrapped::*_get_property_get_revert())(const ::godot::StringName &p_name, ::godot::Variant &) const {
		return (bool(::godot::Wrapped::*)(const ::godot::StringName &p_name, ::godot::Variant &) const) & GenericAnchorNode::_property_get_revert;
	}

	static void (::godot::Wrapped::*_get_validate_property())(::godot::PropertyInfo &p_property) const {
		return (void(::godot::Wrapped::*)(::godot::PropertyInfo & p_property) const) & GenericAnchorNode::_validate_property;
	}

	static ::godot::String (::godot::Wrapped::*_get_to_string())() const {
		return (::godot::String(::godot::Wrapped::*)() const)&GenericAnchorNode::_to_string;
	}

	template <typename T, typename B>
	static void register_virtuals() {
		Node::register_virtuals<T, B>();
	}

public:
	typedef GenericAnchorNode self_type;
	typedef Node parent_type;

	static void initialize_class() {
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

	static const ::godot::StringName &get_class_static();
	static const ::godot::StringName &get_class_static2(const String &override_name);

	static const ::godot::StringName &get_parent_class_static() {
		return Node::get_class_static();
	}

	static void notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed) {
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

	static GDExtensionBool set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value) {
		if (p_instance) {
			if (Node::set_bind(p_instance, p_name, p_value)) {
				return true;
			}
			if (GenericAnchorNode::_get_set() != Node::_get_set()) {
				GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
				return cls->_set(*reinterpret_cast<const ::godot::StringName *>(p_name), *reinterpret_cast<const ::godot::Variant *>(p_value));
			}
		}
		return false;
	}

	static GDExtensionBool get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
		if (p_instance) {
			if (Node::get_bind(p_instance, p_name, r_ret)) {
				return true;
			}
			if (GenericAnchorNode::_get_get() != Node::_get_get()) {
				GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
				return cls->_get(*reinterpret_cast<const ::godot::StringName *>(p_name), *reinterpret_cast<::godot::Variant *>(r_ret));
			}
		}
		return false;
	}

	static inline bool has_get_property_list() {
		return GenericAnchorNode::_get_get_property_list() && GenericAnchorNode::_get_get_property_list() != Node::_get_get_property_list();
	}

	static const GDExtensionPropertyInfo *get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count) {
		if (!p_instance) {
			if (r_count)
				*r_count = 0;
			return nullptr;
		}
		GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
		::godot::List<::godot::PropertyInfo> &plist_cpp = cls->plist_owned;
		ERR_FAIL_COND_V_MSG(!plist_cpp.is_empty(), nullptr, "Internal error, property list was not freed by engine!");
		cls->_get_property_list(&plist_cpp);
		return ::godot::internal::create_c_property_list(plist_cpp, r_count);
	}

	static void free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/) {
		if (p_instance) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			cls->plist_owned.clear();
			::godot::internal::free_c_property_list(const_cast<GDExtensionPropertyInfo *>(p_list));
		}
	}

	static GDExtensionBool property_can_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name) {
		if (p_instance && GenericAnchorNode::_get_property_can_revert()) {
			if (GenericAnchorNode::_get_property_can_revert() != Node::_get_property_can_revert()) {
				GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
				return cls->_property_can_revert(*reinterpret_cast<const ::godot::StringName *>(p_name));
			}
			return Node::property_can_revert_bind(p_instance, p_name);
		}
		return false;
	}

	static GDExtensionBool property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
		if (p_instance && GenericAnchorNode::_get_property_get_revert()) {
			if (GenericAnchorNode::_get_property_get_revert() != Node::_get_property_get_revert()) {
				GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
				return cls->_property_get_revert(*reinterpret_cast<const ::godot::StringName *>(p_name), *reinterpret_cast<::godot::Variant *>(r_ret));
			}
			return Node::property_get_revert_bind(p_instance, p_name, r_ret);
		}
		return false;
	}

	static GDExtensionBool validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property) {
		bool ret = false;
		if (p_instance && GenericAnchorNode::_get_validate_property()) {
			ret = Node::validate_property_bind(p_instance, p_property);
			if (GenericAnchorNode::_get_validate_property() != Node::_get_validate_property()) {
				GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
				::godot::PropertyInfo info(p_property);
				cls->_validate_property(info);
				info._update(p_property);
				return true;
			}
		}
		return ret;
	}

	static void to_string_bind(GDExtensionClassInstancePtr p_instance, GDExtensionBool *r_is_valid, GDExtensionStringPtr r_out) {
		if (p_instance && GenericAnchorNode::_get_to_string()) {
			if (GenericAnchorNode::_get_to_string() != Node::_get_to_string()) {
				GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
				*reinterpret_cast<::godot::String *>(r_out) = cls->_to_string();
				*r_is_valid = true;
				return;
			}
			Node::to_string_bind(p_instance, r_is_valid, r_out);
		}
	}

	static void free(void * /*data*/, GDExtensionClassInstancePtr ptr) {
		if (ptr) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(ptr);
			cls->~GenericAnchorNode();
			::godot::Memory::free_static(cls);
		}
	}

	static void *_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/) {
		return nullptr;
	}

	static void _gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/) {
	}

	static GDExtensionBool _gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/) {
		return true;
	}

	static constexpr GDExtensionInstanceBindingCallbacks _gde_binding_callbacks = {
		_gde_binding_create_callback,
		_gde_binding_free_callback,
		_gde_binding_reference_callback,
	};

protected:
	virtual void _notificationv(int32_t p_what, bool p_reversed = false) override {
		GenericAnchorNode::notification_bind(this, p_what, p_reversed);
	}

private:
protected:
	static void _bind_methods() {
	}

public:
	void _ready() override;
	GenericAnchorNode() = default;
	static void bind_anchor_node(const Dictionary &idl);
	~GenericAnchorNode() = default;
};
} //namespace godot

#endif