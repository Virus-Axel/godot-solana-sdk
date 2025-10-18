#ifndef GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP
#define GODOT_SOLANA_SDK_GENERIC_ANCHOR_NODE_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "gdextension_interface.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

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
	void set_arg_count(int count) {
		set_argument_count(count);
	}
};

/**
 * @brief A generic node that can take the form of any anchor program.
 */
class GenericAnchorNode : public GenericNode {
private:
	using self_type = GenericAnchorNode; ///< This type.
	using parent_type = Node; ///< Parent type.

	Variant anchor_program = nullptr; ///< Specific Anchor program.

	static std::unordered_map<StringName, Dictionary> loaded_idls;
	static std::vector<StringName *> names;
	static std::string string_name;
	static std::unordered_map<StringName, GDExtensionClassCallVirtual> virtual_methods;
	static std::unordered_map<StringName, StringName> account_fetch_method_accounts;

	String pending_fetch_account;
	String local_name = "";

	static void bind_resources(const Array &resources, const String &class_name);

	template <typename NodeType>
	static GDExtensionObjectPtr _create_instance_func(void *data, GDExtensionBool p_notify_postinitialize);
	static GDExtensionClassInstancePtr _recreate_instance_func(void *data, GDExtensionObjectPtr obj);

	static void bind_pid_getter(const StringName &p_class_name);
	static void bind_anchor_program_getter(const StringName &p_class_name);
	static StringName get_fetcher_name(const StringName &account_name);
	static const StringName *_get_extension_class_name();

	Variant generic_instruction_bind();
	Error generic_fetch_account_bind(const Variant &account_key);
	void emit_account_data(const PackedByteArray &account_data);

	static void bind_method(MethodBind &method_bind, GDExtensionClassMethodCall call_function, GDExtensionClassMethodPtrCall ptr_call_function, const StringName &p_class_name);

	friend class ClassDB;
	friend class Wrapped;

	template <typename ResourceType>
	static void bind_types(const Dictionary &idl);

	template <typename ResourceType>
	static void bind_accounts(const Dictionary &idl);

	static void bind_instructions(const StringName &class_name, const Dictionary &idl);

	static void bind_account_fetchers(const StringName &class_name, const Dictionary &idl);

protected:
	/**
	 * @brief Process Anchor program node.
	 *
	 * @param p_delta Elapsed time since last performed process.
	 */
	void _process(double p_delta) override;

	/**
	 * @brief Bind methods of GenericAnchorNode Node.
	 */
	static void _bind_methods();

public:
	GenericAnchorNode() = default;

	/**
	 * @brief @setter{anchor_program, anchor_program}
	 */
	void set_anchor_program(const Variant &anchor_program);

	/**
	 * @brief @getter{anchor_program, Variant}
	 */
	[[nodiscard]] Variant get_anchor_program() const;

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
	template <typename NodeType, typename ResourceType>
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
	static Variant get_pid();
	~GenericAnchorNode() override = default;
};
} //namespace godot

#endif