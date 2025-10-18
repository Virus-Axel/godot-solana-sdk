#include "anchor/generic_anchor_node.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "gdextension_interface.h"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"

#include "anchor/anchor_program.hpp"
#include "anchor/generic_anchor_resource.hpp"
#include "candy_machine_core/candy_guard_core.hpp" // NOLINT(misc-include-cleaner)
#include "custom_class_management/generic_type.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

template class GenericType<Node>;

// NOLINTBEGIN(modernize-use-using)
typedef void (*BindMethodFunc)();
using NotificationMethod = void (Wrapped::*)(int);
typedef bool (Wrapped::*SetMethod)(const StringName &p_name, const Variant &p_property);
typedef bool (Wrapped::*GetMethod)(const StringName &p_name, Variant &r_ret) const;
typedef void (Wrapped::*GetPropertyListMethod)(List<PropertyInfo> *p_list) const;
typedef bool (Wrapped::*PropertyCanRevertMethod)(const StringName &p_name) const;
typedef bool (Wrapped::*PropertyGetRevertMethod)(const StringName &p_name, Variant &) const;
typedef void (Wrapped::*ValidatePropertyMethod)(PropertyInfo &p_property) const;
typedef String (Wrapped::*ToStringMethod)() const;
// NOLINTEND(modernize-use-using)

// TODO(Virax): Delete this memory as well.
std::unordered_map<StringName, Dictionary> GenericAnchorNode::loaded_idls;
std::vector<StringName *> GenericAnchorNode::names;
std::string GenericAnchorNode::string_name;
std::unordered_map<StringName, GDExtensionClassCallVirtual> GenericAnchorNode::virtual_methods;
std::unordered_map<StringName, StringName> GenericAnchorNode::account_fetch_method_accounts;

void GenericAnchorNode::bind_resources(const Array &resources, const String &class_name) {
	(void)class_name;
	for (uint32_t i = 0; i < resources.size(); i++) {
	}
}

template <typename NodeType>
GDExtensionObjectPtr GenericAnchorNode::_create_instance_func(void *data, GDExtensionBool p_notify_postinitialize) { // NOLINT(readability-convert-member-functions-to-static)
	(void)p_notify_postinitialize;
	const String instance_class = *static_cast<StringName *>(data);
	NodeType *new_object = memnew_custom(GenericAnchorNode);
	new_object->anchor_program = memnew_custom(AnchorProgram);
	new_object->local_name = instance_class;
	const Variant custom_pid = AnchorProgram::get_address_from_idl(loaded_idls[instance_class]);
	ERR_FAIL_COND_V_EDMSG_CUSTOM(custom_pid.get_type() != Variant::OBJECT, new_object->_owner, "Could not get PID");
	Object::cast_to<AnchorProgram>(new_object->anchor_program)->set_idl(loaded_idls[instance_class]);
	Object::cast_to<AnchorProgram>(new_object->anchor_program)->set_pid(Object::cast_to<Pubkey>(custom_pid)->to_string());
	Object::cast_to<AnchorProgram>(new_object->anchor_program)->connect("account_data_fetched", callable_mp(new_object, &GenericAnchorNode::emit_account_data));
	return new_object->_owner;
}

const StringName *GenericAnchorNode::_get_extension_class_name() {
	const StringName &string_name = get_class_static();
	return &string_name;
}

Variant GenericAnchorNode::generic_instruction_bind() { // NOLINT(readability-convert-member-functions-to-static)
	return {};
}

Error GenericAnchorNode::generic_fetch_account_bind(const Variant &account_key) { // NOLINT(readability-convert-member-functions-to-static)
	(void)account_key;
	return Error::OK;
}

void GenericAnchorNode::emit_account_data(const PackedByteArray &account_data) {
	const Dictionary fetch_account = Object::cast_to<AnchorProgram>(anchor_program)->find_idl_account(pending_fetch_account);
	uint32_t consumed_bytes = 0;
	emit_signal("account_fetched", Object::cast_to<AnchorProgram>(anchor_program)->deserialize_dict(account_data, fetch_account["type"], consumed_bytes));
}

