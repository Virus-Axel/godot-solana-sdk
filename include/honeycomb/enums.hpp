#ifndef HONEYCOMB_ENUM_CLASSES_HPP
#define HONEYCOMB_ENUM_CLASSES_HPP

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"

namespace godot {

class MintAsKind : public Object {
    GDCLASS(MintAsKind, Object);

public:
    static String get_mplcore() { return "MplCore"; }
    static String get_mplmetadata() { return "MplMetadata"; }
    static String get_mplbubblegum() { return "MplBubblegum"; }
    static String get_tokenextensions() { return "TokenExtensions"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("MintAsKind", D_METHOD("get_mplcore"), &MintAsKind::get_mplcore);
        ClassDB::bind_static_method("MintAsKind", D_METHOD("get_mplmetadata"), &MintAsKind::get_mplmetadata);
        ClassDB::bind_static_method("MintAsKind", D_METHOD("get_mplbubblegum"), &MintAsKind::get_mplbubblegum);
        ClassDB::bind_static_method("MintAsKind", D_METHOD("get_tokenextensions"), &MintAsKind::get_tokenextensions);
    }
};

} // namespace godot

namespace godot {

class CharacterSourceKind : public Object {
    GDCLASS(CharacterSourceKind, Object);

public:
    static String get_wrapped() { return "Wrapped"; }
    static String get_assembled() { return "Assembled"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("CharacterSourceKind", D_METHOD("get_wrapped"), &CharacterSourceKind::get_wrapped);
        ClassDB::bind_static_method("CharacterSourceKind", D_METHOD("get_assembled"), &CharacterSourceKind::get_assembled);
    }
};

} // namespace godot

namespace godot {

class AssetCriteriaKind : public Object {
    GDCLASS(AssetCriteriaKind, Object);

public:
    static String get_prepopulated() { return "Prepopulated"; }
    static String get_collection() { return "Collection"; }
    static String get_creator() { return "Creator"; }
    static String get_merkletree() { return "MerkleTree"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("AssetCriteriaKind", D_METHOD("get_prepopulated"), &AssetCriteriaKind::get_prepopulated);
        ClassDB::bind_static_method("AssetCriteriaKind", D_METHOD("get_collection"), &AssetCriteriaKind::get_collection);
        ClassDB::bind_static_method("AssetCriteriaKind", D_METHOD("get_creator"), &AssetCriteriaKind::get_creator);
        ClassDB::bind_static_method("AssetCriteriaKind", D_METHOD("get_merkletree"), &AssetCriteriaKind::get_merkletree);
    }
};

} // namespace godot

namespace godot {

class SourceKind : public Object {
    GDCLASS(SourceKind, Object);

public:
    static String get_assembled() { return "Assembled"; }
    static String get_mplmetadata() { return "MplMetadata"; }
    static String get_mplbubblegum() { return "MplBubblegum"; }
    static String get_tokenextensions() { return "TokenExtensions"; }
    static String get_mplcoreasset() { return "MplCoreAsset"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("SourceKind", D_METHOD("get_assembled"), &SourceKind::get_assembled);
        ClassDB::bind_static_method("SourceKind", D_METHOD("get_mplmetadata"), &SourceKind::get_mplmetadata);
        ClassDB::bind_static_method("SourceKind", D_METHOD("get_mplbubblegum"), &SourceKind::get_mplbubblegum);
        ClassDB::bind_static_method("SourceKind", D_METHOD("get_tokenextensions"), &SourceKind::get_tokenextensions);
        ClassDB::bind_static_method("SourceKind", D_METHOD("get_mplcoreasset"), &SourceKind::get_mplcoreasset);
    }
};

} // namespace godot

namespace godot {

class CivicGateway : public Object {
    GDCLASS(CivicGateway, Object);

public:
    static String get_livenesspass() { return "LivenessPass"; }
    static String get_uniquenesspass() { return "UniquenessPass"; }
    static String get_idverificationpass() { return "IdVerificationPass"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("CivicGateway", D_METHOD("get_livenesspass"), &CivicGateway::get_livenesspass);
        ClassDB::bind_static_method("CivicGateway", D_METHOD("get_uniquenesspass"), &CivicGateway::get_uniquenesspass);
        ClassDB::bind_static_method("CivicGateway", D_METHOD("get_idverificationpass"), &CivicGateway::get_idverificationpass);
    }
};

} // namespace godot

namespace godot {

class BadgesCondition : public Object {
    GDCLASS(BadgesCondition, Object);

public:
    static String get_public() { return "Public"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("BadgesCondition", D_METHOD("get_public"), &BadgesCondition::get_public);
    }
};

} // namespace godot

