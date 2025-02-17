#include "honeycomb/honeycomb.hpp"

#include "honeycomb/types/CharacterHistory.hpp"
#include "honeycomb/types/AssemblerConfig.hpp"
#include "honeycomb/types/CharacterTrait.hpp"
#include "honeycomb/types/CharacterModel.hpp"
#include "honeycomb/types/CharacterConfigWrapped.hpp"
#include "honeycomb/types/CharacterConfigAssembled.hpp"
#include "honeycomb/types/NftCreator.hpp"
#include "honeycomb/types/MintAs.hpp"
#include "honeycomb/types/MintAsParamsMplBubblegum.hpp"
#include "honeycomb/types/CharacterCooldown.hpp"
#include "honeycomb/types/Character.hpp"
#include "honeycomb/types/CharacterSource.hpp"
#include "honeycomb/types/Wrapped.hpp"
#include "honeycomb/types/AssetCriteria.hpp"
#include "honeycomb/types/Assembled.hpp"
#include "honeycomb/types/CharacterUsedBy.hpp"
#include "honeycomb/types/UsedByStaking.hpp"
#include "honeycomb/types/UsedByMission.hpp"
#include "honeycomb/types/EarnedRewards.hpp"
#include "honeycomb/types/UsedByGuild.hpp"
#include "honeycomb/types/GuildRole.hpp"
#include "honeycomb/types/UsedByEjected.hpp"
#include "honeycomb/types/UsedByCustom.hpp"
#include "honeycomb/types/Global.hpp"
#include "honeycomb/types/Project.hpp"
#include "honeycomb/types/DelegateAuthority.hpp"
#include "honeycomb/types/ServiceDelegation.hpp"
#include "honeycomb/types/Service.hpp"
#include "honeycomb/types/ServiceParamsAssembler.hpp"
#include "honeycomb/types/ServiceParamsAssetManager.hpp"
#include "honeycomb/types/ServiceParamsStaking.hpp"
#include "honeycomb/types/ServiceParamsMissions.hpp"
#include "honeycomb/types/ServiceParamsRaffles.hpp"
#include "honeycomb/types/ServiceParamsGuildKit.hpp"
#include "honeycomb/types/AssociatedProgram.hpp"
#include "honeycomb/types/SerializableActions.hpp"
#include "honeycomb/types/ProfileDataConfig.hpp"
#include "honeycomb/types/User.hpp"
#include "honeycomb/types/HoneycombUserInfo.hpp"
#include "honeycomb/types/CivicInfo.hpp"
#include "honeycomb/types/SocialInfo.hpp"
#include "honeycomb/types/Wallets.hpp"
#include "honeycomb/types/Profile.hpp"
#include "honeycomb/types/ProfileInfo.hpp"
#include "honeycomb/types/PlatformData.hpp"
#include "honeycomb/types/BadgeCriteria.hpp"
#include "honeycomb/types/HoneycombAccount.hpp"
#include "honeycomb/types/CompressedAccount.hpp"
#include "honeycomb/types/HoneycombNode.hpp"
#include "honeycomb/types/Proof.hpp"
#include "honeycomb/types/ControlledMerkleTrees.hpp"
#include "honeycomb/types/MissionPool.hpp"
#include "honeycomb/types/Mission.hpp"
#include "honeycomb/types/MissionCost.hpp"
#include "honeycomb/types/TimeRequirement.hpp"
#include "honeycomb/types/TimeRequirementParams.hpp"
#include "honeycomb/types/Reward.hpp"
#include "honeycomb/types/XpRewardType.hpp"
#include "honeycomb/types/ResourceRewardType.hpp"
#include "honeycomb/types/ResourceRewardTypeParams.hpp"
#include "honeycomb/types/StakingPool.hpp"
#include "honeycomb/types/LockType.hpp"
#include "honeycomb/types/Multipliers.hpp"
#include "honeycomb/types/Multiplier.hpp"
#include "honeycomb/types/MultiplierType.hpp"
#include "honeycomb/types/MultiplierTypeParamsStakeDuration.hpp"
#include "honeycomb/types/MultiplierTypeParamsNFTCount.hpp"
#include "honeycomb/types/MultiplierTypeParamsCreator.hpp"
#include "honeycomb/types/MultiplierTypeParamsCollection.hpp"
#include "honeycomb/types/Staker.hpp"
#include "honeycomb/types/HoneycombTransaction.hpp"
#include "honeycomb/types/CreateCreateProjectTransactionResponse.hpp"
#include "honeycomb/types/CreateAssemblerConfigTransactionResponse.hpp"
#include "honeycomb/types/CreateCharacterModelTransactionResponse.hpp"
#include "honeycomb/types/CreateInitResourceTransactionResponse.hpp"
#include "honeycomb/types/CreateCreateNewResourceTreeTransactionResponse.hpp"
#include "honeycomb/types/CreateInitializeRecipeTransactionResponse.hpp"
#include "honeycomb/types/CreateBeginCookingTransactionResponse.hpp"
#include "honeycomb/types/Transactions.hpp"
#include "honeycomb/types/TransactionResponse.hpp"
#include "honeycomb/types/TransactionBundleResponse.hpp"
#include "honeycomb/types/AuthResponse.hpp"
#include "honeycomb/types/AuthConfirmed.hpp"
#include "honeycomb/types/OutputHoldingTree.hpp"
#include "honeycomb/types/CreateCreateHolderAccountTransactionResponse.hpp"
#include "honeycomb/types/CreateInitMultipliersTransactionResponse.hpp"
#include "honeycomb/types/CreateCreateStakingPoolTransactionResponse.hpp"
#include "honeycomb/types/CreateCreateMissionPoolTransactionResponse.hpp"
#include "honeycomb/types/CreateUpdateMissionPoolTransactionResponse.hpp"
#include "honeycomb/types/CreateCreateMissionTransactionResponse.hpp"
#include "honeycomb/types/CreateUpdateMissionTransactionResponse.hpp"
#include "honeycomb/types/CreateInitializeFaucetTransactionResponse.hpp"
#include "honeycomb/types/TreeSetupResponse.hpp"
#include "honeycomb/types/HoneycombResource.hpp"
#include "honeycomb/types/ResourceBalance.hpp"
#include "honeycomb/types/ResourceStorage.hpp"
#include "honeycomb/types/ResourceStorageParams.hpp"
#include "honeycomb/types/ResourceKind.hpp"
#include "honeycomb/types/ResourceKindParamsHplFungible.hpp"
#include "honeycomb/types/ResourceKindParamsWrappedFungible.hpp"
#include "honeycomb/types/ResourceCustody.hpp"
#include "honeycomb/types/ResourceCustodyParams.hpp"
#include "honeycomb/types/ResourceKindParamsHplNonFungible.hpp"
#include "honeycomb/types/ResourceKindParamsWrappedMplCore.hpp"
#include "honeycomb/types/Recipe.hpp"
#include "honeycomb/types/Ingredient.hpp"
#include "honeycomb/types/Meal.hpp"
#include "honeycomb/types/Faucet.hpp"
#include "honeycomb/types/Holding.hpp"
#include "honeycomb/types/CharacterTraitInput.hpp"
#include "honeycomb/types/MintAsInput.hpp"
#include "honeycomb/types/CharacterCooldownInput.hpp"
#include "honeycomb/types/MintAsMplBubblegumInput.hpp"
#include "honeycomb/types/CharacterConfigInput.hpp"
#include "honeycomb/types/AssemblerConfigInput.hpp"
#include "honeycomb/types/NftCreatorInput.hpp"
#include "honeycomb/types/AssetCriteriaInput.hpp"
#include "honeycomb/types/CharactersFilter.hpp"
#include "honeycomb/types/CharacterSourceFilter.hpp"
#include "honeycomb/types/CharacterSourceParamsFilter.hpp"
#include "honeycomb/types/AssetCriteriaFilter.hpp"
#include "honeycomb/types/CharacterUsedByFilter.hpp"
#include "honeycomb/types/CharacterUsedByParamsFilter.hpp"
#include "honeycomb/types/EarnedRewardsFilter.hpp"
#include "honeycomb/types/UsedByGuildFilter.hpp"
#include "honeycomb/types/GuildRoleFilter.hpp"
#include "honeycomb/types/CivicInfoInput.hpp"
#include "honeycomb/types/UserInfoInput.hpp"
#include "honeycomb/types/PartialUserInfoInput.hpp"
#include "honeycomb/types/UpdateWalletInput.hpp"
#include "honeycomb/types/AssociatedProgramInput.hpp"
#include "honeycomb/types/SerializableActionsInput.hpp"
#include "honeycomb/types/ProfileDataConfigInput.hpp"
#include "honeycomb/types/ProfileInfoInput.hpp"
#include "honeycomb/types/CustomDataInput.hpp"
#include "honeycomb/types/PlatformDataInput.hpp"
#include "honeycomb/types/ServiceDelegationInput.hpp"
#include "honeycomb/types/ServiceDelegationHiveControl.hpp"
#include "honeycomb/types/ServiceDelegationAssetAssembler.hpp"
#include "honeycomb/types/ServiceDelegationAssetManager.hpp"
#include "honeycomb/types/ServiceDelegationResourceManager.hpp"
#include "honeycomb/types/ServiceDelegationNectarStaking.hpp"
#include "honeycomb/types/ServiceDelegationNectarMissions.hpp"
#include "honeycomb/types/ServiceDelegationBuzzGuild.hpp"
#include "honeycomb/types/ModifyServiceDelegationInput.hpp"
#include "honeycomb/types/ModifyDelegationInput.hpp"
#include "honeycomb/types/UpdateMissionInput.hpp"
#include "honeycomb/types/CreateStakingPoolMetadataInput.hpp"
#include "honeycomb/types/UpdateStakingPoolMetadataInput.hpp"
#include "honeycomb/types/MultiplierTypeInput.hpp"
#include "honeycomb/types/AddMultiplierMetadataInput.hpp"
#include "honeycomb/types/InitStakingMultiplierMetadataInput.hpp"
#include "honeycomb/types/DisrcriminatorFilter.hpp"
#include "honeycomb/types/TreeLeaf.hpp"
#include "honeycomb/types/TreeSetupConfig.hpp"
#include "honeycomb/types/BasicTreeConfig.hpp"
#include "honeycomb/types/AdvancedTreeConfig.hpp"
#include "honeycomb/types/RecipeInputResource.hpp"
#include "honeycomb/types/RecipeInputResources.hpp"
#include "honeycomb/types/RecipeOutputResource.hpp"
#include "honeycomb/types/RecipeResources.hpp"
#include "honeycomb/types/NewMissionPoolData.hpp"
#include "honeycomb/types/UpdateMissionPoolData.hpp"
#include "honeycomb/types/NewMissionCost.hpp"
#include "honeycomb/types/MissionReward.hpp"
#include "honeycomb/types/NewMissionData.hpp"
#include "honeycomb/types/UpdateMissionData.hpp"
#include "honeycomb/types/ParticipateOnMissionData.hpp"
#include "honeycomb/types/RecallFromMissionData.hpp"
#include "honeycomb/types/CreateBadgeCriteriaInput.hpp"
#include "honeycomb/types/ClaimBadgeCriteriaInput.hpp"
#include "honeycomb/types/UpdateBadgeCriteriaInput.hpp"
#include "honeycomb/types/SendTransactionsOptions.hpp"
#include "honeycomb/types/SendTransactionBundlesOptions.hpp"
#include "honeycomb/types/TransactionBundlesOptions.hpp"
#include "honeycomb/types/InitResourceInput.hpp"
#include "honeycomb/types/IngredientsInput.hpp"
#include "honeycomb/types/MealInput.hpp"
#include "honeycomb/types/ImportResourceInput.hpp"
#include "honeycomb/types/ImportResourceInputCustodyInput.hpp"
namespace godot {

Variant HoneyComb::findCompressedAccounts(Array addresses, Variant identity, Array leaves, Dictionary parsedData){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("addresses", "[Bytes!]", addresses, false);
	if (identity != Variant(nullptr)) {
		add_arg("identity", "DisrcriminatorFilter", Object::cast_to<godot::honeycomb_resource::DisrcriminatorFilter>(identity)->to_dict(), true);
	}
	if (leaves != Array()) {
		add_arg("leaves", "[TreeLeaf!]", leaves, true);
	}
	if (parsedData != Dictionary()) {
		add_arg("parsedData", "JSON", parsedData, true);
	}


	method_name = "compressedAccount";


	query_fields = "leaf_idx parsed_data raw_data schema_validated tree_id";
	send_query();
	return OK;
}

Variant HoneyComb::fetchProofs(Array leaves){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("leaves", "[TreeLeaf!]", leaves, false);


	method_name = "proof";


	query_fields = "root proof maxDepth node_index leaf leaf_index tree_id canopy_depth";
	send_query();
	return OK;
}

Variant HoneyComb::findAccounts(PackedByteArray programId, PackedByteArray discriminator, Dictionary parsedData){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("programId", "Bytes", programId, false);
	if (discriminator != PackedByteArray()) {
		add_arg("discriminator", "Bytes", discriminator, true);
	}
	if (parsedData != Dictionary()) {
		add_arg("parsedData", "JSON", parsedData, true);
	}


	method_name = "account";


	query_fields = "address program_id discriminator parsed_data";
	send_query();
	return OK;
}

Variant HoneyComb::sendTransactionBundles(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("txs", "[Bytes!]", txs, false);
	add_arg("blockhash", "String", Pubkey::string_from_variant(blockhash), false);
	add_arg("lastValidBlockHeight", "Int", lastValidBlockHeight, false);
	if (options != Variant(nullptr)) {
		add_arg("options", "SendTransactionBundlesOptions", Object::cast_to<godot::honeycomb_resource::SendTransactionBundlesOptions>(options)->to_dict(), true);
	}


	method_name = "sendTransactionBundles";


	query_fields = "bundleId responses { signature error status }";
	send_query();
	return OK;
}

Variant HoneyComb::signWithShadowSignerAndSendTransactionBundles(Array txs, String blockhash, int32_t lastValidBlockHeight, String authToken, Variant options){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("txs", "[Bytes!]", txs, false);
	add_arg("blockhash", "String", Pubkey::string_from_variant(blockhash), false);
	add_arg("lastValidBlockHeight", "Int", lastValidBlockHeight, false);
	add_arg("authToken", "String", Pubkey::string_from_variant(authToken), false);
	if (options != Variant(nullptr)) {
		add_arg("options", "SendTransactionBundlesOptions", Object::cast_to<godot::honeycomb_resource::SendTransactionBundlesOptions>(options)->to_dict(), true);
	}


	method_name = "signWithShadowSignerAndSendTransactionBundles";


	query_fields = "bundleId responses { signature error status }";
	send_query();
	return OK;
}

Variant HoneyComb::sendBulkTransactions(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("txs", "[Bytes!]", txs, false);
	add_arg("blockhash", "String", Pubkey::string_from_variant(blockhash), false);
	add_arg("lastValidBlockHeight", "Int", lastValidBlockHeight, false);
	if (options != Variant(nullptr)) {
		add_arg("options", "SendTransactionBundlesOptions", Object::cast_to<godot::honeycomb_resource::SendTransactionBundlesOptions>(options)->to_dict(), true);
	}


	method_name = "sendBulkTransactions";


	query_fields = "signature error status";
	send_query();
	return OK;
}

Variant HoneyComb::signWithShadowSignerAndSendBulkTransactions(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("txs", "[Bytes!]", txs, false);
	add_arg("blockhash", "String", Pubkey::string_from_variant(blockhash), false);
	add_arg("lastValidBlockHeight", "Int", lastValidBlockHeight, false);
	if (options != Variant(nullptr)) {
		add_arg("options", "SendTransactionBundlesOptions", Object::cast_to<godot::honeycomb_resource::SendTransactionBundlesOptions>(options)->to_dict(), true);
	}


	method_name = "signWithShadowSignerAndSendBulkTransactions";


	query_fields = "signature error status";
	send_query();
	return OK;
}

Variant HoneyComb::authRequest(String wallet, bool useTx, String useRpc){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if (useTx != false) {
		add_arg("useTx", "Boolean", useTx, true);
	}
	if (useRpc != "") {
		add_arg("useRpc", "String", Pubkey::string_from_variant(useRpc), true);
	}


	method_name = "authRequest";


	query_fields = "message tx";
	send_query();
	return OK;
}

Variant HoneyComb::authConfirm(String wallet, String signature){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	add_arg("signature", "String", Pubkey::string_from_variant(signature), false);


	method_name = "authConfirm";


	query_fields = "accessToken user { address id info { bio name pfp } wallets { shadow wallets }  }";
	send_query();
	return OK;
}

Variant HoneyComb::createTransferAssetsTransactions(PackedStringArray addresses, String to, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("addresses", "[String!]", addresses, false);
	add_arg("to", "String", Pubkey::string_from_variant(to), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createTransferAssetsTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::createBurnAssetsTransactions(PackedStringArray addresses, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("addresses", "[String!]", addresses, false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createBurnAssetsTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::findGlobal(String env){
	if (pending) {
		return ERR_BUSY;
	}

	if (env != "") {
		add_arg("env", "String", Pubkey::string_from_variant(env), true);
	}


	method_name = "global";


	query_fields = "address bump config userTrees { active merkle_trees schema } totalUsers";
	send_query();
	return OK;
}

Variant HoneyComb::findProjects(Array addresses, PackedStringArray names, Array authorities){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (names != PackedStringArray()) {
		add_arg("names", "[String!]", names, true);
	}
	if (authorities != Array()) {
		add_arg("authorities", "[Pubkey!]", authorities, true);
	}


	method_name = "project";


	query_fields = "associatedPrograms { address trustedActions { kind }  } authority bump driver address key name profileDataConfig { achievements customDataFields } profileTrees { active merkle_trees schema } badgeCriteria { index condition endTime startTime } services { kind params { ... on ServiceParamsAssembler { assemblerId } ... on ServiceParamsAssetManager { assetManagerId } ... on ServiceParamsStaking { poolId } ... on ServiceParamsMissions { poolId } ... on ServiceParamsRaffles { poolId } ... on ServiceParamsGuildKit { kitId }  }  } subsidyFees";
	send_query();
	return OK;
}

Variant HoneyComb::findDelegateAuthority(Array addresses, Array delegates, Array projects){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (delegates != Array()) {
		add_arg("delegates", "[Pubkey!]", delegates, true);
	}
	if (projects != Array()) {
		add_arg("projects", "[Pubkey!]", projects, true);
	}


	method_name = "delegateAuthority";


	query_fields = "authority bump delegations { kind index permission }";
	send_query();
	return OK;
}

Variant HoneyComb::findUsers(Array addresses, PackedInt32Array ids, Array wallets, bool includeProof, Array includeProjectProfiles){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (ids != PackedInt32Array()) {
		add_arg("ids", "[Int!]", ids, true);
	}
	if (wallets != Array()) {
		add_arg("wallets", "[Pubkey!]", wallets, true);
	}
	if (includeProof != false) {
		add_arg("includeProof", "Boolean", includeProof, true);
	}
	if (includeProjectProfiles != Array()) {
		add_arg("includeProjectProfiles", "[Pubkey!]", includeProjectProfiles, true);
	}


	method_name = "user";


	query_fields = "address id info { bio name pfp username } socialInfo { discord twitter civic { expiry gatekeeperNetwork walletIndex } steam } wallets { shadow wallets } profiles { customData address identity info { bio name pfp } platformData { achievements custom xp } project userId leaf_idx tree_id proof { leaf leaf_index node_index proof maxDepth root tree_id canopy_depth }  } leaf_idx tree_id proof { leaf leaf_index node_index maxDepth proof root tree_id canopy_depth }";
	send_query();
	return OK;
}

Variant HoneyComb::findProfiles(Array addresses, Array projects, PackedInt32Array userIds, PackedStringArray identities, bool includeProof, bool includeUsers){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (projects != Array()) {
		add_arg("projects", "[Pubkey!]", projects, true);
	}
	if (userIds != PackedInt32Array()) {
		add_arg("userIds", "[Int!]", userIds, true);
	}
	if (identities != PackedStringArray()) {
		add_arg("identities", "[String!]", identities, true);
	}
	if (includeProof != false) {
		add_arg("includeProof", "Boolean", includeProof, true);
	}
	if (includeUsers != false) {
		add_arg("includeUsers", "Boolean", includeUsers, true);
	}


	method_name = "profile";


	query_fields = "customData address identity info { bio name pfp } platformData { achievements custom xp } project userId leaf_idx tree_id proof { leaf leaf_index node_index proof maxDepth root tree_id canopy_depth } user { address id info { bio name pfp username } socialInfo { discord twitter civic { expiry gatekeeperNetwork walletIndex } steam } wallets { shadow wallets } leaf_idx tree_id proof { leaf leaf_index node_index maxDepth proof root tree_id canopy_depth }  }";
	send_query();
	return OK;
}

Variant HoneyComb::createNewUserTransaction(String wallet, Variant info, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if (info != Variant(nullptr)) {
		add_arg("info", "UserInfoInput", Object::cast_to<godot::honeycomb_resource::UserInfoInput>(info)->to_dict(), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createNewUserTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createNewUserBulkTransaction(PackedStringArray wallet, Array info, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("wallet", "[String!]", wallet, false);
	add_arg("info", "[UserInfoInput!]", info, false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createNewUserBulkTransaction";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateUserTransaction(String payer, Variant info, Variant wallets, bool populateCivic, Array removeSocials, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if (info != Variant(nullptr)) {
		add_arg("info", "PartialUserInfoInput", Object::cast_to<godot::honeycomb_resource::PartialUserInfoInput>(info)->to_dict(), true);
	}
	if (wallets != Variant(nullptr)) {
		add_arg("wallets", "UpdateWalletInput", Object::cast_to<godot::honeycomb_resource::UpdateWalletInput>(wallets)->to_dict(), true);
	}
	if (populateCivic != false) {
		add_arg("populateCivic", "Boolean", populateCivic, true);
	}
	if (removeSocials != Array()) {
		add_arg("removeSocials", "[RemoveSocialsInput!]", removeSocials, true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateUserTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateProjectTransaction(String authority, String name, String driver, Array associatedPrograms, Variant profileDataConfig, bool subsidizeFees, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("name", "String", Pubkey::string_from_variant(name), false);
	if (driver != "") {
		add_arg("driver", "String", Pubkey::string_from_variant(driver), true);
	}
	if (associatedPrograms != Array()) {
		add_arg("associatedPrograms", "[AssociatedProgramInput!]", associatedPrograms, true);
	}
	if (profileDataConfig != Variant(nullptr)) {
		add_arg("profileDataConfig", "ProfileDataConfigInput", Object::cast_to<godot::honeycomb_resource::ProfileDataConfigInput>(profileDataConfig)->to_dict(), true);
	}
	if (subsidizeFees != false) {
		add_arg("subsidizeFees", "Boolean", subsidizeFees, true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateProjectTransaction";


	query_fields = "project tx { blockhash lastValidBlockHeight transaction }";
	send_query();
	return OK;
}

Variant HoneyComb::createChangeProjectDriverTransaction(String project, String driver, String authority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("driver", "String", Pubkey::string_from_variant(driver), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createChangeProjectDriverTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateDelegateAuthorityTransaction(String project, String delegate, Variant serviceDelegations, String authority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("delegate", "String", Pubkey::string_from_variant(delegate), false);
	add_arg("serviceDelegations", "ServiceDelegationInput", Object::cast_to<godot::honeycomb_resource::ServiceDelegationInput>(serviceDelegations)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateDelegateAuthorityTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createModifyDelegationTransaction(String project, String delegate, Variant modifyDelegation, String authority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("delegate", "String", Pubkey::string_from_variant(delegate), false);
	add_arg("modifyDelegation", "ModifyDelegationInput", Object::cast_to<godot::honeycomb_resource::ModifyDelegationInput>(modifyDelegation)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createModifyDelegationTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateProfilesTreeTransaction(Variant treeConfig, String project, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<godot::honeycomb_resource::TreeSetupConfig>(treeConfig)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateProfilesTreeTransaction";


	query_fields = "tx { blockhash lastValidBlockHeight transaction } treeAddress proofBytes space cost maxTreeCapacity";
	send_query();
	return OK;
}

Variant HoneyComb::createNewProfileTransaction(String project, String payer, String identity, Variant info, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if (identity != "") {
		add_arg("identity", "String", Pubkey::string_from_variant(identity), true);
	}
	if (info != Variant(nullptr)) {
		add_arg("info", "ProfileInfoInput", Object::cast_to<godot::honeycomb_resource::ProfileInfoInput>(info)->to_dict(), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createNewProfileTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateProfileTransaction(String profile, String payer, Variant info, Variant customData, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("profile", "String", Pubkey::string_from_variant(profile), false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if (info != Variant(nullptr)) {
		add_arg("info", "ProfileInfoInput", Object::cast_to<godot::honeycomb_resource::ProfileInfoInput>(info)->to_dict(), true);
	}
	if (customData != Variant(nullptr)) {
		add_arg("customData", "CustomDataInput", Object::cast_to<godot::honeycomb_resource::CustomDataInput>(customData)->to_dict(), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateProfileTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdatePlatformDataTransaction(String profile, String authority, String delegateAuthority, String payer, Variant platformData, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("profile", "String", Pubkey::string_from_variant(profile), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (platformData != Variant(nullptr)) {
		add_arg("platformData", "PlatformDataInput", Object::cast_to<godot::honeycomb_resource::PlatformDataInput>(platformData)->to_dict(), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdatePlatformDataTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createNewUserWithProfileTransaction(String project, String wallet, Variant userInfo, String payer, String profileIdentity, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	add_arg("userInfo", "UserInfoInput", Object::cast_to<godot::honeycomb_resource::UserInfoInput>(userInfo)->to_dict(), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (profileIdentity != "") {
		add_arg("profileIdentity", "String", Pubkey::string_from_variant(profileIdentity), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createNewUserWithProfileTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::findAssemblerConfig(Array addresses, PackedStringArray tickers, Array characterModel, Array project){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (tickers != PackedStringArray()) {
		add_arg("tickers", "[String!]", tickers, true);
	}
	if (characterModel != Array()) {
		add_arg("characterModel", "[Bytes!]", characterModel, true);
	}
	if (project != Array()) {
		add_arg("project", "[Pubkey!]", project, true);
	}


	method_name = "assemblerConfig";


	query_fields = "address program_id discriminator ticker project order merkle_trees { active merkle_trees schema }";
	send_query();
	return OK;
}

Variant HoneyComb::findCharacterTraits(Array addresses, Array trees, bool includeProof){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (trees != Array()) {
		add_arg("trees", "[Bytes!]", trees, true);
	}
	if (includeProof != false) {
		add_arg("includeProof", "Boolean", includeProof, true);
	}


	method_name = "characterTrait";


	query_fields = "address layer leaf_idx name proof { root proof node_index leaf maxDepth leaf_index tree_id canopy_depth } tree_id uri";
	send_query();
	return OK;
}

Variant HoneyComb::findCharacterModels(Array addresses, Variant project, Array assemblerConfigs){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (project != Variant(nullptr)) {
		add_arg("project", "Pubkey", Object::cast_to<Pubkey>(project)->to_string(), true);
	}
	if (assemblerConfigs != Array()) {
		add_arg("assemblerConfigs", "[Pubkey!]", assemblerConfigs, true);
	}


	method_name = "characterModel";


	query_fields = "address program_id discriminator bump key equipableCriteria project attributes merkle_trees { active merkle_trees schema } cooldown { ejection } config { ... on CharacterConfigWrapped { criterias { kind params } kind } ... on CharacterConfigAssembled { kind assemblerConfig name symbol description creators { address share } sellerFeeBasisPoints collectionName mintAs { kind params { ... on MintAsParamsMplBubblegum { merkleTree }  }  }  }  }";
	send_query();
	return OK;
}

Variant HoneyComb::findCharacters(bool includeProof, Array addresses, Variant filters, Array trees, Array wallets, Array mints, Array attributeHashes){
	if (pending) {
		return ERR_BUSY;
	}

	if (includeProof != false) {
		add_arg("includeProof", "Boolean", includeProof, true);
	}
	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (filters != Variant(nullptr)) {
		add_arg("filters", "CharactersFilter", Object::cast_to<godot::honeycomb_resource::CharactersFilter>(filters)->to_dict(), true);
	}
	if (trees != Array()) {
		add_arg("trees", "[Bytes!]", trees, true);
	}
	if (wallets != Array()) {
		add_arg("wallets", "[Pubkey!]", wallets, true);
	}
	if (mints != Array()) {
		add_arg("mints", "[Pubkey!]", mints, true);
	}
	if (attributeHashes != Array()) {
		add_arg("attributeHashes", "[Pubkey!]", attributeHashes, true);
	}


	method_name = "character";


	query_fields = "source { kind params { ... on Wrapped { mint criteria { kind params } kind } ... on Assembled { hash mint uri attributes assemblerConfig }  } kind } owner proof { leaf leaf_index node_index maxDepth proof root tree_id canopy_depth } usedBy { kind params { ... on UsedByStaking { pool staker stakedAt claimedAt } ... on UsedByMission { missionId participationId rewards { delta rewardIdx collected } endTime } ... on UsedByGuild { id role { kind } order } ... on UsedByEjected { mint } ... on UsedByCustom { user data }  }  } equipments leaf_idx tree_id address asset cooldown { ejection }";
	send_query();
	return OK;
}

Variant HoneyComb::fetchCharacterHistory(Array addresses, PackedStringArray event){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("addresses", "[Bytes!]", addresses, false);
	if (event != PackedStringArray()) {
		add_arg("event", "[String!]", event, true);
	}


	method_name = "characterHistory";


	query_fields = "address event event_data time";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateAssemblerConfigTransaction(String ticker, String project, String authority, Variant treeConfig, PackedStringArray order, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("ticker", "String", Pubkey::string_from_variant(ticker), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (treeConfig != Variant(nullptr)) {
		add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<godot::honeycomb_resource::TreeSetupConfig>(treeConfig)->to_dict(), true);
	}
	if (order != PackedStringArray()) {
		add_arg("order", "[String!]", order, true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateAssemblerConfigTransaction";


	query_fields = "tx { blockhash lastValidBlockHeight transaction } assemblerConfig treeAddress proofBytes space cost maxTreeCapacity";
	send_query();
	return OK;
}

Variant HoneyComb::createAddCharacterTraitsTransactions(String assemblerConfig, Array traits, String authority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), false);
	add_arg("traits", "[CharacterTraitInput!]", traits, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createAddCharacterTraitsTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::createRemoveCharacterTraitsTransactions(String assemblerConfig, PackedStringArray traitsAddresses, String authority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), false);
	add_arg("traitsAddresses", "[String!]", traitsAddresses, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createRemoveCharacterTraitsTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateCharacterModelTransaction(Variant config, String project, String authority, Array attributes, Variant mintAs, Variant cooldown, PackedStringArray equipableCriteria, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("config", "CharacterConfigInput", Object::cast_to<godot::honeycomb_resource::CharacterConfigInput>(config)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (attributes != Array()) {
		add_arg("attributes", "VecMapGeneric", attributes, true);
	}
	if (mintAs != Variant(nullptr)) {
		add_arg("mintAs", "MintAsInput", Object::cast_to<godot::honeycomb_resource::MintAsInput>(mintAs)->to_dict(), true);
	}
	if (cooldown != Variant(nullptr)) {
		add_arg("cooldown", "CharacterCooldownInput", Object::cast_to<godot::honeycomb_resource::CharacterCooldownInput>(cooldown)->to_dict(), true);
	}
	if (equipableCriteria != PackedStringArray()) {
		add_arg("equipableCriteria", "[String!]", equipableCriteria, true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateCharacterModelTransaction";


	query_fields = "characterModel tx { blockhash lastValidBlockHeight transaction }";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateCharactersTreeTransaction(Variant treeConfig, String project, String characterModel, String authority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<godot::honeycomb_resource::TreeSetupConfig>(treeConfig)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateCharactersTreeTransaction";


	query_fields = "tx { blockhash lastValidBlockHeight transaction } treeAddress proofBytes space cost maxTreeCapacity";
	send_query();
	return OK;
}

Variant HoneyComb::createAssembleCharacterTransaction(String project, String assemblerConfig, String characterModel, String owner, String authority, String uri, Array attributes, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (uri != "") {
		add_arg("uri", "String", Pubkey::string_from_variant(uri), true);
	}
	if (attributes != Array()) {
		add_arg("attributes", "VecMapGeneric", attributes, true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createAssembleCharacterTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateCharacterTraitsTransaction(String characterAddress, String project, String assemblerConfig, String characterModel, String authority, String uri, Array attributes, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("characterAddress", "String", Pubkey::string_from_variant(characterAddress), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (uri != "") {
		add_arg("uri", "String", Pubkey::string_from_variant(uri), true);
	}
	if (attributes != Array()) {
		add_arg("attributes", "VecMapGeneric", attributes, true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateCharacterTraitsTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createPopulateCharacterTransaction(String project, String characterModel, String mint, String owner, String updateAuthority, String payer, Array attributes, String assemblerConfig, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("mint", "String", Pubkey::string_from_variant(mint), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	add_arg("updateAuthority", "String", Pubkey::string_from_variant(updateAuthority), false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if (attributes != Array()) {
		add_arg("attributes", "VecMapGeneric", attributes, true);
	}
	if (assemblerConfig != "") {
		add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createPopulateCharacterTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createWrapAssetsToCharacterTransactions(String project, String characterModel, String wallet, PackedStringArray mintList, String libreplexDeployment, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	add_arg("mintList", "[String!]", mintList, false);
	if (libreplexDeployment != "") {
		add_arg("libreplexDeployment", "String", Pubkey::string_from_variant(libreplexDeployment), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createWrapAssetsToCharacterTransactions";


	query_fields = "transactions blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createUnwrapAssetsFromCharacterTransactions(String project, PackedStringArray characterAddresses, String characterModel, String wallet, String libreplexDeployment, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterAddresses", "[String!]", characterAddresses, false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if (libreplexDeployment != "") {
		add_arg("libreplexDeployment", "String", Pubkey::string_from_variant(libreplexDeployment), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUnwrapAssetsFromCharacterTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::createEquipResourceOnCharacterTransaction(String characterModel, String characterAddress, String resource, int64_t amount, String owner, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("characterAddress", "String", Pubkey::string_from_variant(characterAddress), false);
	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "BigInt", amount, false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createEquipResourceOnCharacterTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createDismountResourceOnCharacterTransaction(String characterModel, String characterAddress, String resource, int64_t amount, String owner, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("characterAddress", "String", Pubkey::string_from_variant(characterAddress), false);
	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "BigInt", amount, false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createDismountResourceOnCharacterTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createUseCharacterTransaction(String character, String project, String characterModel, String user, Dictionary data, bool unUse, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("character", "String", Pubkey::string_from_variant(character), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("user", "String", Pubkey::string_from_variant(user), false);
	if (data != Dictionary()) {
		add_arg("data", "JSON", data, true);
	}
	if (unUse != false) {
		add_arg("unUse", "Boolean", unUse, true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUseCharacterTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::findStakingPools(Array addresses, Array projects, PackedStringArray names){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (projects != Array()) {
		add_arg("projects", "[Pubkey!]", projects, true);
	}
	if (names != PackedStringArray()) {
		add_arg("names", "[String!]", names, true);
	}


	method_name = "stakingPools";


	query_fields = "allowedMints bump characterModels cooldownDuration resource endTime key lockType { kind } maxRewardsDuration minStakeDuration name project resetStakeDuration rewardsDuration rewardsPerDuration startTime totalStaked address";
	send_query();
	return OK;
}

Variant HoneyComb::findStakers(Array addresses, Array stakingPools, Array wallets){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (stakingPools != Array()) {
		add_arg("stakingPools", "[Pubkey!]", stakingPools, true);
	}
	if (wallets != Array()) {
		add_arg("wallets", "[Pubkey!]", wallets, true);
	}


	method_name = "stakers";


	query_fields = "bump stakingPool totalStaked wallet";
	send_query();
	return OK;
}

Variant HoneyComb::findMultipliers(Array addresses, Array stakingPools){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (stakingPools != Array()) {
		add_arg("stakingPools", "[Pubkey!]", stakingPools, true);
	}


	method_name = "multipliers";


	query_fields = "bump collectionMultipliers { multiplierType { kind params { ... on MultiplierTypeParamsStakeDuration { minDuration } ... on MultiplierTypeParamsNFTCount { minCount } ... on MultiplierTypeParamsCreator { creator } ... on MultiplierTypeParamsCollection { collection }  }  } value } countMultipliers { multiplierType { kind params { ... on MultiplierTypeParamsStakeDuration { minDuration } ... on MultiplierTypeParamsNFTCount { minCount } ... on MultiplierTypeParamsCreator { creator } ... on MultiplierTypeParamsCollection { collection }  }  } value } creatorMultipliers { multiplierType { kind params { ... on MultiplierTypeParamsStakeDuration { minDuration } ... on MultiplierTypeParamsNFTCount { minCount } ... on MultiplierTypeParamsCreator { creator } ... on MultiplierTypeParamsCollection { collection }  }  } value } decimals durationMultipliers { multiplierType { kind params { ... on MultiplierTypeParamsStakeDuration { minDuration } ... on MultiplierTypeParamsNFTCount { minCount } ... on MultiplierTypeParamsCreator { creator } ... on MultiplierTypeParamsCollection { collection }  }  } value } address stakingPool";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateStakingPoolTransaction(String project, String resource, Variant metadata, String authority, Variant multiplier, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("metadata", "CreateStakingPoolMetadataInput", Object::cast_to<godot::honeycomb_resource::CreateStakingPoolMetadataInput>(metadata)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (multiplier != Variant(nullptr)) {
		add_arg("multiplier", "InitStakingMultiplierMetadataInput", Object::cast_to<godot::honeycomb_resource::InitStakingMultiplierMetadataInput>(multiplier)->to_dict(), true);
	}
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateStakingPoolTransaction";


	query_fields = "transactions { transactions blockhash lastValidBlockHeight } stakingPoolAddress multipliersAddress";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateStakingPoolTransaction(String project, String stakingPool, String authority, Variant metadata, String characterModel, String resource, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("stakingPool", "String", Pubkey::string_from_variant(stakingPool), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (metadata != Variant(nullptr)) {
		add_arg("metadata", "UpdateStakingPoolMetadataInput", Object::cast_to<godot::honeycomb_resource::UpdateStakingPoolMetadataInput>(metadata)->to_dict(), true);
	}
	if (characterModel != "") {
		add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), true);
	}
	if (resource != "") {
		add_arg("resource", "String", Pubkey::string_from_variant(resource), true);
	}
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateStakingPoolTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createInitMultipliersTransaction(String project, String stakingPool, int32_t decimals, Array multipliers, String authority, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("stakingPool", "String", Pubkey::string_from_variant(stakingPool), false);
	add_arg("decimals", "Int", decimals, false);
	add_arg("multipliers", "[AddMultiplierMetadataInput!]", multipliers, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitMultipliersTransaction";


	query_fields = "tx { transaction blockhash lastValidBlockHeight } multipliersAddress";
	send_query();
	return OK;
}

Variant HoneyComb::createAddMultiplierTransaction(String project, String multiplier, Variant metadata, String authority, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("multiplier", "String", Pubkey::string_from_variant(multiplier), false);
	add_arg("metadata", "AddMultiplierMetadataInput", Object::cast_to<godot::honeycomb_resource::AddMultiplierMetadataInput>(metadata)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createAddMultiplierTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createStakeCharactersTransactions(PackedStringArray characterAddresses, String characterModel, String stakingPool, String project, String feePayer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("characterAddresses", "[String!]", characterAddresses, false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("stakingPool", "String", Pubkey::string_from_variant(stakingPool), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	if (feePayer != "") {
		add_arg("feePayer", "String", Pubkey::string_from_variant(feePayer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createStakeCharactersTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::createClaimStakingRewardsTransactions(PackedStringArray characterAddresses, String characterModel, String feePayer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("characterAddresses", "[String!]", characterAddresses, false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	if (feePayer != "") {
		add_arg("feePayer", "String", Pubkey::string_from_variant(feePayer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createClaimStakingRewardsTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::createUnstakeCharactersTransactions(PackedStringArray characterAddresses, String characterModel, String feePayer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("characterAddresses", "[String!]", characterAddresses, false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	if (feePayer != "") {
		add_arg("feePayer", "String", Pubkey::string_from_variant(feePayer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUnstakeCharactersTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::findMissions(Array addresses, Array missionPools){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (missionPools != Array()) {
		add_arg("missionPools", "[Pubkey!]", missionPools, true);
	}


	method_name = "mission";


	query_fields = "project address program_id discriminator bump missionPool name minXp cost { amount resource_address } requirement { ... on TimeRequirement { kind params { duration }  }  } rewards { min max rewardType { ... on XpRewardType { kind } ... on ResourceRewardType { kind params { address }  }  }  }";
	send_query();
	return OK;
}

Variant HoneyComb::findMissionPools(Array addresses, Variant project){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (project != Variant(nullptr)) {
		add_arg("project", "Pubkey", Object::cast_to<Pubkey>(project)->to_string(), true);
	}


	method_name = "missionPool";


	query_fields = "address program_id discriminator bump project name factionsMerkleRoot randomizerRound characterModels guildKits";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("data", "NewMissionPoolData", Object::cast_to<godot::honeycomb_resource::NewMissionPoolData>(data)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateMissionPoolTransaction";


	query_fields = "tx { transaction blockhash lastValidBlockHeight } missionPoolAddress";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("data", "UpdateMissionPoolData", Object::cast_to<godot::honeycomb_resource::UpdateMissionPoolData>(data)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateMissionPoolTransaction";


	query_fields = "tx { transaction blockhash lastValidBlockHeight }";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateMissionTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("data", "NewMissionData", Object::cast_to<godot::honeycomb_resource::NewMissionData>(data)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateMissionTransaction";


	query_fields = "tx { transaction blockhash lastValidBlockHeight } missionAddress";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateMissionTransaction(String missionAddress, String authority, Variant params, String payer, String delegateAuthority, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("missionAddress", "String", Pubkey::string_from_variant(missionAddress), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("params", "UpdateMissionInput", Object::cast_to<godot::honeycomb_resource::UpdateMissionInput>(params)->to_dict(), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateMissionTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createSendCharactersOnMissionTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("data", "ParticipateOnMissionData", Object::cast_to<godot::honeycomb_resource::ParticipateOnMissionData>(data)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createSendCharactersOnMissionTransaction";


	query_fields = "transactions blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createRecallCharactersTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("data", "RecallFromMissionData", Object::cast_to<godot::honeycomb_resource::RecallFromMissionData>(data)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createRecallCharactersTransaction";


	query_fields = "transactions blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::findResources(Array addresses, Array projects, Array mints){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (projects != Array()) {
		add_arg("projects", "[Pubkey!]", projects, true);
	}
	if (mints != Array()) {
		add_arg("mints", "[Pubkey!]", mints, true);
	}


	method_name = "resources";


	query_fields = "address bump kind { kind params { ... on ResourceKindParamsHplFungible { __typename decimals } ... on ResourceKindParamsWrappedFungible { __typename decimals custody { kind params { burnerDestination }  }  } ... on ResourceKindParamsHplNonFungible { __typename characteristics } ... on ResourceKindParamsWrappedMplCore { __typename characteristics }  }  } mint project tags storage { params { promiseSupply merkle_trees { active merkle_trees schema }  } kind }";
	send_query();
	return OK;
}

Variant HoneyComb::findResourcesBalance(PackedStringArray wallets, PackedStringArray addresses, PackedStringArray projects, PackedStringArray mints){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("wallets", "[String!]", wallets, false);
	if (addresses != PackedStringArray()) {
		add_arg("addresses", "[String!]", addresses, true);
	}
	if (projects != PackedStringArray()) {
		add_arg("projects", "[String!]", projects, true);
	}
	if (mints != PackedStringArray()) {
		add_arg("mints", "[String!]", mints, true);
	}


	method_name = "resourcesBalance";


	query_fields = "address amount mint wallet resourceStorage";
	send_query();
	return OK;
}

Variant HoneyComb::findFaucets(Array addresses, Array projects, Array resources){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (projects != Array()) {
		add_arg("projects", "[Pubkey!]", projects, true);
	}
	if (resources != Array()) {
		add_arg("resources", "[Pubkey!]", resources, true);
	}


	method_name = "faucets";


	query_fields = "resource repeatInterval project amount lastClaimed address";
	send_query();
	return OK;
}

Variant HoneyComb::findRecipes(Array addresses, Array projects){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (projects != Array()) {
		add_arg("projects", "[Pubkey!]", projects, true);
	}


	method_name = "recipes";


	query_fields = "address bump ingredients { amount resourceAddress } key meal { amount resourceAddress } project xp";
	send_query();
	return OK;
}

Variant HoneyComb::findHoldings(Array addresses, Array holders, Array trees, bool includeProof){
	if (pending) {
		return ERR_BUSY;
	}

	if (addresses != Array()) {
		add_arg("addresses", "[Bytes!]", addresses, true);
	}
	if (holders != Array()) {
		add_arg("holders", "[Pubkey!]", holders, true);
	}
	if (trees != Array()) {
		add_arg("trees", "[Bytes!]", trees, true);
	}
	if (includeProof != false) {
		add_arg("includeProof", "Boolean", includeProof, true);
	}


	method_name = "holdings";


	query_fields = "address balance characteristics holder leaf_idx tree_id proof { canopy_depth leaf leaf_index maxDepth node_index proof root tree_id }";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateNewResourceTransaction(String project, String authority, Variant params, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("params", "InitResourceInput", Object::cast_to<godot::honeycomb_resource::InitResourceInput>(params)->to_dict(), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateNewResourceTransaction";


	query_fields = "tx { transaction blockhash lastValidBlockHeight } resource";
	send_query();
	return OK;
}

Variant HoneyComb::createImportFungibleResourceTransaction(Variant params, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("params", "ImportResourceInput", Object::cast_to<godot::honeycomb_resource::ImportResourceInput>(params)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createImportFungibleResourceTransaction";


	query_fields = "resource tx { blockhash lastValidBlockHeight transaction }";
	send_query();
	return OK;
}

Variant HoneyComb::createExportFungibleResourceTransaction(String resource, String authority, String delegateAuthority, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createExportFungibleResourceTransaction";


	query_fields = "blockhash lastValidBlockHeight transaction";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateNewResourceTreeTransaction(String project, String resource, String authority, Variant treeConfig, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<godot::honeycomb_resource::TreeSetupConfig>(treeConfig)->to_dict(), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateNewResourceTreeTransaction";


	query_fields = "tx { blockhash lastValidBlockHeight transaction } treeAddress proofBytes space cost maxTreeCapacity";
	send_query();
	return OK;
}

Variant HoneyComb::createMintResourceTransaction(String resource, String owner, String authority, int64_t amount, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("amount", "BigInt", amount, false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createMintResourceTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createBurnResourceTransaction(String resource, int64_t amount, String authority, String owner, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "BigInt", amount, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (owner != "") {
		add_arg("owner", "String", Pubkey::string_from_variant(owner), true);
	}
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createBurnResourceTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createTransferResourceTransaction(String resource, String owner, String recipient, int64_t amount, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	add_arg("recipient", "String", Pubkey::string_from_variant(recipient), false);
	add_arg("amount", "BigInt", amount, false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createTransferResourceTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateWrapHoldingTransaction(String resource, int64_t amount, String authority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "BigInt", amount, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateWrapHoldingTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateUnwrapHoldingTransaction(String resource, int64_t amount, String authority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "BigInt", amount, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateUnwrapHoldingTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createInitializeFaucetTransaction(String resource, int32_t amount, int32_t repeatInterval, String authority, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "Int", amount, false);
	add_arg("repeatInterval", "Int", repeatInterval, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitializeFaucetTransaction";


	query_fields = "tx { transaction blockhash lastValidBlockHeight } faucet";
	send_query();
	return OK;
}

Variant HoneyComb::createClaimFaucetTransaction(String faucet, String owner, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("faucet", "String", Pubkey::string_from_variant(faucet), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createClaimFaucetTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createInitializeRecipeTransaction(String project, int64_t xp, Array ingredients, Variant meal, String authority, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("xp", "BigInt", xp, false);
	add_arg("ingredients", "[IngredientsInput!]", ingredients, false);
	add_arg("meal", "MealInput", Object::cast_to<godot::honeycomb_resource::MealInput>(meal)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitializeRecipeTransaction";


	query_fields = "recipe transactions { transactions blockhash lastValidBlockHeight }";
	send_query();
	return OK;
}

Variant HoneyComb::createAddIngredientsTransaction(String recipe, Array ingredients, String authority, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("recipe", "String", Pubkey::string_from_variant(recipe), false);
	add_arg("ingredients", "[IngredientsInput!]", ingredients, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createAddIngredientsTransaction";


	query_fields = "transactions blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createRemoveIngredientsTransaction(String recipe, PackedStringArray ingredients, String authority, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("recipe", "String", Pubkey::string_from_variant(recipe), false);
	add_arg("ingredients", "[String!]", ingredients, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (delegateAuthority != "") {
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createRemoveIngredientsTransaction";


	query_fields = "transactions blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createInitCookingProcessTransactions(String recipe, String authority, String payer, int32_t computeUnitPrice, PackedStringArray lutAddresses){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("recipe", "String", Pubkey::string_from_variant(recipe), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if (payer != "") {
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}


	method_name = "createInitCookingProcessTransactions";


	query_fields = "blockhash lastValidBlockHeight transactions";
	send_query();
	return OK;
}

Variant HoneyComb::createInitializeBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("args", "CreateBadgeCriteriaInput", Object::cast_to<godot::honeycomb_resource::CreateBadgeCriteriaInput>(args)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitializeBadgeCriteriaTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createClaimBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("args", "ClaimBadgeCriteriaInput", Object::cast_to<godot::honeycomb_resource::ClaimBadgeCriteriaInput>(args)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createClaimBadgeCriteriaTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if (pending) {
		return ERR_BUSY;
	}

	add_arg("args", "UpdateBadgeCriteriaInput", Object::cast_to<godot::honeycomb_resource::UpdateBadgeCriteriaInput>(args)->to_dict(), false);
	if (lutAddresses != PackedStringArray()) {
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if (computeUnitPrice != -1) {
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateBadgeCriteriaTransaction";


	query_fields = "transaction blockhash lastValidBlockHeight";
	send_query();
	return OK;
}

void HoneyComb::_bind_methods() {
	bind_non_changing_methods();
	ClassDB::bind_method(D_METHOD("find_compressed_accounts", "addresses", "identity", "leaves", "parsedData"), &HoneyComb::findCompressedAccounts, DEFVAL(Variant(nullptr)), DEFVAL(Array()), DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("fetch_proofs", "leaves"), &HoneyComb::fetchProofs);
	ClassDB::bind_method(D_METHOD("find_accounts", "programId", "discriminator", "parsedData"), &HoneyComb::findAccounts, DEFVAL(PackedByteArray()), DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("send_transaction_bundles", "txs", "blockhash", "lastValidBlockHeight", "options"), &HoneyComb::sendTransactionBundles, DEFVAL(Variant(nullptr)));
	ClassDB::bind_method(D_METHOD("sign_with_shadow_signer_and_send_transaction_bundles", "txs", "blockhash", "lastValidBlockHeight", "authToken", "options"), &HoneyComb::signWithShadowSignerAndSendTransactionBundles, DEFVAL(Variant(nullptr)));
	ClassDB::bind_method(D_METHOD("send_bulk_transactions", "txs", "blockhash", "lastValidBlockHeight", "options"), &HoneyComb::sendBulkTransactions, DEFVAL(Variant(nullptr)));
	ClassDB::bind_method(D_METHOD("sign_with_shadow_signer_and_send_bulk_transactions", "txs", "blockhash", "lastValidBlockHeight", "options"), &HoneyComb::signWithShadowSignerAndSendBulkTransactions, DEFVAL(Variant(nullptr)));
	ClassDB::bind_method(D_METHOD("auth_request", "wallet", "useTx", "useRpc"), &HoneyComb::authRequest, DEFVAL(false), DEFVAL(""));
	ClassDB::bind_method(D_METHOD("auth_confirm", "wallet", "signature"), &HoneyComb::authConfirm);
	ClassDB::bind_method(D_METHOD("create_transfer_assets_transactions", "addresses", "to", "lutAddresses", "computeUnitPrice"), &HoneyComb::createTransferAssetsTransactions, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_burn_assets_transactions", "addresses", "lutAddresses", "computeUnitPrice"), &HoneyComb::createBurnAssetsTransactions, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("find_global", "env"), &HoneyComb::findGlobal, DEFVAL(""));
	ClassDB::bind_method(D_METHOD("find_projects", "addresses", "names", "authorities"), &HoneyComb::findProjects, DEFVAL(Array()), DEFVAL(PackedStringArray()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_delegate_authority", "addresses", "delegates", "projects"), &HoneyComb::findDelegateAuthority, DEFVAL(Array()), DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_users", "addresses", "ids", "wallets", "includeProof", "includeProjectProfiles"), &HoneyComb::findUsers, DEFVAL(Array()), DEFVAL(PackedInt32Array()), DEFVAL(Array()), DEFVAL(false), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_profiles", "addresses", "projects", "userIds", "identities", "includeProof", "includeUsers"), &HoneyComb::findProfiles, DEFVAL(Array()), DEFVAL(Array()), DEFVAL(PackedInt32Array()), DEFVAL(PackedStringArray()), DEFVAL(false), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("create_new_user_transaction", "wallet", "info", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewUserTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_new_user_bulk_transaction", "wallet", "info", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewUserBulkTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_update_user_transaction", "payer", "info", "wallets", "populateCivic", "removeSocials", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateUserTransaction, DEFVAL(Variant(nullptr)), DEFVAL(Variant(nullptr)), DEFVAL(false), DEFVAL(Array()), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_project_transaction", "authority", "name", "driver", "associatedPrograms", "profileDataConfig", "subsidizeFees", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateProjectTransaction, DEFVAL(""), DEFVAL(Array()), DEFVAL(Variant(nullptr)), DEFVAL(false), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_change_project_driver_transaction", "project", "driver", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createChangeProjectDriverTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_delegate_authority_transaction", "project", "delegate", "serviceDelegations", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateDelegateAuthorityTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_modify_delegation_transaction", "project", "delegate", "modifyDelegation", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createModifyDelegationTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_profiles_tree_transaction", "treeConfig", "project", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateProfilesTreeTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_new_profile_transaction", "project", "payer", "identity", "info", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewProfileTransaction, DEFVAL(""), DEFVAL(Variant(nullptr)), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_update_profile_transaction", "profile", "payer", "info", "customData", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateProfileTransaction, DEFVAL(Variant(nullptr)), DEFVAL(Variant(nullptr)), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_update_platform_data_transaction", "profile", "authority", "delegateAuthority", "payer", "platformData", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdatePlatformDataTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(Variant(nullptr)), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_new_user_with_profile_transaction", "project", "wallet", "userInfo", "payer", "profileIdentity", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewUserWithProfileTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("find_assembler_config", "addresses", "tickers", "characterModel", "project"), &HoneyComb::findAssemblerConfig, DEFVAL(Array()), DEFVAL(PackedStringArray()), DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_character_traits", "addresses", "trees", "includeProof"), &HoneyComb::findCharacterTraits, DEFVAL(Array()), DEFVAL(Array()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("find_character_models", "addresses", "project", "assemblerConfigs"), &HoneyComb::findCharacterModels, DEFVAL(Array()), DEFVAL(Variant(nullptr)), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_characters", "includeProof", "addresses", "filters", "trees", "wallets", "mints", "attributeHashes"), &HoneyComb::findCharacters, DEFVAL(false), DEFVAL(Array()), DEFVAL(Variant(nullptr)), DEFVAL(Array()), DEFVAL(Array()), DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("fetch_character_history", "addresses", "event"), &HoneyComb::fetchCharacterHistory, DEFVAL(PackedStringArray()));
	ClassDB::bind_method(D_METHOD("create_create_assembler_config_transaction", "ticker", "project", "authority", "treeConfig", "order", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateAssemblerConfigTransaction, DEFVAL(Variant(nullptr)), DEFVAL(PackedStringArray()), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_add_character_traits_transactions", "assemblerConfig", "traits", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createAddCharacterTraitsTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_remove_character_traits_transactions", "assemblerConfig", "traitsAddresses", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createRemoveCharacterTraitsTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_character_model_transaction", "config", "project", "authority", "attributes", "mintAs", "cooldown", "equipableCriteria", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateCharacterModelTransaction, DEFVAL(Array()), DEFVAL(Variant(nullptr)), DEFVAL(Variant(nullptr)), DEFVAL(PackedStringArray()), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_characters_tree_transaction", "treeConfig", "project", "characterModel", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateCharactersTreeTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_assemble_character_transaction", "project", "assemblerConfig", "characterModel", "owner", "authority", "uri", "attributes", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createAssembleCharacterTransaction, DEFVAL(""), DEFVAL(Array()), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_update_character_traits_transaction", "characterAddress", "project", "assemblerConfig", "characterModel", "authority", "uri", "attributes", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateCharacterTraitsTransaction, DEFVAL(""), DEFVAL(Array()), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_populate_character_transaction", "project", "characterModel", "mint", "owner", "updateAuthority", "payer", "attributes", "assemblerConfig", "lutAddresses", "computeUnitPrice"), &HoneyComb::createPopulateCharacterTransaction, DEFVAL(Array()), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_wrap_assets_to_character_transactions", "project", "characterModel", "wallet", "mintList", "libreplexDeployment", "lutAddresses", "computeUnitPrice"), &HoneyComb::createWrapAssetsToCharacterTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_unwrap_assets_from_character_transactions", "project", "characterAddresses", "characterModel", "wallet", "libreplexDeployment", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUnwrapAssetsFromCharacterTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_equip_resource_on_character_transaction", "characterModel", "characterAddress", "resource", "amount", "owner", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createEquipResourceOnCharacterTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_dismount_resource_on_character_transaction", "characterModel", "characterAddress", "resource", "amount", "owner", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createDismountResourceOnCharacterTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_use_character_transaction", "character", "project", "characterModel", "user", "data", "unUse", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUseCharacterTransaction, DEFVAL(Dictionary()), DEFVAL(false), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("find_staking_pools", "addresses", "projects", "names"), &HoneyComb::findStakingPools, DEFVAL(Array()), DEFVAL(Array()), DEFVAL(PackedStringArray()));
	ClassDB::bind_method(D_METHOD("find_stakers", "addresses", "stakingPools", "wallets"), &HoneyComb::findStakers, DEFVAL(Array()), DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_multipliers", "addresses", "stakingPools"), &HoneyComb::findMultipliers, DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("create_create_staking_pool_transaction", "project", "resource", "metadata", "authority", "multiplier", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateStakingPoolTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_update_staking_pool_transaction", "project", "stakingPool", "authority", "metadata", "characterModel", "resource", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateStakingPoolTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(""), DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_init_multipliers_transaction", "project", "stakingPool", "decimals", "multipliers", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitMultipliersTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_add_multiplier_transaction", "project", "multiplier", "metadata", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createAddMultiplierTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_stake_characters_transactions", "characterAddresses", "characterModel", "stakingPool", "project", "feePayer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createStakeCharactersTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_claim_staking_rewards_transactions", "characterAddresses", "characterModel", "feePayer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createClaimStakingRewardsTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_unstake_characters_transactions", "characterAddresses", "characterModel", "feePayer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUnstakeCharactersTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("find_missions", "addresses", "missionPools"), &HoneyComb::findMissions, DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_mission_pools", "addresses", "project"), &HoneyComb::findMissionPools, DEFVAL(Array()), DEFVAL(Variant(nullptr)));
	ClassDB::bind_method(D_METHOD("create_create_mission_pool_transaction", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateMissionPoolTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_update_mission_pool_transaction", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateMissionPoolTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_mission_transaction", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateMissionTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_update_mission_transaction", "missionAddress", "authority", "params", "payer", "delegateAuthority", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateMissionTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_send_characters_on_mission_transaction", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createSendCharactersOnMissionTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_recall_characters_transaction", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createRecallCharactersTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("find_resources", "addresses", "projects", "mints"), &HoneyComb::findResources, DEFVAL(Array()), DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_resources_balance", "wallets", "addresses", "projects", "mints"), &HoneyComb::findResourcesBalance, DEFVAL(PackedStringArray()), DEFVAL(PackedStringArray()), DEFVAL(PackedStringArray()));
	ClassDB::bind_method(D_METHOD("find_faucets", "addresses", "projects", "resources"), &HoneyComb::findFaucets, DEFVAL(Array()), DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_recipes", "addresses", "projects"), &HoneyComb::findRecipes, DEFVAL(Array()), DEFVAL(Array()));
	ClassDB::bind_method(D_METHOD("find_holdings", "addresses", "holders", "trees", "includeProof"), &HoneyComb::findHoldings, DEFVAL(Array()), DEFVAL(Array()), DEFVAL(Array()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("create_create_new_resource_transaction", "project", "authority", "params", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateNewResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_import_fungible_resource_transaction", "params", "lutAddresses", "computeUnitPrice"), &HoneyComb::createImportFungibleResourceTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_export_fungible_resource_transaction", "resource", "authority", "delegateAuthority", "lutAddresses", "computeUnitPrice"), &HoneyComb::createExportFungibleResourceTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_new_resource_tree_transaction", "project", "resource", "authority", "treeConfig", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateNewResourceTreeTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_mint_resource_transaction", "resource", "owner", "authority", "amount", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createMintResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_burn_resource_transaction", "resource", "amount", "authority", "owner", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createBurnResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_transfer_resource_transaction", "resource", "owner", "recipient", "amount", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createTransferResourceTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_wrap_holding_transaction", "resource", "amount", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateWrapHoldingTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_create_unwrap_holding_transaction", "resource", "amount", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateUnwrapHoldingTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_initialize_faucet_transaction", "resource", "amount", "repeatInterval", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitializeFaucetTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_claim_faucet_transaction", "faucet", "owner", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createClaimFaucetTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_initialize_recipe_transaction", "project", "xp", "ingredients", "meal", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitializeRecipeTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_add_ingredients_transaction", "recipe", "ingredients", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createAddIngredientsTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_remove_ingredients_transaction", "recipe", "ingredients", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createRemoveIngredientsTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_init_cooking_process_transactions", "recipe", "authority", "payer", "computeUnitPrice", "lutAddresses"), &HoneyComb::createInitCookingProcessTransactions, DEFVAL(""), DEFVAL(-1), DEFVAL(PackedStringArray()));
	ClassDB::bind_method(D_METHOD("create_initialize_badge_criteria_transaction", "args", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitializeBadgeCriteriaTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_claim_badge_criteria_transaction", "args", "lutAddresses", "computeUnitPrice"), &HoneyComb::createClaimBadgeCriteriaTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_update_badge_criteria_transaction", "args", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateBadgeCriteriaTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
}
} // godot