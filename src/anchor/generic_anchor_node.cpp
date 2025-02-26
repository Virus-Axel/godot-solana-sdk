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
#include "pubkey.hpp"
#include "solana_utils.hpp"

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

GDExtensionObjectPtr GenericAnchorNode::_create_instance_func(void *data) {
	const String instance_class = *static_cast<StringName *>(data);
	GenericAnchorNode *new_object = memnew_custom(GenericAnchorNode);
	new_object->anchor_program = memnew_custom(AnchorProgram);
	const Variant custom_pid = AnchorProgram::get_address_from_idl(loaded_idls[instance_class]);
	ERR_FAIL_COND_V_EDMSG_CUSTOM(custom_pid.get_type() != Variant::OBJECT, new_object->_owner, "Could not get PID");
	Object::cast_to<AnchorProgram>(new_object->anchor_program)->set_idl(loaded_idls[instance_class]);
	Object::cast_to<AnchorProgram>(new_object->anchor_program)->set_pid(Object::cast_to<Pubkey>(custom_pid)->to_string());
	Object::cast_to<AnchorProgram>(new_object->anchor_program)->connect("account_data_fetched", callable_mp(new_object, &GenericAnchorNode::emit_account_data));
	return new_object->_owner;
}

GDExtensionClassCallVirtual GenericAnchorNode::get_virtual_func(void *p_userdata, GDExtensionConstStringNamePtr p_name) {
	// This is called by Godot the first time it calls a virtual function, and it caches the result, per object instance.
	// Because of this, it can happen from different threads at once.
	// It should be ok not using any mutex as long as we only READ data.
	const StringName *class_name = reinterpret_cast<const StringName *>(p_userdata);
	const StringName *name = reinterpret_cast<const StringName *>(p_name);

	ERR_FAIL_COND_V_MSG(virtual_methods.find(*name) == virtual_methods.end(), nullptr, String("Virtual method '{0}' doesn't exist.").format(Array::make(*name)));

	return virtual_methods[*name];
}

bool GenericAnchorNode::_is_extension_class() const {
	return true;
}

const StringName *GenericAnchorNode::_get_extension_class_name() {
	const StringName &string_name = get_class_static();
	return &string_name;
}

BindMethodFunc GenericAnchorNode::_get_bind_methods() {
	return &GenericAnchorNode::_bind_methods;
}

NotificationMethod GenericAnchorNode::_get_notification() {
	return static_cast<void (::godot::Wrapped::*)(int)>(&GenericAnchorNode::_notification);
}

SetMethod GenericAnchorNode::_get_set() {
	return static_cast<bool (Wrapped::*)(const StringName &p_name, const Variant &p_property)>(&GenericAnchorNode::_set);
}

GetMethod GenericAnchorNode::_get_get() {
	return static_cast<bool (Wrapped::*)(const StringName &p_name, Variant &r_ret) const>(&GenericAnchorNode::_get);
}

GetPropertyListMethod GenericAnchorNode::_get_get_property_list() {
	return static_cast<void (Wrapped::*)(List<PropertyInfo> *p_list) const>(&GenericAnchorNode::_get_property_list);
}

PropertyCanRevertMethod GenericAnchorNode::_get_property_can_revert() {
	return static_cast<bool (Wrapped::*)(const StringName &p_name) const>(&GenericAnchorNode::_property_can_revert);
}

PropertyGetRevertMethod GenericAnchorNode::_get_property_get_revert(){
	return static_cast<bool (Wrapped::*)(const StringName &p_name, Variant &) const>(&GenericAnchorNode::_property_get_revert);
}

ValidatePropertyMethod GenericAnchorNode::_get_validate_property() {
	return static_cast<void (Wrapped::*)(PropertyInfo &p_property) const>(&GenericAnchorNode::_validate_property);
}

ToStringMethod GenericAnchorNode::_get_to_string() {
	return static_cast<String (Wrapped::*)() const>(&GenericAnchorNode::_to_string);
}

void GenericAnchorNode::_process(double p_delta) {
	Object::cast_to<AnchorProgram>(anchor_program)->_process(p_delta);
}

