#include "anchor/generic_anchor_node.hpp"

#include <cstdint>

#include "gdextension_interface.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#include "anchor/generic_anchor_resource.hpp"
#include "anchor_program.hpp"
#include "custom_class_management/generic_node.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

template class GenericType<Node>;

typedef void (*BindMethodFunc)();
using NotificationMethod = void (Wrapped::*)(int);
typedef bool (Wrapped::*SetMethod)(const StringName &p_name, const Variant &p_property);
typedef bool (Wrapped::*GetMethod)(const StringName &p_name, Variant &r_ret) const;
typedef void (Wrapped::*GetPropertyListMethod)(List<PropertyInfo> *p_list) const;
typedef bool (Wrapped::*PropertyCanRevertMethod)(const StringName &p_name) const;
typedef bool (Wrapped::*PropertyGetRevertMethod)(const StringName &p_name, Variant &) const;
typedef void (Wrapped::*ValidatePropertyMethod)(PropertyInfo &p_property) const;
typedef String (Wrapped::*ToStringMethod)() const;

// TODO(Virax): Delete this memory as well.
std::unordered_map<StringName, Dictionary> GenericAnchorNode::loaded_idls;
std::vector<StringName *> GenericAnchorNode::names;
std::string GenericAnchorNode::string_name;
std::unordered_map<StringName, GDExtensionClassCallVirtual> GenericAnchorNode::virtual_methods;
std::unordered_map<StringName, StringName> GenericAnchorNode::account_fetch_method_accounts;

void GenericAnchorNode::bind_resources(const Array &resources, const String &class_name) {
	for (uint32_t i = 0; i < resources.size(); i++) {
	}
}

GDExtensionObjectPtr GenericAnchorNode::_create_instance_func(void *data, GDExtensionBool p_notify_postinitialize) {
	const String instance_class = *static_cast<StringName *>(data);
	GenericAnchorNode *new_object = memnew_custom(GenericAnchorNode);
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

bool GenericAnchorNode::has_extra_accounts(const StringName &program, const StringName &instruction) {
	return (program == StringName("candy_guard")) && (instruction == StringName("mintV1"));
}

void GenericAnchorNode::_process(double p_delta) {
	Object::cast_to<AnchorProgram>(anchor_program)->_process(p_delta);
}

GDExtensionClassInstancePtr GenericAnchorNode::_recreate_instance_func(void *data, GDExtensionObjectPtr obj) { // NOLINT(bugprone-easily-swappable-parameters)
	if constexpr (!std::is_abstract_v<GenericAnchorNode>) {
#ifdef HOT_RELOAD_ENABLED
		//Wrapped::_constructing_recreate_owner = obj;
		GenericAnchorNode *new_instance = (GenericAnchorNode *)memalloc(sizeof(GenericAnchorNode));
		memnew_placement(new_instance, GenericAnchorNode);
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

	ERR_FAIL_COND_V(!idl.has("instructions"), Array());
	const Array instructions = idl["instructions"];

	const Dictionary instruction = program->find_idl_instruction(instruction_name);
	ERR_FAIL_COND_V(!instruction.has("accounts"), Array());
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

	ERR_FAIL_COND_V(!idl.has("instructions"), Array());
	const Array instructions = idl["instructions"];

	const Dictionary instruction = program->find_idl_instruction(instruction_name);
	ERR_FAIL_COND_V(!instruction.has("accounts"), Array());
	const Array accounts = instruction["accounts"];
	if (has_extra_accounts(program_name, instruction_name)) {
		return args.slice(accounts.size() + 1);
	} else {
		return args.slice(accounts.size());
	}
}

void GenericAnchorNode::bind_anchor_node(const Dictionary &idl) {
	ERR_FAIL_COND_EDMSG(!idl.has("name"), "IDL does not contain a name.");

	const Variant custom_pid = AnchorProgram::get_address_from_idl(idl);

	ERR_FAIL_COND_EDMSG(custom_pid.get_type() != Variant::OBJECT, "IDL does not contain a PID.");

	//ClassDB::register_class<GenericAnchorResource>();

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
				GenericAnchorResource::bind_anchor_resource(types[i]);
			}
		}
	}

	loaded_idls[idl["name"]] = idl;
	const String class_name = idl["name"];

	names.push_back(memnew(StringName(class_name)));
	set_class_name(class_name);

	// Register this class with Godot
	GDExtensionClassCreationInfo4 class_info = {
		false, // GDExtensionBool is_virtual;
		false, // GDExtensionBool is_abstract;
		true, // GDExtensionBool is_exposed;
		true, // GDExtensionBool is_runtime;
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
		&_create_instance_func, // GDExtensionClassCreateInstance create_instance_func; /* this one is mandatory */
		free, // GDExtensionClassFreeInstance free_instance_func; /* this one is mandatory */
		&_recreate_instance_func, // GDExtensionClassRecreateInstance recreate_instance_func;
		&get_virtual_func, // GDExtensionClassGetVirtual get_virtual_func;
		nullptr, // GDExtensionClassGetVirtualCallData get_virtual_call_data_func;
		nullptr, // GDExtensionClassCallVirtualWithData call_virtual_func;
		(void *)names[names.size() - 1], // void *class_userdata;
	};

	StringName name = String(class_name);
	StringName parent_name = "Node";
	internal::gdextension_interface_classdb_register_extension_class4(internal::library, name._native_ptr(), parent_name._native_ptr(), &class_info);

	bind_signal(class_name, MethodInfo("account_fetched", PropertyInfo(Variant::DICTIONARY, "account_info")));

	if (idl.has("instructions")) {
		const Array instructions = idl["instructions"];
		for (unsigned int i = 0; i < instructions.size(); i++) {
			const Dictionary instruction = instructions[i];
			bind_instruction_caller(class_name, instruction);
		}
	}

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

	bind_pid_getter(class_name);

	// call bind_methods etc. to register all members of the class
	initialize_class();
}