void GenericAnchorNode::bind_method(MethodBind &method_bind, GDExtensionClassMethodCall call_function, GDExtensionClassMethodPtrCall ptr_call_function, const StringName &p_class_name) {
	const std::vector<PropertyInfo> return_value_and_arguments_info = method_bind.get_arguments_info_list();
	std::vector<GDExtensionPropertyInfo> return_value_and_arguments_gdextension_info;
	return_value_and_arguments_gdextension_info.reserve(return_value_and_arguments_info.size());
	for (const auto &argument_info : return_value_and_arguments_info) {
		return_value_and_arguments_gdextension_info.push_back(
				GDExtensionPropertyInfo{
						static_cast<GDExtensionVariantType>(argument_info.type),
						argument_info.name._native_ptr(),
						argument_info.class_name._native_ptr(),
						argument_info.hint,
						argument_info.hint_string._native_ptr(),
						argument_info.usage,
				});
	}

	GDExtensionPropertyInfo *return_value_info = return_value_and_arguments_gdextension_info.data();

	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata = method_bind.get_arguments_metadata_list();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata = return_value_and_arguments_metadata.data();
	GDExtensionPropertyInfo *arguments_info = return_value_and_arguments_gdextension_info.data() + 1; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	GDExtensionClassMethodArgumentMetadata *arguments_metadata = return_value_and_arguments_metadata.data() + 1; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

	std::vector<GDExtensionVariantPtr> def_args;
	const std::vector<Variant> &def_args_val = method_bind.get_default_arguments();
	def_args.resize(def_args_val.size());
	for (size_t i = 0; i < def_args_val.size(); i++) {
		def_args[i] = def_args_val[i];
	}

	const StringName name = method_bind.get_name();
	const GDExtensionClassMethodInfo method_info = {
		name._native_ptr(),
		static_cast<void *>(&method_bind),
		call_function,
		ptr_call_function,
		method_bind.get_hint_flags(),
		static_cast<GDExtensionBool>(method_bind.has_return()),
		return_value_info,
		*return_value_metadata,
		static_cast<uint32_t>(method_bind.get_argument_count()),
		arguments_info,
		arguments_metadata,
		static_cast<uint32_t>(method_bind.get_default_argument_count()),
		def_args.data(),
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, p_class_name._native_ptr(), &method_info);
}

bool GenericAnchorNode::has_extra_accounts(const StringName &program, const StringName &instruction) {
	return (program == StringName("candy_guard")) && (instruction == StringName("mintV1"));
}

void GenericAnchorNode::_process(double p_delta) {
	Object::cast_to<AnchorProgram>(anchor_program)->_process(p_delta);
}

GDExtensionClassInstancePtr GenericAnchorNode::_recreate_instance_func(void *data, GDExtensionObjectPtr obj) {
	(void)data;
	(void)obj;
	if constexpr (!std::is_abstract_v<GenericAnchorNode>) {
#ifdef HOT_RELOAD_ENABLED
		//Wrapped::_constructing_recreate_owner = obj;
		auto *new_instance = (GenericAnchorNode *)memalloc(sizeof(GenericAnchorNode)); // NOLINT(google-readability-casting,cppcoreguidelines-pro-type-cstyle-cast)
		memnew_placement(new_instance, GenericAnchorNode); // NOLINT(cppcoreguidelines-owning-memory)
		return new_instance;
#else
		return nullptr;
#endif
	} else {
		return nullptr;
	}
}

StringName GenericAnchorNode::get_fetcher_name(const StringName &account_name) {
	return "fetch_" + account_name;
}

void GenericAnchorNode::set_anchor_program(const Variant &anchor_program) {
	this->anchor_program = anchor_program;
}

Variant GenericAnchorNode::get_anchor_program() const {
	return anchor_program;
}

const StringName &GenericAnchorNode::get_class_static() {
	static StringName string_name_gd = "GenericAnchorNode";
	string_name_gd = String(string_name.c_str());
	return string_name_gd;
}

