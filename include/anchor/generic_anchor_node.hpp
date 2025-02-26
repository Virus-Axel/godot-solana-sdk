#ifndef GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP
#define GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP

#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
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

#define GET_VIRTUAL_METHOD(m_class, m_method)                                                                      \
	[](GDExtensionObjectPtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr p_ret) -> void { \
		call_with_ptr_args(reinterpret_cast<m_class *>(p_instance), &m_class::m_method, p_args, p_ret);            \
	}

class MethodBindHack : public MethodBind {
public:
	void set_arg_count(unsigned int count) {
		set_argument_count(count);
	}
};

class GenericAnchorNode : public Node {
private:
	static std::unordered_map<StringName, Dictionary> loaded_idls;
	static std::vector<StringName *> names;
	static std::string string_name;
	static std::unordered_map<StringName, GDExtensionClassCallVirtual> virtual_methods;
	static std::unordered_map<StringName, StringName> account_fetch_method_accounts;

	String pending_fetch_account;

	static void bind_resources(const Array &resources, const String &class_name);

	static GDExtensionObjectPtr _create_instance_func(void *data);
	static GDExtensionClassInstancePtr _recreate_instance_func(void *data, GDExtensionObjectPtr obj);

	static StringName get_fetcher_name(const StringName &account_name);

	void operator=(const GenericAnchorNode & /*p_rval*/) {}
	friend class ClassDB;
	friend class Wrapped;

protected:
	void _process(double p_delta);
	void _physics_process(double p_delta) {}
	void _enter_tree() {}
	void _exit_tree() {}
	void _ready() {}
	PackedStringArray _get_configuration_warnings() const {
		return PackedStringArray();
	}
	void _input(const Ref<InputEvent> &p_event) {}
	void _shortcut_input(const Ref<InputEvent> &p_event) {}
	void _unhandled_input(const Ref<InputEvent> &p_event) {}
	void _unhandled_key_input(const Ref<InputEvent> &p_event) {}

	virtual bool _is_extension_class() const override;
	static const StringName *_get_extension_class_name();

	static GDExtensionClassCallVirtual get_virtual_func(void *p_userdata, GDExtensionConstStringNamePtr p_name);

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
		//Node::register_virtuals<T, B>();
	}

public:
	typedef GenericAnchorNode self_type;
	typedef Node parent_type;

	static void initialize_class();
	static const StringName &get_class_static();
	static void set_class_name(const String &name) {
		string_name = name.ascii();
	}
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
	Variant anchor_program = nullptr; //memnew(AnchorProgram);

	GenericAnchorNode() = default;
	Array split_accounts(const Array &args, const StringName &instruction_name);
	Array split_args(const Array &args, const StringName &instruction_name);
	static void bind_anchor_node(const Dictionary &idl);
	static void bind_instruction_caller(const StringName &p_class_name, const Dictionary &anchor_instruction);
	static void bind_account_fetcher(const StringName &p_class_name, const Dictionary &anchor_account);
	static void bind_signal(const StringName &p_class_name, const MethodInfo &p_signal);
	Variant generic_instruction_bind() {
		return Variant();
	}
	Error generic_fetch_account_bind(const Variant &account_key) {
		return Error::OK;
	}
	void emit_account_data(const PackedByteArray &account_data) {
		const Dictionary fetch_account = Object::cast_to<AnchorProgram>(anchor_program)->find_idl_account(pending_fetch_account);
		int consumed_bytes = 0;
		emit_signal("account_fetched", Object::cast_to<AnchorProgram>(anchor_program)->deserialize_dict(account_data, fetch_account["type"], consumed_bytes));
	}
	~GenericAnchorNode() = default;
};
} //namespace godot

#endif