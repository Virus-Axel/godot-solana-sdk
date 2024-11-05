#include "honeycomb/honeycomb.hpp"

#include "honeycomb/types/Transaction.hpp"
#include "honeycomb/types/SendTransactionBundlesOptions.hpp"
#include "honeycomb/types/CreateBadgeCriteriaInput.hpp"
#include "honeycomb/types/ModifyDelegationInput.hpp"
#include "honeycomb/types/ServiceDelegationInput.hpp"
#include "honeycomb/types/ProfileDataConfigInput.hpp"
#include "honeycomb/types/UpdateBadgeCriteriaInput.hpp"
#include "honeycomb/types/ClaimBadgeCriteriaInput.hpp"
#include "honeycomb/types/ProfileInfoInput.hpp"
#include "honeycomb/types/CustomDataInput.hpp"
#include "honeycomb/types/UpdateWalletInput.hpp"
#include "honeycomb/types/PartialUserInfoInput.hpp"
namespace godot{

Variant HoneyComb::createCreateNewResourceTransaction(const Variant& project, const Variant& authority, Variant params, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(Pubkey::new_from_string("11111111111111111111111111111111"));

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("params", "InitResourceInput", Object::cast_to<godot::honeycomb_resource::InitResourceInput>(params)->to_dict(), false);
	if(delegateAuthority != ""){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != ""){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateNewResourceTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } resource ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateNewResourceTreeTransaction(const Variant& project, const Variant& resource, const Variant& authority, Variant treeConfig, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<godot::honeycomb_resource::TreeSetupConfig>(treeConfig)->to_dict(), false);
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateNewResourceTreeTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } treeAddress proofBytes space cost maxTreeCapacity ";
	send_query();
	return OK;
}

