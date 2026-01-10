#include "register_types.hpp"

#include "gdextension_interface.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account.hpp"
#include "account_meta.hpp"
#include "address_lookup_table.hpp"
#include "anchor/anchor_program.hpp"
#include "anchor/generated/candy_machine_core.hpp"
#include "anchor/generated/mpl_core.hpp"
#include "anchor/generic_anchor_node.hpp"
#include "anchor/generic_anchor_resource.hpp"
#include "associated_token_account.hpp"
#include "candy_machine/candy_guard.hpp"
#include "candy_machine/candy_guard_access_list.hpp"
#include "candy_machine/candy_machine.hpp"
#include "candy_machine/candy_machine_data.hpp"
#include "candy_machine/config_line.hpp"
#include "candy_machine_core/candy_guard_core.hpp"
#include "compiled_instruction.hpp"
#include "compute_budget.hpp"
#include "dialogs/add_custom_idl.hpp"
#include "dialogs/generic_dialog.hpp"
#include "dialogs/menubar_helper.hpp"
#include "doc_data_godot-solana-sdk.gen.h"
#include "hash.hpp"
#include "honeycomb/enums_generated.hpp"
#include "honeycomb/honeycomb.hpp"
#include "honeycomb/honeycomb_generated.hpp"
#include "honeycomb/honeycomb_types.hpp" // NOLINT(misc-include-cleaner)
#include "instruction.hpp"
#include "keypair.hpp"
#include "meta_data/collection.hpp"
#include "meta_data/create_metadata_args.hpp"
#include "meta_data/creator.hpp"
#include "meta_data/meta_data.hpp"
#include "meta_data/uses.hpp"
#include "mpl_token_metadata.hpp"
#include "pubkey.hpp"
#include "rpc_multi_http_request_client.hpp"
#include "rpc_single_http_request_client.hpp"
#include "rpc_single_ws_request_client.hpp"
#include "shdwdrive.hpp"
#include "solana_client.hpp"
#include "solana_utils.hpp"
#include "spl_token.hpp"
#include "spl_token_2022.hpp"
#include "system_program.hpp"
#include "transaction.hpp"
#include "wallet_adapter.hpp"

namespace godot {

namespace {
void add_setting(const String &name, Variant::Type type, const Variant &default_value, PropertyHint hint = PropertyHint::PROPERTY_HINT_NONE, const String &hint_string = "") {
	if (!ProjectSettings::get_singleton()->has_setting(name)) {
		ProjectSettings::get_singleton()->set(name, default_value);

		ProjectSettings::get_singleton()->set_initial_value(name, default_value);
		ProjectSettings::get_singleton()->set_as_basic(name, true);

		Dictionary property_info;
		property_info["name"] = name;
		property_info["type"] = type;
		property_info["hint"] = hint;
		property_info["hint_string"] = hint_string;

		ProjectSettings::get_singleton()->add_property_info(property_info);
	}
}

void load_user_programs() {
	const Variant idl_locations = ProjectSettings::get_singleton()->get_setting(String(CUSTOM_IDL_LOCATION_SETTING.c_str()));
	ERR_FAIL_COND_EDMSG_CUSTOM(idl_locations.get_type() != Variant::PACKED_STRING_ARRAY, "Could not find setting for idl locations");
	const PackedStringArray idl_filenames = static_cast<PackedStringArray>(idl_locations);
	for (const auto &idl_filename : idl_filenames) {
		AddCustomIdlDialog::load_idl(idl_filename);
	}
}

void load_idl_from_string(const String &json_content) {
	const Dictionary content = JSON::parse_string(json_content);
	GenericAnchorNode::bind_anchor_node<GenericAnchorNode, GenericAnchorResource>(content);
}

void initialize_solana_sdk_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR && (_doc_data_size > 0)) {
		auto editor_help_load_xml_from_utf8_chars_and_len = reinterpret_cast<GDExtensionsInterfaceEditorHelpLoadXmlFromUtf8CharsAndLen>(internal::gdextension_interface_get_proc_address("editor_help_load_xml_from_utf8_chars_and_len")); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
		editor_help_load_xml_from_utf8_chars_and_len(static_cast<const char *>(_doc_data), _doc_data_size);
	}
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<SolanaUtils>();
	ClassDB::register_class<RpcSingleWsRequestClient>();
	ClassDB::register_class<SolanaClient>();
	ClassDB::register_class<Pubkey>();
	ClassDB::register_class<CompiledInstruction>();
	ClassDB::register_class<AddressLookupTable>();
	ClassDB::register_class<Hash>();
	ClassDB::register_class<Account>();
	ClassDB::register_class<AccountFetcher>();
	ClassDB::register_class<AccountSimulator>();
	ClassDB::register_class<AccountMeta>();
	ClassDB::register_class<Instruction>();
	ClassDB::register_class<Transaction>();
	ClassDB::register_class<Keypair>();
	ClassDB::register_class<WalletAdapter>();
	ClassDB::register_class<ComputeBudget>();
	ClassDB::register_class<SystemProgram>();
	ClassDB::register_class<TokenProgram>();
	ClassDB::register_class<TokenProgram2022>();
	ClassDB::register_class<MplTokenMetadata>();
	ClassDB::register_class<MetaDataCreator>();
	ClassDB::register_class<MetaDataCollection>();
	ClassDB::register_class<MetaDataUses>();
	ClassDB::register_class<CreateMetaDataArgs>();
	ClassDB::register_class<MetaData>();
	ClassDB::register_class<AssociatedTokenAccountProgram>();
	ClassDB::register_class<MplCandyMachine>();
	ClassDB::register_class<MplCandyGuard>();
	ClassDB::register_class<CandyGuardAccessList>();
	ClassDB::register_class<CandyMachineData>();
	ClassDB::register_class<ConfigLineSetting>();
	ClassDB::register_class<ConfigLine>();
	ClassDB::register_class<AnchorProgram>();
	ClassDB::register_class<ShdwDrive>();
	ClassDB::register_class<UserInfo>();
	ClassDB::register_class<StorageAccountV2>();
	ClassDB::register_class<RpcSingleHttpRequestClient>();
	ClassDB::register_class<RpcMultiHttpRequestClient>();
	ClassDB::register_class<HoneyComb>();