void GenericAnchorNode::initialize_class() {
	static bool initialized = false;

	if (initialized) {
		return;
	}

	virtual_methods["_process"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _process);
	virtual_methods["_ready"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _ready);
	virtual_methods["_enter_tree"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _enter_tree);
	virtual_methods["_exit_tree"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _exit_tree);
	virtual_methods["_get_configuration_warnings"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _get_configuration_warnings);
	virtual_methods["_input"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _input);
	virtual_methods["_shortcut_input"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _shortcut_input);
	virtual_methods["_unhandled_input"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _unhandled_input);
	virtual_methods["_unhandled_key_input"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _unhandled_key_input);
	virtual_methods["_physics_process"] = GET_VIRTUAL_METHOD(GenericAnchorNode, _physics_process);

	Node::initialize_class();
	if (GenericAnchorNode::_get_bind_methods() != Node::_get_bind_methods()) {
		_bind_methods();
		//Node::register_virtuals<GenericAnchorNode, Node>();
	}
	initialized = true;
}

GDExtensionClassInstancePtr GenericAnchorNode::_recreate_instance_func(void *data, GDExtensionObjectPtr obj) { // NOLINT(bugprone-easily-swappable-parameters)
	#ifdef HOT_RELOAD_ENABLED
	auto *new_instance = static_cast<GenericAnchorNode *>(memalloc(sizeof(GenericAnchorNode)));
	Wrapped::RecreateInstance recreate_data = { new_instance, obj, Wrapped::recreate_instance };
	Wrapped::recreate_instance = &recreate_data;
	memnew_placement(new_instance, GenericAnchorNode);
	return new_instance;
#else
	return nullptr;
#endif
}

StringName GenericAnchorNode::get_fetcher_name(const StringName& account_name){
	return "fetch_" + account_name;
}

const StringName &GenericAnchorNode::get_class_static() {
	static StringName string_name_gd = "GenericAnchorNode";
	string_name_gd = *memnew_custom(String(string_name.c_str()));
	return string_name_gd;
}

const StringName &GenericAnchorNode::get_parent_class_static() {
	return Node::get_class_static();
}

void GenericAnchorNode::notification_bind(GDExtensionClassInstancePtr p_instance, int32_t p_what, GDExtensionBool p_reversed) {
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

GDExtensionBool GenericAnchorNode::set_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionConstVariantPtr p_value) {
	if (p_instance) {
		if (Node::set_bind(p_instance, p_name, p_value)) {
			return true;
		}
		if (GenericAnchorNode::_get_set() != Node::_get_set()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			return cls->_set(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<const Variant *>(p_value));
		}
	}
	return false;
}

GDExtensionBool GenericAnchorNode::get_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance) {
		if (Node::get_bind(p_instance, p_name, r_ret)) {
			return true;
		}
		if (GenericAnchorNode::_get_get() != Node::_get_get()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			return cls->_get(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<Variant *>(r_ret));
		}
	}
	return false;
}

bool GenericAnchorNode::has_get_property_list() {
	return GenericAnchorNode::_get_get_property_list() && GenericAnchorNode::_get_get_property_list() != Node::_get_get_property_list();
}

const GDExtensionPropertyInfo *GenericAnchorNode::get_property_list_bind(GDExtensionClassInstancePtr p_instance, uint32_t *r_count) {
	if (!p_instance) {
		if (r_count)
			*r_count = 0;
		return nullptr;
	}
	GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
	List<PropertyInfo> &plist_cpp = cls->plist_owned;
	ERR_FAIL_COND_V_MSG(!plist_cpp.is_empty(), nullptr, "Internal error, property list was not freed by engine!");
	cls->_get_property_list(&plist_cpp);
	return internal::create_c_property_list(plist_cpp, r_count);
}

void GenericAnchorNode::free_property_list_bind(GDExtensionClassInstancePtr p_instance, const GDExtensionPropertyInfo *p_list, uint32_t /*p_count*/) {
	if (p_instance) {
		GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
		cls->plist_owned.clear();
		internal::free_c_property_list(const_cast<GDExtensionPropertyInfo *>(p_list));
	}
}

GDExtensionBool GenericAnchorNode::property_can_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name) {
	if (p_instance && GenericAnchorNode::_get_property_can_revert()) {
		if (GenericAnchorNode::_get_property_can_revert() != Node::_get_property_can_revert()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			return cls->_property_can_revert(*reinterpret_cast<const StringName *>(p_name));
		}
		return Node::property_can_revert_bind(p_instance, p_name);
	}
	return false;
}