void GenericAnchorNode::_bind_methods() {
}

Array GenericAnchorNode::split_accounts(const Array &args, const StringName &instruction_name) {
	const AnchorProgram *program = Object::cast_to<AnchorProgram>(anchor_program);
	const Dictionary idl = program->get_idl();
	const String program_name = program->get_idl_name();

	ERR_FAIL_COND_V_CUSTOM(!idl.has("instructions"), Array());
	const Array instructions = idl["instructions"];

	const Dictionary instruction = program->find_idl_instruction(instruction_name);
	ERR_FAIL_COND_V_CUSTOM(!instruction.has("accounts"), Array());
	const Array accounts = instruction["accounts"];

	if (has_extra_accounts(program_name, instruction_name)) {
		const int64_t accounts_size = accounts.size();
		Array result = args.slice(0, accounts_size);
		result.append_array(args[accounts_size]);
		return result;
	}

	return args.slice(0, accounts.size());
}

Array GenericAnchorNode::split_args(const Array &args, const StringName &instruction_name) {
	const AnchorProgram *program = Object::cast_to<AnchorProgram>(anchor_program);
	const Dictionary idl = program->get_idl();
	const String program_name = program->get_idl_name();

	ERR_FAIL_COND_V_CUSTOM(!idl.has("instructions"), Array());
	const Array instructions = idl["instructions"];

	const Dictionary instruction = program->find_idl_instruction(instruction_name);
	ERR_FAIL_COND_V_CUSTOM(!instruction.has("accounts"), Array());
	const Array accounts = instruction["accounts"];
	if (has_extra_accounts(program_name, instruction_name)) {
		return args.slice(accounts.size() + 1);
	}

	return args.slice(accounts.size());
}

template <typename ResourceType>
void GenericAnchorNode::bind_types(const Dictionary &idl) {
	if (idl.has("types")) {
		const Array types = idl["types"];
		// Bind enum constants first.
		for (unsigned int i = 0; i < types.size(); i++) {
			if (AnchorProgram::is_enum(types[i])) {
				GenericAnchorResource::bind_anchor_enum(types[i]);
			}
		}
		for (unsigned int i = 0; i < types.size(); i++) {
			if (!AnchorProgram::is_enum(types[i])) {
				GenericAnchorResource::bind_anchor_resource<ResourceType>(types[i]);
			}
		}
	}
}

template <typename ResourceType>
void GenericAnchorNode::bind_accounts(const Dictionary &idl) {
	if (idl.has("accounts")) {
		const Array types = idl["accounts"];
		for (unsigned int i = 0; i < types.size(); i++) {
			if (!AnchorProgram::is_enum(types[i])) {
				GenericAnchorResource::bind_anchor_resource<ResourceType>(types[i]);
			}
		}
	}
}

void GenericAnchorNode::bind_instructions(const StringName &class_name, const Dictionary &idl) {
	if (idl.has("instructions")) {
		const Array instructions = idl["instructions"];
		for (unsigned int i = 0; i < instructions.size(); i++) {
			const Dictionary instruction = instructions[i];
			bind_instruction_caller(class_name, instruction);
		}
	}
}

void GenericAnchorNode::bind_account_fetchers(const StringName &class_name, const Dictionary &idl) {
	if (idl.has("accounts")) {
		const Array accounts = idl["accounts"];
		for (unsigned int i = 0; i < accounts.size(); i++) {
			const Dictionary account = accounts[i];
			const StringName account_name = account["name"];
			const StringName account_fetcher_name = get_fetcher_name(account_name);
			account_fetch_method_accounts[account_fetcher_name] = account_name;
			bind_account_fetcher(class_name, account);
		}
	}
}