	REGISTER_HONEYCOMB_TYPES
	REGISTER_HONEYCOMB_ENUM

	ClassDB::register_class<MenuBarHelper>();
	ClassDB::register_class<AddCustomIdlDialog>();
	ClassDB::register_class<GenericDialog>();
	ClassDB::register_class<GenericAnchorResource>();

	add_setting(String(SOLANA_SERVICE_SETTING_LOCATION.c_str()), Variant::Type::BOOL, false);
	add_setting("solana_sdk/client/default_url", Variant::Type::STRING, "https://api.devnet.solana.com");
	add_setting("solana_sdk/client/default_http_port", Variant::Type::INT, HTTPS_PORT);
	add_setting("solana_sdk/client/default_ws_port", Variant::Type::INT, WSS_PORT);
	add_setting("solana_sdk/anchor/custom_anchor_programs", Variant::Type::PACKED_STRING_ARRAY, PackedStringArray());

	CandyGuardCore::register_from_idl();
	load_idl_from_string(String(candy_machine_core_idl.c_str()));
	load_idl_from_string(String(mpl_core_idl.c_str()));

	load_user_programs();

	// Leave memory allocated and free in unregister_singleton.
	Engine::get_singleton()->register_singleton("http_client", memnew_custom(RpcMultiHttpRequestClient)); // NOLINT (cppcoreguidelines-owning-memory)
	Engine::get_singleton()->register_singleton("ws_client", memnew_custom(RpcSingleWsRequestClient)); // NOLINT (cppcoreguidelines-owning-memory)
	Engine::get_singleton()->register_singleton("account_fetcher", memnew_custom(AccountFetcher)); // NOLINT (cppcoreguidelines-owning-memory)
	Engine::get_singleton()->register_singleton("account_simulator", memnew_custom(AccountSimulator)); // NOLINT (cppcoreguidelines-owning-memory)
}

void uninitialize_solana_sdk_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	Engine::get_singleton()->unregister_singleton("http_client");
	Engine::get_singleton()->unregister_singleton("ws_client");
	MenuBarHelper::deinitialize_dialogs();
}
} //namespace

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT solana_sdk_library_init(const GDExtensionInterfaceGetProcAddress interface, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	const godot::GDExtensionBinding::InitObject init_obj(interface, p_library, r_initialization);

	init_obj.register_initializer(initialize_solana_sdk_module);
	init_obj.register_terminator(uninitialize_solana_sdk_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
} //namespace godot