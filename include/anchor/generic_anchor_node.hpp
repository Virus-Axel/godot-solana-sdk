#ifndef GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP
#define GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "anchor_program.hpp"

namespace godot {

class MethodBindHack : public MethodBind{
	public:
	void set_arg_count(unsigned int count){
		set_argument_count(count);
	}
};

class GenericAnchorNode : public Node {
private:
	static std::unordered_map<StringName, Dictionary> loaded_idls;
	static std::vector<String *> names;
	static std::string string_name;

	static void bind_resources(const Array &resources, const String &class_name);

	static GDExtensionObjectPtr _create_instance_func(void *data);
	static GDExtensionClassInstancePtr _recreate_instance_func(void *data, GDExtensionObjectPtr obj);

	void operator=(const GenericAnchorNode & /*p_rval*/) {}
	friend class ClassDB;
	friend class Wrapped;

protected:
	virtual bool _is_extension_class() const override;
	static const StringName *_get_extension_class_name();

	static void (*_get_bind_methods())();

	static void (Wrapped::*_get_notification())(int);
	static bool (Wrapped::*_get_set())(const StringName &p_name, const Variant &p_property);
	static bool (Wrapped::*_get_get())(const StringName &p_name, Variant &r_ret) const;
	static void (Wrapped::*_get_get_property_list())(List<PropertyInfo> *p_list) const;
	static bool (Wrapped::*_get_property_can_revert())(const StringName &p_name) const;
	static bool (Wrapped::*_get_property_get_revert())(const StringName &p_name, Variant &) const;
	static void (Wrapped::*_get_validate_property())(PropertyInfo &p_property) const;
	static String (Wrapped::*_get_to_string())() const;

	template <typename T, typename B>
	static void register_virtuals() {
		Node::register_virtuals<T, B>();
	}

public:
	typedef GenericAnchorNode self_type;
	typedef Node parent_type;

	static void initialize_class();
	static const StringName &get_class_static();
	static void set_class_name(const String& name){
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
	Variant anchor_program = nullptr;//memnew(AnchorProgram);

	void _ready() override;
	GenericAnchorNode() = default;
	Array split_accounts(const Array& args, const StringName& instruction_name);
	Array split_args(const Array& args, const StringName& instruction_name);
	static void bind_anchor_node(const Dictionary &idl);
	static void bind_instruction_caller(const StringName &p_class_name, const Dictionary& anchor_instruction);

    Variant generic_instruction_bind() {
		return Variant();
    }
	~GenericAnchorNode() = default;
};
} //namespace godot

#endif