GDExtensionBool GenericAnchorNode::property_get_revert_bind(GDExtensionClassInstancePtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret) {
	if (p_instance && GenericAnchorNode::_get_property_get_revert()) {
		if (GenericAnchorNode::_get_property_get_revert() != Node::_get_property_get_revert()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			return cls->_property_get_revert(*reinterpret_cast<const StringName *>(p_name), *reinterpret_cast<Variant *>(r_ret));
		}
		return Node::property_get_revert_bind(p_instance, p_name, r_ret);
	}
	return false;
}

GDExtensionBool GenericAnchorNode::validate_property_bind(GDExtensionClassInstancePtr p_instance, GDExtensionPropertyInfo *p_property) {
	bool ret = false;
	if (p_instance && GenericAnchorNode::_get_validate_property()) {
		ret = Node::validate_property_bind(p_instance, p_property);
		if (GenericAnchorNode::_get_validate_property() != Node::_get_validate_property()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			PropertyInfo info(p_property);
			cls->_validate_property(info);
			info._update(p_property);
			return true;
		}
	}
	return ret;
}

void GenericAnchorNode::to_string_bind(GDExtensionClassInstancePtr p_instance, GDExtensionBool *r_is_valid, GDExtensionStringPtr r_out) {
	if (p_instance && GenericAnchorNode::_get_to_string()) {
		if (GenericAnchorNode::_get_to_string() != Node::_get_to_string()) {
			GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(p_instance);
			*reinterpret_cast<String *>(r_out) = cls->_to_string();
			*r_is_valid = true;
			return;
		}
		Node::to_string_bind(p_instance, r_is_valid, r_out);
	}
}

void GenericAnchorNode::free(void * /*data*/, GDExtensionClassInstancePtr ptr) {
	if (ptr) {
		GenericAnchorNode *cls = reinterpret_cast<GenericAnchorNode *>(ptr);
		cls->~GenericAnchorNode();
		Memory::free_static(cls);
	}
}

void *GenericAnchorNode::_gde_binding_create_callback(void * /*p_token*/, void * /*p_instance*/) {
	return nullptr;
}

void GenericAnchorNode::_gde_binding_free_callback(void * /*p_token*/, void * /*p_instance*/, void * /*p_binding*/) {}

GDExtensionBool GenericAnchorNode::_gde_binding_reference_callback(void * /*p_token*/, void * /*p_instance*/, GDExtensionBool /*p_reference*/) {
	return true;
}

void GenericAnchorNode::_notificationv(int32_t p_what, bool p_reversed) {
	GenericAnchorNode::notification_bind(this, p_what, p_reversed);
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
	return args.slice(accounts.size());
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
	GDExtensionClassCreationInfo3 class_info = {
		false, // GDExtensionBool is_virtual;
		false, // GDExtensionBool is_abstract;
		true, // GDExtensionBool is_exposed;
		true, // GDExtensionBool is_runtime;
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
		nullptr, // GDExtensionClassGetRID get_rid;
		(void *)names[names.size() - 1], // void *class_userdata;
	};

	StringName name = String(class_name);
	StringName parent_name = "Node";
	internal::gdextension_interface_classdb_register_extension_class3(internal::library, name._native_ptr(), parent_name._native_ptr(), &class_info);

	bind_signal(class_name, MethodInfo("account_fetched", PropertyInfo(Variant::DICTIONARY, "account_info")));

	if (idl.has("instructions")) {
		const Array instructions = idl["instructions"];
		for (unsigned int i = 0; i < instructions.size(); i++) {
			const Dictionary instruction = instructions[i];
			bind_instruction_caller(class_name, instruction);
		}
	}

	if (idl.has("accounts")){
		const Array accounts = idl["accounts"];
		for (unsigned int i = 0; i < accounts.size(); i++) {
			const Dictionary account = accounts[i];
			const StringName account_name = account["name"];
			const StringName account_fetcher_name = get_fetcher_name(account_name);
			account_fetch_method_accounts[account_fetcher_name] = account_name;
			bind_account_fetcher(class_name, account);
		}
	}

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
		ERR_FAIL_COND_EDMSG(static_cast<Object*>(*var)->get_class() != "Pubkey", "Parameter is not Pubkey");

		static_cast<GenericAnchorNode *>(p_instance)->pending_fetch_account =  account_fetch_method_accounts[va->get_name()];
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

void GenericAnchorNode::bind_signal(const StringName &p_class_name, const MethodInfo &p_signal){
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

} //namespace godot