template void GenericAnchorNode::bind_anchor_node<GenericAnchorNode, CandyGuardCore>(const Dictionary &idl);
template void GenericAnchorNode::bind_anchor_node<GenericAnchorNode, GenericAnchorResource>(const Dictionary &idl);
template <typename NodeType, typename ResourceType>
void GenericAnchorNode::bind_anchor_node(const Dictionary &idl) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!idl.has("name"), "IDL does not contain a name.");

	const Variant custom_pid = AnchorProgram::get_address_from_idl(idl);

	ERR_FAIL_COND_EDMSG_CUSTOM(custom_pid.get_type() != Variant::OBJECT, "IDL does not contain a PID.");

	bind_types<ResourceType>(idl);
	bind_accounts<ResourceType>(idl);

	loaded_idls[idl["name"]] = idl;
	const String class_name = idl["name"];

	names.push_back(memnew_custom(StringName(class_name)));
	set_class_name(class_name);

	// Register this class with Godot
	const GDExtensionClassCreationInfo4 class_info = {
		static_cast<GDExtensionBool>(false), // GDExtensionBool is_virtual;
		static_cast<GDExtensionBool>(false), // GDExtensionBool is_abstract;
		static_cast<GDExtensionBool>(true), // GDExtensionBool is_exposed;
		static_cast<GDExtensionBool>(true), // GDExtensionBool is_runtime;
		nullptr,
		set_bind, // GDExtensionClassSet set_func;
		get_bind, // GDExtensionClassGet get_func;
		has_get_property_list() ? get_property_list_bind : nullptr, // GDExtensionClassGetPropertyList get_property_list_func;
		free_property_list_bind, // GDExtensionClassFreePropertyList2 free_property_list_func;
		property_can_revert_bind, // GDExtensionClassPropertyCanRevert property_can_revert_func;
		property_get_revert_bind, // GDExtensionClassPropertyGetRevert property_get_revert_func;
		validate_property_bind, // GDExtensionClassValidateProperty validate_property_func;
		notification_bind, // GDExtensionClassNotification2 notification_func;
		to_string_bind, // GDExtensionClassToString to_string_func;
		nullptr, // GDExtensionClassReference reference_func;
		nullptr, // GDExtensionClassUnreference unreference_func;
		&_create_instance_func<NodeType>, // GDExtensionClassCreateInstance create_instance_func; /* this one is mandatory */
		free, // GDExtensionClassFreeInstance free_instance_func; /* this one is mandatory */
		&_recreate_instance_func, // GDExtensionClassRecreateInstance recreate_instance_func;
		&get_virtual_func, // GDExtensionClassGetVirtual get_virtual_func;
		nullptr, // GDExtensionClassGetVirtualCallData get_virtual_call_data_func;
		nullptr, // GDExtensionClassCallVirtualWithData call_virtual_func;
		static_cast<void *>(names[names.size() - 1]), // void *class_userdata;
	};

	const StringName name = String(class_name);
	const StringName parent_name = "Node";
	internal::gdextension_interface_classdb_register_extension_class4(internal::library, name._native_ptr(), parent_name._native_ptr(), &class_info);

	bind_signal(class_name, MethodInfo("account_fetched", PropertyInfo(Variant::DICTIONARY, "account_info")));

	bind_instructions(class_name, idl);
	bind_account_fetchers(class_name, idl);

	bind_pid_getter(class_name);
	bind_anchor_program_getter(class_name);

	// call bind_methods etc. to register all members of the class
	initialize_class();
}