namespace godot {

class RemoveSocialsInput : public Object {
    GDCLASS(RemoveSocialsInput, Object);

public:
    static String get_twitter() { return "Twitter"; }
    static String get_discord() { return "Discord"; }
    static String get_steam() { return "Steam"; }
    static String get_all() { return "All"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("RemoveSocialsInput", D_METHOD("get_twitter"), &RemoveSocialsInput::get_twitter);
        ClassDB::bind_static_method("RemoveSocialsInput", D_METHOD("get_discord"), &RemoveSocialsInput::get_discord);
        ClassDB::bind_static_method("RemoveSocialsInput", D_METHOD("get_steam"), &RemoveSocialsInput::get_steam);
        ClassDB::bind_static_method("RemoveSocialsInput", D_METHOD("get_all"), &RemoveSocialsInput::get_all);
    }
};

} // namespace godot

namespace godot {

class HiveControlPermissionInput : public Object {
    GDCLASS(HiveControlPermissionInput, Object);

public:
    static String get_managecriterias() { return "ManageCriterias"; }
    static String get_manageservices() { return "ManageServices"; }
    static String get_manageindexing() { return "ManageIndexing"; }
    static String get_manageprofiles() { return "ManageProfiles"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("HiveControlPermissionInput", D_METHOD("get_managecriterias"), &HiveControlPermissionInput::get_managecriterias);
        ClassDB::bind_static_method("HiveControlPermissionInput", D_METHOD("get_manageservices"), &HiveControlPermissionInput::get_manageservices);
        ClassDB::bind_static_method("HiveControlPermissionInput", D_METHOD("get_manageindexing"), &HiveControlPermissionInput::get_manageindexing);
        ClassDB::bind_static_method("HiveControlPermissionInput", D_METHOD("get_manageprofiles"), &HiveControlPermissionInput::get_manageprofiles);
    }
};

} // namespace godot

namespace godot {

class AssetAssemblerPermissionInput : public Object {
    GDCLASS(AssetAssemblerPermissionInput, Object);

public:
    static String get_manageassembler() { return "ManageAssembler"; }
    static String get_updateblock() { return "UpdateBlock"; }
    static String get_updateblockdefinition() { return "UpdateBlockDefinition"; }
    static String get_updatenft() { return "UpdateNFT"; }
    static String get_initialartgeneration() { return "InitialArtGeneration"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("AssetAssemblerPermissionInput", D_METHOD("get_manageassembler"), &AssetAssemblerPermissionInput::get_manageassembler);
        ClassDB::bind_static_method("AssetAssemblerPermissionInput", D_METHOD("get_updateblock"), &AssetAssemblerPermissionInput::get_updateblock);
        ClassDB::bind_static_method("AssetAssemblerPermissionInput", D_METHOD("get_updateblockdefinition"), &AssetAssemblerPermissionInput::get_updateblockdefinition);
        ClassDB::bind_static_method("AssetAssemblerPermissionInput", D_METHOD("get_updatenft"), &AssetAssemblerPermissionInput::get_updatenft);
        ClassDB::bind_static_method("AssetAssemblerPermissionInput", D_METHOD("get_initialartgeneration"), &AssetAssemblerPermissionInput::get_initialartgeneration);
    }
};

} // namespace godot

namespace godot {

class AssetManagerPermissionInput : public Object {
    GDCLASS(AssetManagerPermissionInput, Object);

public:
    static String get_manageassets() { return "ManageAssets"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("AssetManagerPermissionInput", D_METHOD("get_manageassets"), &AssetManagerPermissionInput::get_manageassets);
    }
};

} // namespace godot

namespace godot {

class ResourceManagerPermissionInput : public Object {
    GDCLASS(ResourceManagerPermissionInput, Object);

public:
    static String get_createresources() { return "CreateResources"; }
    static String get_mintresources() { return "MintResources"; }
    static String get_burnresources() { return "BurnResources"; }
    static String get_createfaucet() { return "CreateFaucet"; }
    static String get_createrecipe() { return "CreateRecipe"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("ResourceManagerPermissionInput", D_METHOD("get_createresources"), &ResourceManagerPermissionInput::get_createresources);
        ClassDB::bind_static_method("ResourceManagerPermissionInput", D_METHOD("get_mintresources"), &ResourceManagerPermissionInput::get_mintresources);
        ClassDB::bind_static_method("ResourceManagerPermissionInput", D_METHOD("get_burnresources"), &ResourceManagerPermissionInput::get_burnresources);
        ClassDB::bind_static_method("ResourceManagerPermissionInput", D_METHOD("get_createfaucet"), &ResourceManagerPermissionInput::get_createfaucet);
        ClassDB::bind_static_method("ResourceManagerPermissionInput", D_METHOD("get_createrecipe"), &ResourceManagerPermissionInput::get_createrecipe);
    }
};

} // namespace godot

