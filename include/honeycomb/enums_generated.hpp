// Godot-compatible enum classes generated for Honeycomb enums
#ifndef HONEYCOMB_ENUM_CLASSES_HPP
#define HONEYCOMB_ENUM_CLASSES_HPP

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'MintAsKind'
 */
class MintAsKind : public Object {
	GDCLASS(MintAsKind, Object);

public:
	/** @brief Returns enum value 'MplCore' */
	static String get_mplcore() { return "MplCore"; }
	/** @brief Returns enum value 'MplMetadata' */
	static String get_mplmetadata() { return "MplMetadata"; }
	/** @brief Returns enum value 'MplBubblegum' */
	static String get_mplbubblegum() { return "MplBubblegum"; }
	/** @brief Returns enum value 'TokenExtensions' */
	static String get_tokenextensions() { return "TokenExtensions"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("MintAsKind", D_METHOD("get_mplcore"), &MintAsKind::get_mplcore);
		ClassDB::bind_static_method("MintAsKind", D_METHOD("get_mplmetadata"), &MintAsKind::get_mplmetadata);
		ClassDB::bind_static_method("MintAsKind", D_METHOD("get_mplbubblegum"), &MintAsKind::get_mplbubblegum);
		ClassDB::bind_static_method("MintAsKind", D_METHOD("get_tokenextensions"), &MintAsKind::get_tokenextensions);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'CharacterSourceKind'
 */
class CharacterSourceKind : public Object {
	GDCLASS(CharacterSourceKind, Object);

public:
	/** @brief Returns enum value 'Wrapped' */
	static String get_wrapped() { return "Wrapped"; }
	/** @brief Returns enum value 'Assembled' */
	static String get_assembled() { return "Assembled"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("CharacterSourceKind", D_METHOD("get_wrapped"), &CharacterSourceKind::get_wrapped);
		ClassDB::bind_static_method("CharacterSourceKind", D_METHOD("get_assembled"), &CharacterSourceKind::get_assembled);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'AssetCriteriaKind'
 */
class AssetCriteriaKind : public Object {
	GDCLASS(AssetCriteriaKind, Object);

public:
	/** @brief Returns enum value 'Prepopulated' */
	static String get_prepopulated() { return "Prepopulated"; }
	/** @brief Returns enum value 'Collection' */
	static String get_collection() { return "Collection"; }
	/** @brief Returns enum value 'Creator' */
	static String get_creator() { return "Creator"; }
	/** @brief Returns enum value 'MerkleTree' */
	static String get_merkletree() { return "MerkleTree"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("AssetCriteriaKind", D_METHOD("get_prepopulated"), &AssetCriteriaKind::get_prepopulated);
		ClassDB::bind_static_method("AssetCriteriaKind", D_METHOD("get_collection"), &AssetCriteriaKind::get_collection);
		ClassDB::bind_static_method("AssetCriteriaKind", D_METHOD("get_creator"), &AssetCriteriaKind::get_creator);
		ClassDB::bind_static_method("AssetCriteriaKind", D_METHOD("get_merkletree"), &AssetCriteriaKind::get_merkletree);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'SourceKind'
 */
class SourceKind : public Object {
	GDCLASS(SourceKind, Object);

public:
	/** @brief Returns enum value 'Assembled' */
	static String get_assembled() { return "Assembled"; }
	/** @brief Returns enum value 'MplMetadata' */
	static String get_mplmetadata() { return "MplMetadata"; }
	/** @brief Returns enum value 'MplBubblegum' */
	static String get_mplbubblegum() { return "MplBubblegum"; }
	/** @brief Returns enum value 'TokenExtensions' */
	static String get_tokenextensions() { return "TokenExtensions"; }
	/** @brief Returns enum value 'MplCoreAsset' */
	static String get_mplcoreasset() { return "MplCoreAsset"; }

	/** @brief Register static getters with Godot */
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

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'CivicGateway'
 */
class CivicGateway : public Object {
	GDCLASS(CivicGateway, Object);

public:
	/** @brief Returns enum value 'LivenessPass' */
	static String get_livenesspass() { return "LivenessPass"; }
	/** @brief Returns enum value 'UniquenessPass' */
	static String get_uniquenesspass() { return "UniquenessPass"; }
	/** @brief Returns enum value 'IdVerificationPass' */
	static String get_idverificationpass() { return "IdVerificationPass"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("CivicGateway", D_METHOD("get_livenesspass"), &CivicGateway::get_livenesspass);
		ClassDB::bind_static_method("CivicGateway", D_METHOD("get_uniquenesspass"), &CivicGateway::get_uniquenesspass);
		ClassDB::bind_static_method("CivicGateway", D_METHOD("get_idverificationpass"), &CivicGateway::get_idverificationpass);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'BadgesCondition'
 */
class BadgesCondition : public Object {
	GDCLASS(BadgesCondition, Object);

public:
	/** @brief Returns enum value 'Public' */
	static String get_public() { return "Public"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("BadgesCondition", D_METHOD("get_public"), &BadgesCondition::get_public);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'RemoveSocialsInput'
 */
class RemoveSocialsInput : public Object {
	GDCLASS(RemoveSocialsInput, Object);

public:
	/** @brief Returns enum value 'Twitter' */
	static String get_twitter() { return "Twitter"; }
	/** @brief Returns enum value 'Discord' */
	static String get_discord() { return "Discord"; }
	/** @brief Returns enum value 'Steam' */
	static String get_steam() { return "Steam"; }
	/** @brief Returns enum value 'All' */
	static String get_all() { return "All"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("RemoveSocialsInput", D_METHOD("get_twitter"), &RemoveSocialsInput::get_twitter);
		ClassDB::bind_static_method("RemoveSocialsInput", D_METHOD("get_discord"), &RemoveSocialsInput::get_discord);
		ClassDB::bind_static_method("RemoveSocialsInput", D_METHOD("get_steam"), &RemoveSocialsInput::get_steam);
		ClassDB::bind_static_method("RemoveSocialsInput", D_METHOD("get_all"), &RemoveSocialsInput::get_all);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'HiveControlPermissionInput'
 */
class HiveControlPermissionInput : public Object {
	GDCLASS(HiveControlPermissionInput, Object);

public:
	/** @brief Returns enum value 'ManageCriterias' */
	static String get_managecriterias() { return "ManageCriterias"; }
	/** @brief Returns enum value 'ManageServices' */
	static String get_manageservices() { return "ManageServices"; }
	/** @brief Returns enum value 'ManageProjectDriver' */
	static String get_manageprojectdriver() { return "ManageProjectDriver"; }
	/** @brief Returns enum value 'UpdatePlatformData' */
	static String get_updateplatformdata() { return "UpdatePlatformData"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("HiveControlPermissionInput", D_METHOD("get_managecriterias"), &HiveControlPermissionInput::get_managecriterias);
		ClassDB::bind_static_method("HiveControlPermissionInput", D_METHOD("get_manageservices"), &HiveControlPermissionInput::get_manageservices);
		ClassDB::bind_static_method("HiveControlPermissionInput", D_METHOD("get_manageprojectdriver"), &HiveControlPermissionInput::get_manageprojectdriver);
		ClassDB::bind_static_method("HiveControlPermissionInput", D_METHOD("get_updateplatformdata"), &HiveControlPermissionInput::get_updateplatformdata);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'CharacterManagerPermissionInput'
 */
class CharacterManagerPermissionInput : public Object {
	GDCLASS(CharacterManagerPermissionInput, Object);

public:
	/** @brief Returns enum value 'ManageAssemblerConfig' */
	static String get_manageassemblerconfig() { return "ManageAssemblerConfig"; }
	/** @brief Returns enum value 'ManageCharacterModels' */
	static String get_managecharactermodels() { return "ManageCharacterModels"; }
	/** @brief Returns enum value 'AssignCharacterTraits' */
	static String get_assigncharactertraits() { return "AssignCharacterTraits"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("CharacterManagerPermissionInput", D_METHOD("get_manageassemblerconfig"), &CharacterManagerPermissionInput::get_manageassemblerconfig);
		ClassDB::bind_static_method("CharacterManagerPermissionInput", D_METHOD("get_managecharactermodels"), &CharacterManagerPermissionInput::get_managecharactermodels);
		ClassDB::bind_static_method("CharacterManagerPermissionInput", D_METHOD("get_assigncharactertraits"), &CharacterManagerPermissionInput::get_assigncharactertraits);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'ResourceManagerPermissionInput'
 */
class ResourceManagerPermissionInput : public Object {
	GDCLASS(ResourceManagerPermissionInput, Object);

public:
	/** @brief Returns enum value 'CreateResources' */
	static String get_createresources() { return "CreateResources"; }
	/** @brief Returns enum value 'MintResources' */
	static String get_mintresources() { return "MintResources"; }
	/** @brief Returns enum value 'BurnResources' */
	static String get_burnresources() { return "BurnResources"; }
	/** @brief Returns enum value 'CreateFaucet' */
	static String get_createfaucet() { return "CreateFaucet"; }
	/** @brief Returns enum value 'CreateRecipe' */
	static String get_createrecipe() { return "CreateRecipe"; }

	/** @brief Register static getters with Godot */
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

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'NectarStakingPermissionInput'
 */
class NectarStakingPermissionInput : public Object {
	GDCLASS(NectarStakingPermissionInput, Object);

public:
	/** @brief Returns enum value 'ManageStakingPool' */
	static String get_managestakingpool() { return "ManageStakingPool"; }
	/** @brief Returns enum value 'WithdrawStakingPoolRewards' */
	static String get_withdrawstakingpoolrewards() { return "WithdrawStakingPoolRewards"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("NectarStakingPermissionInput", D_METHOD("get_managestakingpool"), &NectarStakingPermissionInput::get_managestakingpool);
		ClassDB::bind_static_method("NectarStakingPermissionInput", D_METHOD("get_withdrawstakingpoolrewards"), &NectarStakingPermissionInput::get_withdrawstakingpoolrewards);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'NectarMissionsPermissionInput'
 */
class NectarMissionsPermissionInput : public Object {
	GDCLASS(NectarMissionsPermissionInput, Object);

public:
	/** @brief Returns enum value 'ManageMissionPool' */
	static String get_managemissionpool() { return "ManageMissionPool"; }
	/** @brief Returns enum value 'WithdrawMissionPoolRewards' */
	static String get_withdrawmissionpoolrewards() { return "WithdrawMissionPoolRewards"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("NectarMissionsPermissionInput", D_METHOD("get_managemissionpool"), &NectarMissionsPermissionInput::get_managemissionpool);
		ClassDB::bind_static_method("NectarMissionsPermissionInput", D_METHOD("get_withdrawmissionpoolrewards"), &NectarMissionsPermissionInput::get_withdrawmissionpoolrewards);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'BuzzGuildPermissionInput'
 */
class BuzzGuildPermissionInput : public Object {
	GDCLASS(BuzzGuildPermissionInput, Object);

public:
	/** @brief Returns enum value 'ManageGuildKit' */
	static String get_manageguildkit() { return "ManageGuildKit"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("BuzzGuildPermissionInput", D_METHOD("get_manageguildkit"), &BuzzGuildPermissionInput::get_manageguildkit);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'LockTypeEnum'
 */
class LockTypeEnum : public Object {
	GDCLASS(LockTypeEnum, Object);

public:
	/** @brief Returns enum value 'Freeze' */
	static String get_freeze() { return "Freeze"; }
	/** @brief Returns enum value 'Custody' */
	static String get_custody() { return "Custody"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("LockTypeEnum", D_METHOD("get_freeze"), &LockTypeEnum::get_freeze);
		ClassDB::bind_static_method("LockTypeEnum", D_METHOD("get_custody"), &LockTypeEnum::get_custody);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'RewardKind'
 */
class RewardKind : public Object {
	GDCLASS(RewardKind, Object);

public:
	/** @brief Returns enum value 'Xp' */
	static String get_xp() { return "Xp"; }
	/** @brief Returns enum value 'Resource' */
	static String get_resource() { return "Resource"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("RewardKind", D_METHOD("get_xp"), &RewardKind::get_xp);
		ClassDB::bind_static_method("RewardKind", D_METHOD("get_resource"), &RewardKind::get_resource);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'ResourceStorageEnum'
 */
class ResourceStorageEnum : public Object {
	GDCLASS(ResourceStorageEnum, Object);

public:
	/** @brief Returns enum value 'AccountState' */
	static String get_accountstate() { return "AccountState"; }
	/** @brief Returns enum value 'LedgerState' */
	static String get_ledgerstate() { return "LedgerState"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("ResourceStorageEnum", D_METHOD("get_accountstate"), &ResourceStorageEnum::get_accountstate);
		ClassDB::bind_static_method("ResourceStorageEnum", D_METHOD("get_ledgerstate"), &ResourceStorageEnum::get_ledgerstate);
	}
};

} // namespace godot

namespace godot {

/**
 * @brief Godot wrapper for Honeycomb GraphQL enum 'ResourceCustodyEnum'
 */
class ResourceCustodyEnum : public Object {
	GDCLASS(ResourceCustodyEnum, Object);

public:
	/** @brief Returns enum value 'Authority' */
	static String get_authority() { return "Authority"; }
	/** @brief Returns enum value 'Supply' */
	static String get_supply() { return "Supply"; }

	/** @brief Register static getters with Godot */
	static void _bind_methods() {
		ClassDB::bind_static_method("ResourceCustodyEnum", D_METHOD("get_authority"), &ResourceCustodyEnum::get_authority);
		ClassDB::bind_static_method("ResourceCustodyEnum", D_METHOD("get_supply"), &ResourceCustodyEnum::get_supply);
	}
};

} // namespace godot

#endif // HONEYCOMB_ENUM_CLASSES_HPP

// Register generated Honeycomb enum classes with Godot
#define REGISTER_HONEYCOMB_ENUM                                        \
	ClassDB::register_class<godot::MintAsKind>();                      \
	ClassDB::register_class<godot::CharacterSourceKind>();             \
	ClassDB::register_class<godot::AssetCriteriaKind>();               \
	ClassDB::register_class<godot::SourceKind>();                      \
	ClassDB::register_class<godot::CivicGateway>();                    \
	ClassDB::register_class<godot::BadgesCondition>();                 \
	ClassDB::register_class<godot::RemoveSocialsInput>();              \
	ClassDB::register_class<godot::HiveControlPermissionInput>();      \
	ClassDB::register_class<godot::CharacterManagerPermissionInput>(); \
	ClassDB::register_class<godot::ResourceManagerPermissionInput>();  \
	ClassDB::register_class<godot::NectarStakingPermissionInput>();    \
	ClassDB::register_class<godot::NectarMissionsPermissionInput>();   \
	ClassDB::register_class<godot::BuzzGuildPermissionInput>();        \
	ClassDB::register_class<godot::LockTypeEnum>();                    \
	ClassDB::register_class<godot::RewardKind>();                      \
	ClassDB::register_class<godot::ResourceStorageEnum>();             \
	ClassDB::register_class<godot::ResourceCustodyEnum>();