void GenericAnchorNode::bind_instruction_caller(const StringName &p_class_name, const Dictionary &anchor_instruction) {
	ERR_FAIL_COND_EDMSG(!anchor_instruction.has("name"), "Anchor instruction does not have name");
	const String instruction_name = anchor_instruction["name"];

	MethodBindHack *method_bind = (MethodBindHack *)create_method_bind(&GenericAnchorNode::generic_instruction_bind);

	//std::vector<StringName> args(method_prototype.args.begin(), method_prototype.args.end());
	std::vector<StringName> args;
	const Array instruction_accounts = anchor_instruction["accounts"];
	const Array instruction_args = anchor_instruction["args"];
	for (unsigned int i = 0; i < instruction_accounts.size(); i++) {
		const Dictionary account_data = instruction_accounts[i];
		ERR_FAIL_COND_MSG(!account_data.has("name"), "Account data does not have name");
		args.push_back(account_data["name"]);
	}
	if (has_extra_accounts(p_class_name, instruction_name)) {
		args.push_back("extra_accounts");
	}
	for (unsigned int i = 0; i < instruction_args.size(); i++) {
		const Dictionary arg_data = instruction_args[i];
		ERR_FAIL_COND_MSG(!arg_data.has("name"), "Account data does not have name");
		args.push_back(arg_data["name"]);
	}

	method_bind->set_arg_count(args.size());
	method_bind->set_argument_names(args);
	method_bind->set_name(instruction_name);

	const std::vector<PropertyInfo> return_value_and_arguments_info = method_bind->get_arguments_info_list();
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

	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata = method_bind->get_arguments_metadata_list();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata = return_value_and_arguments_metadata.data();
	GDExtensionPropertyInfo *arguments_info = return_value_and_arguments_gdextension_info.data() + 1;
	GDExtensionClassMethodArgumentMetadata *arguments_metadata = return_value_and_arguments_metadata.data() + 1;

	std::vector<GDExtensionVariantPtr> def_args;
	const std::vector<Variant> &def_args_val = method_bind->get_default_arguments();
	def_args.resize(def_args_val.size());
	for (size_t i = 0; i < def_args_val.size(); i++) {
		def_args[i] = (GDExtensionVariantPtr)&def_args_val[i];
	}

	auto *lambda = new auto([](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstVariantPtr *p_args, GDExtensionInt p_argument_count, GDExtensionVariantPtr r_return, GDExtensionCallError *r_error) {
		if (p_instance == nullptr) {
			return;
		}
		MethodBind *va = (MethodBind *)p_method_userdata;
		const String stored_instruction_name = va->get_name();
		const AnchorProgram *program = Object::cast_to<AnchorProgram>(static_cast<GenericAnchorNode *>(p_instance)->anchor_program);
		Array accounts_and_args;
		for (unsigned int i = 0; i < p_argument_count; i++) {
			const Variant *var = reinterpret_cast<const Variant *>(p_args[i]);
			accounts_and_args.append(*var);
		}
		const Array accounts = static_cast<GenericAnchorNode *>(p_instance)->split_accounts(accounts_and_args, stored_instruction_name);
		const Array args = static_cast<GenericAnchorNode *>(p_instance)->split_args(accounts_and_args, stored_instruction_name);
		const Dictionary decorated_args = program->build_argument_dictionary(args, stored_instruction_name);
		Variant ret = program->build_instruction(stored_instruction_name, accounts, decorated_args);
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	});
	(*lambda)(nullptr, nullptr, nullptr, 0, nullptr, nullptr);

	auto *lambda2 = new auto([](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_return) {
		Variant ret = {};
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	});

	const StringName name = method_bind->get_name();
	const GDExtensionClassMethodInfo method_info = {
		name._native_ptr(),
		method_bind,
		*lambda,
		*lambda2,
		method_bind->get_hint_flags(),
		static_cast<GDExtensionBool>(method_bind->has_return()),
		return_value_info,
		*return_value_metadata,
		static_cast<uint32_t>(method_bind->get_argument_count()),
		arguments_info,
		arguments_metadata,
		static_cast<uint32_t>(method_bind->get_default_argument_count()),
		def_args.data(),
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, p_class_name._native_ptr(), &method_info);
}

