#ifndef GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP
#define GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP

#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "anchor/anchor_program.hpp"
#include "custom_class_management/generic_node.hpp"

namespace godot {

/**
 * @brief Object that exposes protected property arg count.
 */
class MethodBindHack : public MethodBind {
public:
	/**
	 * @setter{arg_count, count}
	 */
	void set_arg_count(unsigned int count) {
		set_argument_count(count);
	}
};

/**
 * @brief A generic node that can take the form of any anchor program.
 */
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

	static GDExtensionObjectPtr _create_instance_func(void *data, GDExtensionBool p_notify_postinitialize);
	static GDExtensionClassInstancePtr _recreate_instance_func(void *data, GDExtensionObjectPtr obj);

	static void bind_pid_getter(const StringName &p_class_name);
	static StringName get_fetcher_name(const StringName &account_name);
	static const StringName *_get_extension_class_name();

	void operator=(const GenericAnchorNode & /*p_rval*/) {}

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

	friend class ClassDB;
	friend class Wrapped;

protected:
	/**
	 * @brief Process Anchor program node.
	 *
	 * @param p_delta Elapsed time since last performed process.
	 */
	void _process(double p_delta);

	/**
	 * @brief Bind methods of GenericAnchorNode Node.
	 */
	static void _bind_methods();

public:
	typedef GenericAnchorNode self_type; ///< This type.
	typedef Node parent_type; ///< Parent type.

	/**
	 * @brief Get the class name of the Node.
	 *
	 * @return const StringName& Reference to class name.
	 */
	static const StringName &get_class_static();

	/**
	 * @brief Set the class name of the Node.
	 *
	 * @param name Class name to set.
	 */
	static void set_class_name(const String &name) {
		string_name = name.ascii();
	}

	/**
	 * @brief Checks if specific Anchor node instruction expects extra accounts.
	 *
	 * @param program Anchor Program name.
	 * @param instruction Instruction name.
	 * @return true If instruction expects extra accounts.
	 * @return false otherwise.
	 */
	static bool has_extra_accounts(const StringName &program, const StringName &instruction);

	Variant anchor_program = nullptr; ///< Specific Anchor program.

	GenericAnchorNode() = default;

	/**
	 * @brief Extract the accounts from a list of args.
	 *
	 * @param args to extract from.
	 * @param instruction_name Instruction with argument specification.
	 * @return Array Extracted accounts.
	 */
	Array split_accounts(const Array &args, const StringName &instruction_name);

	/**
	 * @brief Extract the arguments from a list of args.
	 *
	 * @param args to extract from.
	 * @param instruction_name Instruction with argument specification.
	 * @return Array Extracted accounts.
	 */
	Array split_args(const Array &args, const StringName &instruction_name);

	/**
	 * @brief Bind an anchor node to godot.
	 *
	 * @param idl IDL of anchor node.
	 */
	static void bind_anchor_node(const Dictionary &idl);

	/**
	 * @brief Bind instruction builder method to a class.
	 *
	 * @param p_class_name Class name of class to bind method to.
	 * @param anchor_instruction Instruction specification.
	 */
	static void bind_instruction_caller(const StringName &p_class_name, const Dictionary &anchor_instruction);

	/**
	 * @brief Bind account fetcher method to a class.
	 *
	 * @param p_class_name Class name of class to bind method to.
	 * @param anchor_account Account specification.
	 */
	static void bind_account_fetcher(const StringName &p_class_name, const Dictionary &anchor_account);

	/**
	 * @brief Bind signal to a class.
	 *
	 * @param p_class_name Class name of class to bind signal to.
	 * @param p_signal Signal to bind.
	 */
	static void bind_signal(const StringName &p_class_name, const MethodInfo &p_signal);

	/**
	 * @brief Get the Program ID of the specific anchor program.
	 *
	 * @return Variant Program ID Pubkey.
	 */
	Variant get_pid() const;
	~GenericAnchorNode() = default;
};
} //namespace godot

#endif