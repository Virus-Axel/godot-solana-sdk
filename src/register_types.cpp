#include "register_types.hpp"

#include "solana_sdk.hpp"
#include "pubkey.hpp"
#include "account.hpp"
#include "account_meta.hpp"
#include "instruction.hpp"
#include "keypair.hpp"
#include "transaction.hpp"
#include "hash.hpp"
#include "utils.hpp"
#include "wallet_adapter.hpp"
#include "message.hpp"
#include "solana_client.hpp"
#include "compute_budget.hpp"
#include "system_program.hpp"
#include "spl_token.hpp"
#include "meta_data.hpp"
#include "mpl_token_metadata.hpp"
#include "associated_token_account.hpp"
#include "mpl_candy_machine.hpp"
#include "anchor_program.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_solana_sdk_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<SolanaSDK>();
    ClassDB::register_class<SolanaClient>();
    ClassDB::register_class<Pubkey>();
    ClassDB::register_class<CompiledInstruction>();
    ClassDB::register_class<Message>();
    ClassDB::register_class<Hash>();
    ClassDB::register_class<Account>();
    ClassDB::register_class<AccountMeta>();
    ClassDB::register_class<Instruction>();
    ClassDB::register_class<Transaction>();
    ClassDB::register_class<Keypair>();
    ClassDB::register_class<WalletAdapter>();
    ClassDB::register_class<ComputeBudget>();
    ClassDB::register_class<SystemProgram>();
    ClassDB::register_class<TokenProgram>();
    ClassDB::register_class<MplTokenMetadata>();
    ClassDB::register_class<MetaDataCreator>();
    ClassDB::register_class<MetaDataCollection>();
    ClassDB::register_class<MetaDataUses>();
    ClassDB::register_class<MetaData>();
    ClassDB::register_class<AssociatedTokenAccountProgram>();
    ClassDB::register_class<MplCandyMachine>();
    ClassDB::register_class<MplCandyGuard>();
    ClassDB::register_class<AnchorProgram>();

    SolanaClient::set_commitment("confirmed");
    SolanaClient::set_encoding("base64");
}

void uninitialize_solana_sdk_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT solana_sdk_library_init(const GDExtensionInterfaceGetProcAddress interface, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(interface, p_library, r_initialization);

    init_obj.register_initializer(initialize_solana_sdk_module);
    init_obj.register_terminator(uninitialize_solana_sdk_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}