void GenericAnchorNode::bind_pid_getter(const StringName &p_class_name) {
	MethodBindHack *method_bind = (MethodBindHack *)create_method_bind(&GenericAnchorNode::get_pid);

	//std::vector<StringName> args(method_prototype.args.begin(), method_prototype.args.end());
	std::vector<StringName> args;

	method_bind->set_arg_count(args.size());
	method_bind->set_argument_names(args);
	method_bind->set_name("get_pid");

	const std::vector<PropertyInfo> return_value_and_arguments_info = method_bind->get_arguments_info_list();
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

	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata = method_bind->get_arguments_metadata_list();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata = return_value_and_arguments_metadata.data();
	GDExtensionPropertyInfo *arguments_info = return_value_and_arguments_gdextension_info.data() + 1;
	GDExtensionClassMethodArgumentMetadata *arguments_metadata = return_value_and_arguments_metadata.data() + 1;

	std::vector<GDExtensionVariantPtr> def_args;
	const std::vector<Variant> &def_args_val = method_bind->get_default_arguments();
	def_args.resize(def_args_val.size());
	for (size_t i = 0; i < def_args_val.size(); i++) {
		def_args[i] = (GDExtensionVariantPtr)&def_args_val[i];
	}

	const StringName name = method_bind->get_name();
	const GDExtensionClassMethodInfo method_info = {
		name._native_ptr(),
		method_bind,
		MethodBind::bind_call,
		MethodBind::bind_ptrcall,
		method_bind->get_hint_flags(),
		static_cast<GDExtensionBool>(method_bind->has_return()),
		return_value_info,
		*return_value_metadata,
		static_cast<uint32_t>(method_bind->get_argument_count()),
		arguments_info,
		arguments_metadata,
		static_cast<uint32_t>(method_bind->get_default_argument_count()),
		def_args.data(),
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, p_class_name._native_ptr(), &method_info);
}