Variant HoneyComb::createMintResourceTransaction(const Variant& resource, const Variant& owner, const Variant& authority, int64_t amount, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(owner);
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("amount", "BigInt", amount, false);
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createMintResourceTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createBurnResourceTransaction(const Variant& resource, int64_t amount, const Variant& authority, const Variant& owner, const Variant& payer, const Variant& delegateAuthority, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(owner);
	signers.append(payer);
	signers.append(delegateAuthority);

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "BigInt", amount, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(owner != Variant(nullptr)){
		add_arg("owner", "String", Pubkey::string_from_variant(owner), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createBurnResourceTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateUnwrapHoldingTransaction(String resource, int64_t amount, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "BigInt", amount, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateUnwrapHoldingTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateWrapHoldingTransaction(String resource, int64_t amount, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "BigInt", amount, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateWrapHoldingTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createTransferResourceTransaction(String resource, const Variant& owner, String recipient, int64_t amount, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(owner);
	signers.append(payer);

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	add_arg("recipient", "String", Pubkey::string_from_variant(recipient), false);
	add_arg("amount", "BigInt", amount, false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createTransferResourceTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createInitializeRecipeTransaction(const Variant& project, int64_t xp, Array ingredients, Variant meal, const Variant& authority, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("xp", "BigInt", xp, false);
	add_arg("ingredients", "[IngredientsInput!]", ingredients, false);
	add_arg("meal", "MealInput", Object::cast_to<godot::honeycomb_resource::MealInput>(meal)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitializeRecipeTransaction";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createAddIngredientsTransaction(const Variant& recipe, Array ingredients, const Variant& authority, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("recipe", "String", Pubkey::string_from_variant(recipe), false);
	add_arg("ingredients", "[IngredientsInput!]", ingredients, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createAddIngredientsTransaction";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createRemoveIngredientsTransaction(const Variant& recipe, PackedStringArray ingredients, const Variant& authority, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("recipe", "String", Pubkey::string_from_variant(recipe), false);
	add_arg("ingredients", "[String!]", ingredients, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createRemoveIngredientsTransaction";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createInitCookingProcessTransactions(const Variant& recipe, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("recipe", "String", Pubkey::string_from_variant(recipe), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitCookingProcessTransactions";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("data", "NewMissionPoolData", Object::cast_to<godot::honeycomb_resource::NewMissionPoolData>(data)->to_dict(), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateMissionPoolTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } missionPoolAddress ";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("data", "UpdateMissionPoolData", Object::cast_to<godot::honeycomb_resource::UpdateMissionPoolData>(data)->to_dict(), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateMissionPoolTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateMissionTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("data", "NewMissionData", Object::cast_to<godot::honeycomb_resource::NewMissionData>(data)->to_dict(), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateMissionTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } missionAddress ";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateMissionTransaction(const Variant& missionAddress, const Variant& authority, Variant params, const Variant& payer, const Variant& delegateAuthority, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);
	signers.append(delegateAuthority);

	add_arg("missionAddress", "String", Pubkey::string_from_variant(missionAddress), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("params", "UpdateMissionInput", Object::cast_to<godot::honeycomb_resource::UpdateMissionInput>(params)->to_dict(), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateMissionTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createSendCharactersOnMissionTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("data", "ParticipateOnMissionData", Object::cast_to<godot::honeycomb_resource::ParticipateOnMissionData>(data)->to_dict(), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createSendCharactersOnMissionTransaction";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateStakingPoolTransaction(const Variant& project, const Variant& resource, Variant metadata, const Variant& authority, Variant multiplier, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("metadata", "CreateStakingPoolMetadataInput", Object::cast_to<godot::honeycomb_resource::CreateStakingPoolMetadataInput>(metadata)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(multiplier != Variant(nullptr)){
		add_arg("multiplier", "InitStakingMultiplierMetadataInput", Object::cast_to<godot::honeycomb_resource::InitStakingMultiplierMetadataInput>(multiplier)->to_dict(), true);
	}
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateStakingPoolTransaction";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateStakingPoolTransaction(const Variant& project, const Variant& stakingPool, const Variant& authority, Variant metadata, const Variant& characterModel, const Variant& resource, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("stakingPool", "String", Pubkey::string_from_variant(stakingPool), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(metadata != Variant(nullptr)){
		add_arg("metadata", "UpdateStakingPoolMetadataInput", Object::cast_to<godot::honeycomb_resource::UpdateStakingPoolMetadataInput>(metadata)->to_dict(), true);
	}
	if(characterModel != Variant(nullptr)){
		add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), true);
	}
	if(resource != Variant(nullptr)){
		add_arg("resource", "String", Pubkey::string_from_variant(resource), true);
	}
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateStakingPoolTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createInitMultipliersTransaction(const Variant& project, const Variant& stakingPool, int32_t decimals, Array multipliers, const Variant& authority, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("stakingPool", "String", Pubkey::string_from_variant(stakingPool), false);
	add_arg("decimals", "Int", decimals, false);
	add_arg("multipliers", "[AddMultiplierMetadataInput]", multipliers, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitMultipliersTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } multipliersAddress ";
	send_query();
	return OK;
}

Variant HoneyComb::createAddMultiplierTransaction(const Variant& project, const Variant& multiplier, Variant metadata, const Variant& authority, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("multiplier", "String", Pubkey::string_from_variant(multiplier), false);
	add_arg("metadata", "AddMultiplierMetadataInput", Object::cast_to<godot::honeycomb_resource::AddMultiplierMetadataInput>(metadata)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createAddMultiplierTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createStakeCharactersTransactions(PackedStringArray characterAddresses, const Variant& project, const Variant& characterModel, const Variant& stakingPool, const Variant& feePayer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(feePayer);

	add_arg("characterAddresses", "[String!]", characterAddresses, false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("stakingPool", "String", Pubkey::string_from_variant(stakingPool), false);
	if(feePayer != Variant(nullptr)){
		add_arg("feePayer", "String", Pubkey::string_from_variant(feePayer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createStakeCharactersTransactions";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createClaimStakingRewardsTransactions(PackedStringArray characterAddresses, const Variant& characterModel, const Variant& feePayer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(feePayer);

	add_arg("characterAddresses", "[String!]", characterAddresses, false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	if(feePayer != Variant(nullptr)){
		add_arg("feePayer", "String", Pubkey::string_from_variant(feePayer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createClaimStakingRewardsTransactions";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createUnstakeCharactersTransactions(PackedStringArray characterAddresses, const Variant& characterModel, const Variant& feePayer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(feePayer);

	add_arg("characterAddresses", "[String!]", characterAddresses, false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	if(feePayer != Variant(nullptr)){
		add_arg("feePayer", "String", Pubkey::string_from_variant(feePayer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUnstakeCharactersTransactions";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createAddCharacterTraitsTransactions(const Variant& assemblerConfig, Array traits, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), false);
	add_arg("traits", "[CharacterTraitInput!]", traits, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createAddCharacterTraitsTransactions";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createRemoveCharacterTraitsTransactions(const Variant& assemblerConfig, PackedStringArray traitsAddresses, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), false);
	add_arg("traitsAddresses", "[String!]", traitsAddresses, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createRemoveCharacterTraitsTransactions";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateCharacterModelTransaction(Variant config, const Variant& project, const Variant& authority, Variant attributes, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("config", "CharacterConfigInput", Object::cast_to<godot::honeycomb_resource::CharacterConfigInput>(config)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(attributes != Variant(nullptr)){
		add_arg("attributes", "VecMapGeneric", Object::cast_to<godot::honeycomb_resource::VecMapGeneric>(attributes)->to_dict(), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateCharacterModelTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } characterModel ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateCharactersTreeTransaction(Variant treeConfig, const Variant& project, const Variant& characterModel, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<godot::honeycomb_resource::TreeSetupConfig>(treeConfig)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateCharactersTreeTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } treeAddress proofBytes space cost maxTreeCapacity ";
	send_query();
	return OK;
}

Variant HoneyComb::createAssembleCharacterTransaction(Variant attributes, const Variant& project, const Variant& assemblerConfig, const Variant& characterModel, const Variant& charactersTree, const Variant& wallet, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(wallet);

	add_arg("attributes", "VecMapGeneric", Object::cast_to<godot::honeycomb_resource::VecMapGeneric>(attributes)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("charactersTree", "String", Pubkey::string_from_variant(charactersTree), false);
	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createAssembleCharacterTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateCharacterTraitsTransaction(const Variant& characterAddress, Variant attributes, const Variant& project, const Variant& assemblerConfig, const Variant& characterModel, const Variant& charactersTree, const Variant& wallet, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(wallet);

	add_arg("characterAddress", "String", Pubkey::string_from_variant(characterAddress), false);
	add_arg("attributes", "VecMapGeneric", Object::cast_to<godot::honeycomb_resource::VecMapGeneric>(attributes)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("assemblerConfig", "String", Pubkey::string_from_variant(assemblerConfig), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("charactersTree", "String", Pubkey::string_from_variant(charactersTree), false);
	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateCharacterTraitsTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createPopulateAssembleablCharacterTransaction(Variant attributes, const Variant& project, const Variant& characterModel, const Variant& charactersTree, const Variant& mint, const Variant& owner, const Variant& updateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(owner);
	signers.append(updateAuthority);
	signers.append(payer);

	add_arg("attributes", "VecMapGeneric", Object::cast_to<godot::honeycomb_resource::VecMapGeneric>(attributes)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("charactersTree", "String", Pubkey::string_from_variant(charactersTree), false);
	add_arg("mint", "String", Pubkey::string_from_variant(mint), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	add_arg("updateAuthority", "String", Pubkey::string_from_variant(updateAuthority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createPopulateAssembleablCharacterTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createWrapAssetsToCharacterTransactions(PackedStringArray mintList, const Variant& project, const Variant& characterModel, const Variant& wallet, String libreplexDeployment, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(wallet);

	add_arg("mintList", "[String!]", mintList, false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if(libreplexDeployment != ""){
		add_arg("libreplexDeployment", "String", Pubkey::string_from_variant(libreplexDeployment), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createWrapAssetsToCharacterTransactions";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createUnwrapAssetsFromCharacterTransactions(PackedStringArray characterAddresses, const Variant& project, String characterModel, const Variant& wallet, String libreplexDeployment, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(wallet);

	add_arg("characterAddresses", "[String!]", characterAddresses, false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("characterModel", "String", Pubkey::string_from_variant(characterModel), false);
	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if(libreplexDeployment != ""){
		add_arg("libreplexDeployment", "String", Pubkey::string_from_variant(libreplexDeployment), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUnwrapAssetsFromCharacterTransactions";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createRecallCharactersTransaction(Variant data, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("data", "RecallFromMissionData", Object::cast_to<godot::honeycomb_resource::RecallFromMissionData>(data)->to_dict(), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createRecallCharactersTransaction";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::signWithShadowSignerAndSendTransactionBundles(Array txs, String blockhash, int32_t lastValidBlockHeight, String authToken, Variant options){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("txs", "[Bytes!]", txs, false);
	add_arg("blockhash", "String", Pubkey::string_from_variant(blockhash), false);
	add_arg("lastValidBlockHeight", "Int", lastValidBlockHeight, false);
	add_arg("authToken", "String", Pubkey::string_from_variant(authToken), false);
	if(options != Variant(nullptr)){
		add_arg("options", "SendTransactionBundlesOptions", Object::cast_to<godot::honeycomb_resource::SendTransactionBundlesOptions>(options)->to_dict(), true);
	}


	method_name = "signWithShadowSignerAndSendTransactionBundles";


	query_fields = " signature error status ";
	send_query();
	return OK;
}

Variant HoneyComb::createInitializeFaucetTransaction(const Variant& resource, int32_t amount, int32_t repeatInterval, const Variant& authority, const Variant& delegateAuthority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(delegateAuthority);
	signers.append(payer);

	add_arg("resource", "String", Pubkey::string_from_variant(resource), false);
	add_arg("amount", "Int", amount, false);
	add_arg("repeatInterval", "Int", repeatInterval, false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(delegateAuthority != Variant(nullptr)){
		add_arg("delegateAuthority", "String", Pubkey::string_from_variant(delegateAuthority), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitializeFaucetTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } faucet ";
	send_query();
	return OK;
}

Variant HoneyComb::createClaimFaucetTransaction(const Variant& faucet, const Variant& owner, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(owner);
	signers.append(payer);

	add_arg("faucet", "String", Pubkey::string_from_variant(faucet), false);
	add_arg("owner", "String", Pubkey::string_from_variant(owner), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createClaimFaucetTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createNewUserTransaction(const Variant& wallet, Variant info, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(wallet);
	signers.append(payer);

	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if(info != Variant(nullptr)){
		add_arg("info", "UserInfoInput", Object::cast_to<godot::honeycomb_resource::UserInfoInput>(info)->to_dict(), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createNewUserTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createNewUserBulkTransaction(Array info, PackedStringArray wallet, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(payer);

	add_arg("info", "[UserInfoInput!]", info, false);
	add_arg("wallet", "[String!]", wallet, false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createNewUserBulkTransaction";


	query_fields = " transactions blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateUserTransaction(const Variant& payer, Variant info, Variant wallets, bool populateCivic, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(payer);

	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if(info != Variant(nullptr)){
		add_arg("info", "PartialUserInfoInput", Object::cast_to<godot::honeycomb_resource::PartialUserInfoInput>(info)->to_dict(), true);
	}
	if(wallets != Variant(nullptr)){
		add_arg("wallets", "UpdateWalletInput", Object::cast_to<godot::honeycomb_resource::UpdateWalletInput>(wallets)->to_dict(), true);
	}
	if(populateCivic != false){
		add_arg("populateCivic", "Boolean", populateCivic, true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateUserTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateProfilesTreeTransaction(Variant treeConfig, const Variant& project, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(payer);

	add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<godot::honeycomb_resource::TreeSetupConfig>(treeConfig)->to_dict(), false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateProfilesTreeTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } treeAddress proofBytes space cost maxTreeCapacity ";
	send_query();
	return OK;
}

Variant HoneyComb::createNewProfileTransaction(const Variant& project, const Variant& payer, String identity, Variant info, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if(identity != ""){
		add_arg("identity", "String", Pubkey::string_from_variant(identity), true);
	}
	if(info != Variant(nullptr)){
		add_arg("info", "ProfileInfoInput", Object::cast_to<godot::honeycomb_resource::ProfileInfoInput>(info)->to_dict(), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createNewProfileTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateProfileTransaction(const Variant& profile, const Variant& payer, Variant info, Variant customData, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(profile);
	signers.append(payer);

	add_arg("profile", "String", Pubkey::string_from_variant(profile), false);
	add_arg("payer", "String", Pubkey::string_from_variant(payer), false);
	if(info != Variant(nullptr)){
		add_arg("info", "ProfileInfoInput", Object::cast_to<godot::honeycomb_resource::ProfileInfoInput>(info)->to_dict(), true);
	}
	if(customData != Variant(nullptr)){
		add_arg("customData", "CustomDataInput", Object::cast_to<godot::honeycomb_resource::CustomDataInput>(customData)->to_dict(), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateProfileTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createNewUserWithProfileTransaction(const Variant& project, const Variant& wallet, Variant userInfo, String profileIdentity, Variant profileInfo, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(wallet);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if(userInfo != Variant(nullptr)){
		add_arg("userInfo", "UserInfoInput", Object::cast_to<godot::honeycomb_resource::UserInfoInput>(userInfo)->to_dict(), true);
	}
	if(profileIdentity != ""){
		add_arg("profileIdentity", "String", Pubkey::string_from_variant(profileIdentity), true);
	}
	if(profileInfo != Variant(nullptr)){
		add_arg("profileInfo", "ProfileInfoInput", Object::cast_to<godot::honeycomb_resource::ProfileInfoInput>(profileInfo)->to_dict(), true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createNewUserWithProfileTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createClaimBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("args", "ClaimBadgeCriteriaInput", Object::cast_to<godot::honeycomb_resource::ClaimBadgeCriteriaInput>(args)->to_dict(), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createClaimBadgeCriteriaTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createUpdateBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("args", "UpdateBadgeCriteriaInput", Object::cast_to<godot::honeycomb_resource::UpdateBadgeCriteriaInput>(args)->to_dict(), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createUpdateBadgeCriteriaTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateProjectTransaction(const Variant& authority, String name, const Variant& driver, Array associatedPrograms, Variant profileDataConfig, bool subsidizeFees, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("name", "String", Pubkey::string_from_variant(name), false);
	if(driver != Variant(nullptr)){
		add_arg("driver", "String", Pubkey::string_from_variant(driver), true);
	}
	if(associatedPrograms != Array()){
		add_arg("associatedPrograms", "[AssociatedProgramInput!]", associatedPrograms, true);
	}
	if(profileDataConfig != Variant(nullptr)){
		add_arg("profileDataConfig", "ProfileDataConfigInput", Object::cast_to<godot::honeycomb_resource::ProfileDataConfigInput>(profileDataConfig)->to_dict(), true);
	}
	if(subsidizeFees != false){
		add_arg("subsidizeFees", "Boolean", subsidizeFees, true);
	}
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateProjectTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } project ";
	send_query();
	return OK;
}

Variant HoneyComb::createChangeProjectDriverTransaction(const Variant& project, const Variant& driver, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("driver", "String", Pubkey::string_from_variant(driver), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createChangeProjectDriverTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateDelegateAuthorityTransaction(const Variant& project, const Variant& delegate, Variant serviceDelegations, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("delegate", "String", Pubkey::string_from_variant(delegate), false);
	add_arg("serviceDelegations", "ServiceDelegationInput", Object::cast_to<godot::honeycomb_resource::ServiceDelegationInput>(serviceDelegations)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateDelegateAuthorityTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createModifyDelegationTransaction(const Variant& project, const Variant& delegate, Variant modifyDelegation, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("delegate", "String", Pubkey::string_from_variant(delegate), false);
	add_arg("modifyDelegation", "ModifyDelegationInput", Object::cast_to<godot::honeycomb_resource::ModifyDelegationInput>(modifyDelegation)->to_dict(), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createModifyDelegationTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

Variant HoneyComb::createCreateAssemblerConfigTransaction(Variant treeConfig, String ticker, PackedStringArray order, const Variant& project, const Variant& authority, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(payer);

	add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<godot::honeycomb_resource::TreeSetupConfig>(treeConfig)->to_dict(), false);
	add_arg("ticker", "String", Pubkey::string_from_variant(ticker), false);
	add_arg("order", "[String!]", order, false);
	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	if(payer != Variant(nullptr)){
		add_arg("payer", "String", Pubkey::string_from_variant(payer), true);
	}
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createCreateAssemblerConfigTransaction";


	query_fields = " tx { transaction blockhash lastValidBlockHeight } assemblerConfig treeAddress proofBytes space cost maxTreeCapacity ";
	send_query();
	return OK;
}

Variant HoneyComb::createInitializeBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}

	add_arg("args", "CreateBadgeCriteriaInput", Object::cast_to<godot::honeycomb_resource::CreateBadgeCriteriaInput>(args)->to_dict(), false);
	if(lutAddresses != PackedStringArray()){
		add_arg("lutAddresses", "[String!]", lutAddresses, true);
	}
	if(computeUnitPrice != -1){
		add_arg("computeUnitPrice", "Int", computeUnitPrice, true);
	}


	method_name = "createInitializeBadgeCriteriaTransaction";


	query_fields = " transaction blockhash lastValidBlockHeight ";
	send_query();
	return OK;
}

void HoneyComb::_bind_methods(){
	bind_non_changing_methods();
	ClassDB::bind_method(D_METHOD("create_new_resource", "project", "authority", "params", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateNewResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_new_resource_tree", "project", "resource", "authority", "treeConfig", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateNewResourceTreeTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("mint_resource", "resource", "owner", "authority", "amount", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createMintResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("burn_resource", "resource", "amount", "authority", "owner", "payer", "delegateAuthority", "lutAddresses", "computeUnitPrice"), &HoneyComb::createBurnResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_unwrap_holding", "resource", "amount", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateUnwrapHoldingTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_wrap_holding", "resource", "amount", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateWrapHoldingTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("transfer_resource", "resource", "owner", "recipient", "amount", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createTransferResourceTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("initialize_recipe", "project", "xp", "ingredients", "meal", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitializeRecipeTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("add_ingredients", "recipe", "ingredients", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createAddIngredientsTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("remove_ingredients", "recipe", "ingredients", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createRemoveIngredientsTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("init_cooking_process_t", "recipe", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitCookingProcessTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_mission_pool", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateMissionPoolTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("update_mission_pool", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateMissionPoolTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_mission", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateMissionTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("update_mission", "missionAddress", "authority", "params", "payer", "delegateAuthority", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateMissionTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("send_characters_on_mission", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createSendCharactersOnMissionTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_staking_pool", "project", "resource", "metadata", "authority", "multiplier", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateStakingPoolTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("update_staking_pool", "project", "stakingPool", "authority", "metadata", "characterModel", "resource", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateStakingPoolTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(""), DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("init_multipliers", "project", "stakingPool", "decimals", "multipliers", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitMultipliersTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("add_multiplier", "project", "multiplier", "metadata", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createAddMultiplierTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("stake_characters_t", "characterAddresses", "project", "characterModel", "stakingPool", "feePayer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createStakeCharactersTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("claim_staking_rewards_t", "characterAddresses", "characterModel", "feePayer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createClaimStakingRewardsTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("unstake_characters_t", "characterAddresses", "characterModel", "feePayer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUnstakeCharactersTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("add_character_traits_t", "assemblerConfig", "traits", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createAddCharacterTraitsTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("remove_character_traits_t", "assemblerConfig", "traitsAddresses", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createRemoveCharacterTraitsTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_character_model", "config", "project", "authority", "attributes", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateCharacterModelTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_characters_tree", "treeConfig", "project", "characterModel", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateCharactersTreeTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("assemble_character", "attributes", "project", "assemblerConfig", "characterModel", "charactersTree", "wallet", "lutAddresses", "computeUnitPrice"), &HoneyComb::createAssembleCharacterTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("update_character_traits", "characterAddress", "attributes", "project", "assemblerConfig", "characterModel", "charactersTree", "wallet", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateCharacterTraitsTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("populate_assembleabl_character", "attributes", "project", "characterModel", "charactersTree", "mint", "owner", "updateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createPopulateAssembleablCharacterTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("wrap_assets_to_character_t", "mintList", "project", "characterModel", "wallet", "libreplexDeployment", "lutAddresses", "computeUnitPrice"), &HoneyComb::createWrapAssetsToCharacterTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("unwrap_assets_from_character_t", "characterAddresses", "project", "characterModel", "wallet", "libreplexDeployment", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUnwrapAssetsFromCharacterTransactions, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("recall_characters", "data", "lutAddresses", "computeUnitPrice"), &HoneyComb::createRecallCharactersTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("th_shadow_signer_and_send_transac", "txs", "blockhash", "lastValidBlockHeight", "authToken", "options"), &HoneyComb::signWithShadowSignerAndSendTransactionBundles, DEFVAL(Variant(nullptr)));
	ClassDB::bind_method(D_METHOD("initialize_faucet", "resource", "amount", "repeatInterval", "authority", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitializeFaucetTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("claim_faucet", "faucet", "owner", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createClaimFaucetTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("new_user", "wallet", "info", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewUserTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("new_user_bulk", "info", "wallet", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewUserBulkTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("update_user", "payer", "info", "wallets", "populateCivic", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateUserTransaction, DEFVAL(Variant(nullptr)), DEFVAL(Variant(nullptr)), DEFVAL(false), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_profiles_tree", "treeConfig", "project", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateProfilesTreeTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("new_profile", "project", "payer", "identity", "info", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewProfileTransaction, DEFVAL(""), DEFVAL(Variant(nullptr)), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("update_profile", "profile", "payer", "info", "customData", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateProfileTransaction, DEFVAL(Variant(nullptr)), DEFVAL(Variant(nullptr)), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("new_user_with_profile", "project", "wallet", "userInfo", "profileIdentity", "profileInfo", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewUserWithProfileTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("claim_badge_criteria", "args", "lutAddresses", "computeUnitPrice"), &HoneyComb::createClaimBadgeCriteriaTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("update_badge_criteria", "args", "lutAddresses", "computeUnitPrice"), &HoneyComb::createUpdateBadgeCriteriaTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_project", "authority", "name", "driver", "associatedPrograms", "profileDataConfig", "subsidizeFees", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateProjectTransaction, DEFVAL(""), DEFVAL(Array()), DEFVAL(Variant(nullptr)), DEFVAL(false), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("change_project_driver", "project", "driver", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createChangeProjectDriverTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_delegate_authority", "project", "delegate", "serviceDelegations", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateDelegateAuthorityTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("modify_delegation", "project", "delegate", "modifyDelegation", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createModifyDelegationTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_assembler_config", "treeConfig", "ticker", "order", "project", "authority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateAssemblerConfigTransaction, DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("initialize_badge_criteria", "args", "lutAddresses", "computeUnitPrice"), &HoneyComb::createInitializeBadgeCriteriaTransaction, DEFVAL(PackedStringArray()), DEFVAL(-1));
}
} // godot