namespace godot {

class NectarStakingPermissionInput : public Object {
    GDCLASS(NectarStakingPermissionInput, Object);

public:
    static String get_managestakingpool() { return "ManageStakingPool"; }
    static String get_withdrawstakingpoolrewards() { return "WithdrawStakingPoolRewards"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("NectarStakingPermissionInput", D_METHOD("get_managestakingpool"), &NectarStakingPermissionInput::get_managestakingpool);
        ClassDB::bind_static_method("NectarStakingPermissionInput", D_METHOD("get_withdrawstakingpoolrewards"), &NectarStakingPermissionInput::get_withdrawstakingpoolrewards);
    }
};

} // namespace godot

namespace godot {

class NectarMissionsPermissionInput : public Object {
    GDCLASS(NectarMissionsPermissionInput, Object);

public:
    static String get_managemissionpool() { return "ManageMissionPool"; }
    static String get_withdrawmissionpoolrewards() { return "WithdrawMissionPoolRewards"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("NectarMissionsPermissionInput", D_METHOD("get_managemissionpool"), &NectarMissionsPermissionInput::get_managemissionpool);
        ClassDB::bind_static_method("NectarMissionsPermissionInput", D_METHOD("get_withdrawmissionpoolrewards"), &NectarMissionsPermissionInput::get_withdrawmissionpoolrewards);
    }
};

} // namespace godot

namespace godot {

class BuzzGuildPermissionInput : public Object {
    GDCLASS(BuzzGuildPermissionInput, Object);

public:
    static String get_manageguildkit() { return "ManageGuildKit"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("BuzzGuildPermissionInput", D_METHOD("get_manageguildkit"), &BuzzGuildPermissionInput::get_manageguildkit);
    }
};

} // namespace godot

namespace godot {

class LockTypeEnum : public Object {
    GDCLASS(LockTypeEnum, Object);

public:
    static String get_freeze() { return "Freeze"; }
    static String get_custody() { return "Custody"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("LockTypeEnum", D_METHOD("get_freeze"), &LockTypeEnum::get_freeze);
        ClassDB::bind_static_method("LockTypeEnum", D_METHOD("get_custody"), &LockTypeEnum::get_custody);
    }
};

} // namespace godot

namespace godot {

class RewardKind : public Object {
    GDCLASS(RewardKind, Object);

public:
    static String get_xp() { return "Xp"; }
    static String get_resource() { return "Resource"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("RewardKind", D_METHOD("get_xp"), &RewardKind::get_xp);
        ClassDB::bind_static_method("RewardKind", D_METHOD("get_resource"), &RewardKind::get_resource);
    }
};

} // namespace godot

namespace godot {

class ResourceStorageEnum : public Object {
    GDCLASS(ResourceStorageEnum, Object);

public:
    static String get_accountstate() { return "AccountState"; }
    static String get_ledgerstate() { return "LedgerState"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("ResourceStorageEnum", D_METHOD("get_accountstate"), &ResourceStorageEnum::get_accountstate);
        ClassDB::bind_static_method("ResourceStorageEnum", D_METHOD("get_ledgerstate"), &ResourceStorageEnum::get_ledgerstate);
    }
};

} // namespace godot

namespace godot {

class ResourceCustodyEnum : public Object {
    GDCLASS(ResourceCustodyEnum, Object);

public:
    static String get_authority() { return "Authority"; }
    static String get_supply() { return "Supply"; }

  static void _bind_methods() {
        ClassDB::bind_static_method("ResourceCustodyEnum", D_METHOD("get_authority"), &ResourceCustodyEnum::get_authority);
        ClassDB::bind_static_method("ResourceCustodyEnum", D_METHOD("get_supply"), &ResourceCustodyEnum::get_supply);
    }
};

} // namespace godot

#endif // HONEYCOMB_ENUM_CLASSES_HPP

#define REGISTER_HONEYCOMB_ENUM \
    ClassDB::register_class<godot::MintAsKind>(); \
    ClassDB::register_class<godot::CharacterSourceKind>(); \
    ClassDB::register_class<godot::AssetCriteriaKind>(); \
    ClassDB::register_class<godot::SourceKind>(); \
    ClassDB::register_class<godot::CivicGateway>(); \
    ClassDB::register_class<godot::BadgesCondition>(); \
    ClassDB::register_class<godot::RemoveSocialsInput>(); \
    ClassDB::register_class<godot::HiveControlPermissionInput>(); \
    ClassDB::register_class<godot::AssetAssemblerPermissionInput>(); \
    ClassDB::register_class<godot::AssetManagerPermissionInput>(); \
    ClassDB::register_class<godot::ResourceManagerPermissionInput>(); \
    ClassDB::register_class<godot::NectarStakingPermissionInput>(); \
    ClassDB::register_class<godot::NectarMissionsPermissionInput>(); \
    ClassDB::register_class<godot::BuzzGuildPermissionInput>(); \
    ClassDB::register_class<godot::LockTypeEnum>(); \
    ClassDB::register_class<godot::RewardKind>(); \
    ClassDB::register_class<godot::ResourceStorageEnum>(); \
    ClassDB::register_class<godot::ResourceCustodyEnum>(); 