void GenericAnchorNode::bind_account_fetcher(const StringName &p_class_name, const Dictionary &anchor_account) {
	ERR_FAIL_COND_EDMSG(!anchor_account.has("name"), "Anchor instruction does not have name");
	const String account_name = anchor_account["name"];

	MethodBindHack *method_bind = (MethodBindHack *)create_method_bind(&GenericAnchorNode::generic_fetch_account_bind);

	//std::vector<StringName> args(method_prototype.args.begin(), method_prototype.args.end());
	std::vector<StringName> args;
	args.push_back("account_key");

	method_bind->set_arg_count(args.size());
	method_bind->set_argument_names(args);
	method_bind->set_name(get_fetcher_name(account_name));

	const std::vector<PropertyInfo> return_value_and_arguments_info = method_bind->get_arguments_info_list();
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

	std::vector<GDExtensionClassMethodArgumentMetadata> return_value_and_arguments_metadata = method_bind->get_arguments_metadata_list();
	GDExtensionClassMethodArgumentMetadata *return_value_metadata = return_value_and_arguments_metadata.data();
	GDExtensionPropertyInfo *arguments_info = return_value_and_arguments_gdextension_info.data() + 1;
	GDExtensionClassMethodArgumentMetadata *arguments_metadata = return_value_and_arguments_metadata.data() + 1;

	std::vector<GDExtensionVariantPtr> def_args;
	const std::vector<Variant> &def_args_val = method_bind->get_default_arguments();
	def_args.resize(def_args_val.size());
	for (size_t i = 0; i < def_args_val.size(); i++) {
		def_args[i] = (GDExtensionVariantPtr)&def_args_val[i];
	}

	auto *lambda = new auto([](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstVariantPtr *p_args, GDExtensionInt p_argument_count, GDExtensionVariantPtr r_return, GDExtensionCallError *r_error) {
		if (p_instance == nullptr) {
			return;
		}
		MethodBind *va = (MethodBind *)p_method_userdata;
		const String stored_instruction_name = va->get_name();
		AnchorProgram *program = Object::cast_to<AnchorProgram>(static_cast<GenericAnchorNode *>(p_instance)->anchor_program);

		const Variant *var = reinterpret_cast<const Variant *>(p_args[0]);
		//ERR_FAIL_COND_EDMSG(var->get_type() == Variant::STRING_NAME, "Parameter is not string name.");
		//const Variant *var2 = reinterpret_cast<const Variant *>(p_args[1]);
		ERR_FAIL_COND_EDMSG(var->get_type() != Variant::OBJECT, "Parameter is not object.");
		ERR_FAIL_COND_EDMSG(static_cast<Object *>(*var)->get_class() != "Pubkey", "Parameter is not Pubkey");

		static_cast<GenericAnchorNode *>(p_instance)->pending_fetch_account = account_fetch_method_accounts[va->get_name()];
		Variant ret = program->fetch_account(account_fetch_method_accounts[va->get_name()], *var);
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	});
	(*lambda)(nullptr, nullptr, nullptr, 0, nullptr, nullptr);

	auto *lambda2 = new auto([](void *p_method_userdata, GDExtensionClassInstancePtr p_instance, const GDExtensionConstTypePtr *p_args, GDExtensionTypePtr r_return) {
		Variant ret = {};
		internal::gdextension_interface_variant_new_copy(r_return, ret._native_ptr());
	});

	const StringName name = method_bind->get_name();
	const GDExtensionClassMethodInfo method_info = {
		name._native_ptr(),
		method_bind,
		*lambda,
		*lambda2,
		method_bind->get_hint_flags(),
		static_cast<GDExtensionBool>(method_bind->has_return()),
		return_value_info,
		*return_value_metadata,
		static_cast<uint32_t>(method_bind->get_argument_count()),
		arguments_info,
		arguments_metadata,
		static_cast<uint32_t>(method_bind->get_default_argument_count()),
		def_args.data(),
	};
	internal::gdextension_interface_classdb_register_extension_class_method(internal::library, p_class_name._native_ptr(), &method_info);
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

	internal::gdextension_interface_classdb_register_extension_class_signal(internal::library, p_class_name._native_ptr(), p_signal.name._native_ptr(), parameters.data(), parameters.size());
}

Variant GenericAnchorNode::get_pid() const {
	const Dictionary idl = Object::cast_to<AnchorProgram>(anchor_program)->get_idl();

	ERR_FAIL_COND_V_EDMSG_CUSTOM(!idl.has("metadata"), nullptr, "IDL does not have metadata");
	const Dictionary metadata = idl["metadata"];
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!metadata.has("address"), nullptr, "Metadata does not have address");

	return Pubkey::new_from_string(metadata["address"]);
}

} //namespace godot