void GenericAnchorNode::bind_instruction_caller(const StringName &p_class_name, const Dictionary &anchor_instruction) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!anchor_instruction.has("name"), "Anchor instruction does not have name");
	const String instruction_name = anchor_instruction["name"];

	auto *method_bind = static_cast<MethodBindHack *>(create_method_bind(&GenericAnchorNode::generic_instruction_bind)); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

	//std::vector<StringName> args(method_prototype.args.begin(), method_prototype.args.end());
	std::vector<StringName> args;
	const Array instruction_accounts = anchor_instruction["accounts"];
	const Array instruction_args = anchor_instruction["args"];
	for (unsigned int i = 0; i < instruction_accounts.size(); i++) {
		const Dictionary account_data = instruction_accounts[i];
		ERR_FAIL_COND_EDMSG_CUSTOM(!account_data.has("name"), "Account data does not have name");
		args.push_back(account_data["name"]);
	}
	if (has_extra_accounts(p_class_name, instruction_name)) {
		args.emplace_back("extra_accounts");
	}
	for (unsigned int i = 0; i < instruction_args.size(); i++) {
		const Dictionary arg_data = instruction_args[i];
		ERR_FAIL_COND_EDMSG_CUSTOM(!arg_data.has("name"), "Account data does not have name");
		args.push_back(arg_data["name"]);
	}

	method_bind->set_arg_count(static_cast<int>(args.size()));
	method_bind->set_argument_names(args);
	method_bind->set_name(instruction_name);

	auto lambda = [](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstVariantPtr *p_args, GDExtensionInt p_argument_count, GDExtensionVariantPtr r_return, GDExtensionCallError *r_error) {
		(void)r_error;
		if (p_instance == nullptr) {
			return;
		}
		auto *method_data = static_cast<MethodBind *>(p_method_userdata);
		const String stored_instruction_name = method_data->get_name();
		const AnchorProgram *program = Object::cast_to<AnchorProgram>(static_cast<GenericAnchorNode *>(p_instance)->anchor_program);
		Array accounts_and_args;
		for (unsigned int i = 0; i < p_argument_count; i++) {
			const auto *var = static_cast<const Variant *>(p_args[i]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
			accounts_and_args.append(*var);
		}
		const Array accounts = static_cast<GenericAnchorNode *>(p_instance)->split_accounts(accounts_and_args, stored_instruction_name);
		const Array args = static_cast<GenericAnchorNode *>(p_instance)->split_args(accounts_and_args, stored_instruction_name);
		const Dictionary decorated_args = program->build_argument_dictionary(args, stored_instruction_name);
		const Variant ret = program->build_instruction(stored_instruction_name, accounts, decorated_args);
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	};
	(*lambda)(nullptr, nullptr, nullptr, 0, nullptr, nullptr);

	auto lambda2 = [](void * /*p_method_userdata*/, GDExtensionClassInstancePtr /*p_instance*/, const GDExtensionConstTypePtr * /*p_args*/, GDExtensionTypePtr r_return) {
		const Variant ret = {};
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	};

	const StringName name = method_bind->get_name();
	bind_method(*method_bind, lambda, lambda2, p_class_name);
}

void GenericAnchorNode::bind_pid_getter(const StringName &p_class_name) {
	auto *method_bind = static_cast<MethodBindHack *>(create_static_method_bind(&GenericAnchorNode::get_pid)); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

	const std::vector<StringName> args;

	method_bind->set_arg_count(static_cast<int>(args.size()));
	method_bind->set_argument_names(args);
	method_bind->set_name("get_pid");
	method_bind->set_instance_class(p_class_name);

	auto call_function = [](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstVariantPtr *p_args, GDExtensionInt p_argument_count, GDExtensionVariantPtr r_return, GDExtensionCallError *r_error) {
		(void)p_instance;
		(void)p_args;
		(void)p_argument_count;
		(void)r_error;
		auto *method_data = static_cast<MethodBind *>(p_method_userdata);
		const Dictionary program_idl = loaded_idls[method_data->get_instance_class()];
		const Variant ret = AnchorProgram::get_address_from_idl(program_idl);

		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	};

	auto ptr_call_function = [](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_return) {
		(void)p_instance;
		(void)p_args;
		auto *method_data = static_cast<MethodBind *>(p_method_userdata);
		const Dictionary program_idl = loaded_idls[method_data->get_instance_class()];

		const Variant ret = AnchorProgram::get_address_from_idl(program_idl);
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	};

	bind_method(*method_bind, call_function, ptr_call_function, p_class_name);
}

void GenericAnchorNode::bind_anchor_program_getter(const StringName &p_class_name) {
	auto *method_bind = static_cast<MethodBindHack *>(create_method_bind(&GenericAnchorNode::get_anchor_program)); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

	//std::vector<StringName> args(method_prototype.args.begin(), method_prototype.args.end());
	const std::vector<StringName> args;

	method_bind->set_arg_count(static_cast<int>(args.size()));
	method_bind->set_argument_names(args);
	method_bind->set_name("get_anchor_program");

	const StringName name = method_bind->get_name();
	bind_method(*method_bind, MethodBind::bind_call, MethodBind::bind_ptrcall, p_class_name);
}

