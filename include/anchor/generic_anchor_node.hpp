#ifndef GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP
#define GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP

#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "anchor_program.hpp"
#include "custom_class_management/generic_node.hpp"

namespace godot {


class MethodBindHack : public MethodBind {
public:
	void set_arg_count(unsigned int count) {
		set_argument_count(count);
	}
};

class GenericAnchorNode : public GenericNode {
private:
	static std::unordered_map<StringName, Dictionary> loaded_idls;
	static std::vector<StringName *> names;
	static std::string string_name;
	static std::unordered_map<StringName, GDExtensionClassCallVirtual> virtual_methods;
	static std::unordered_map<StringName, StringName> account_fetch_method_accounts;

	String pending_fetch_account;
	String local_name = "";

	static void bind_resources(const Array &resources, const String &class_name);

	static GDExtensionObjectPtr _create_instance_func(void *data);
	static GDExtensionClassInstancePtr _recreate_instance_func(void *data, GDExtensionObjectPtr obj);

	static void bind_pid_getter(const StringName &p_class_name);
	static StringName get_fetcher_name(const StringName &account_name);

	void operator=(const GenericAnchorNode & /*p_rval*/) {}
	friend class ClassDB;
	friend class Wrapped;

protected:
	void _process(double p_delta);

	static const StringName *_get_extension_class_name();

public:
	typedef GenericAnchorNode self_type;
	typedef Node parent_type;

	static const StringName &get_class_static();
	static void set_class_name(const String &name) {
		string_name = name.ascii();
	}

	static bool has_extra_accounts(const StringName& program, const StringName& instruction);

protected:

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
	Variant get_pid() const;
	~GenericAnchorNode() = default;
};
} //namespace godot

#endif