void GenericAnchorNode::bind_account_fetcher(const StringName &p_class_name, const Dictionary &anchor_account) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!anchor_account.has("name"), "Anchor instruction does not have name");
	const String account_name = anchor_account["name"];

	auto *method_bind = static_cast<MethodBindHack *>(create_method_bind(&GenericAnchorNode::generic_fetch_account_bind)); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

	std::vector<StringName> args;
	args.emplace_back("account_key");

	method_bind->set_arg_count(static_cast<int>(args.size()));
	method_bind->set_argument_names(args);
	method_bind->set_name(get_fetcher_name(account_name));

	auto lambda = [](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstVariantPtr *p_args, GDExtensionInt /*p_argument_count*/, GDExtensionVariantPtr r_return, GDExtensionCallError * /*r_error*/) {
		if (p_instance == nullptr) {
			return;
		}
		auto *method_data = static_cast<MethodBind *>(p_method_userdata);
		const String stored_instruction_name = method_data->get_name();
		auto *program = Object::cast_to<AnchorProgram>(static_cast<GenericAnchorNode *>(p_instance)->anchor_program);

		const auto *var = static_cast<const Variant *>(p_args[0]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

		ERR_FAIL_COND_EDMSG_LAMBDA(var->get_type() != Variant::OBJECT, "Parameter is not object.");
		ERR_FAIL_COND_EDMSG_LAMBDA(static_cast<Object *>(*var)->get_class() != "Pubkey", "Parameter is not Pubkey");

		static_cast<GenericAnchorNode *>(p_instance)->pending_fetch_account = account_fetch_method_accounts[method_data->get_name()];
		const Variant ret = program->fetch_account(account_fetch_method_accounts[method_data->get_name()], *var);
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	};
	(*lambda)(nullptr, nullptr, nullptr, 0, nullptr, nullptr);

	auto lambda2 = [](void * /*p_method_userdata*/, GDExtensionClassInstancePtr /*p_instance*/,
						   const GDExtensionConstTypePtr * /*p_args*/, GDExtensionTypePtr r_return) {
		const Variant ret = {};
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	};

	const StringName name = method_bind->get_name();
	bind_method(*method_bind, lambda, lambda2, p_class_name);
}

void GenericAnchorNode::bind_signal(const StringName &p_class_name, const MethodInfo &p_signal) {
	std::vector<GDExtensionPropertyInfo> parameters;
	parameters.reserve(p_signal.arguments.size());

	for (const PropertyInfo &par : p_signal.arguments) {
		parameters.push_back(GDExtensionPropertyInfo{
				static_cast<GDExtensionVariantType>(par.type), // GDExtensionVariantType type;
				par.name._native_ptr(), // GDExtensionStringNamePtr name;
				par.class_name._native_ptr(), // GDExtensionStringNamePtr class_name;
				par.hint, // NONE //uint32_t hint;
				par.hint_string._native_ptr(), // GDExtensionStringPtr hint_string;
				par.usage, // DEFAULT //uint32_t usage;
		});
	}

	internal::gdextension_interface_classdb_register_extension_class_signal(internal::library, p_class_name._native_ptr(), p_signal.name._native_ptr(), parameters.data(), static_cast<GDExtensionInt>(parameters.size()));
}

Variant GenericAnchorNode::get_pid() {
	/*const Dictionary idl = Object::cast_to<AnchorProgram>(anchor_program)->get_idl();

	ERR_FAIL_COND_V_EDMSG_CUSTOM(!idl.has("metadata"), nullptr, "IDL does not have metadata");
	const Dictionary metadata = idl["metadata"];
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!metadata.has("address"), nullptr, "Metadata does not have address");
	*/
	return Pubkey::new_random();
}

} //namespace godot