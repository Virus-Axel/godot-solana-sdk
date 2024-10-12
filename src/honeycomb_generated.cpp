#include "honeycomb.hpp"

namespace godot{

Variant HoneyComb::createCreateNewResourceTransaction(const Variant& project, const Variant& authority, Variant params, String delegateAuthority, String payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(authority);
	signers.append(Pubkey::new_from_string("11111111111111111111111111111111"));

	add_arg("project", "String", Pubkey::string_from_variant(project), false);
	add_arg("authority", "String", Pubkey::string_from_variant(authority), false);
	add_arg("params", "InitResourceInput", Object::cast_to<InitResourceInput>(params)->to_dict(), false);
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
	add_arg("treeConfig", "TreeSetupConfig", Object::cast_to<TreeSetupConfig>(treeConfig)->to_dict(), false);
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

Variant HoneyComb::createNewUserTransaction(const Variant& wallet, Variant info, const Variant& payer, PackedStringArray lutAddresses, int32_t computeUnitPrice){
	if(pending){
		return ERR_BUSY;
	}
	signers.append(wallet);
	signers.append(payer);

	add_arg("wallet", "String", Pubkey::string_from_variant(wallet), false);
	if(info != Variant(nullptr)){
		add_arg("info", "UserInfoInput", Object::cast_to<UserInfoInput>(info)->to_dict(), true);
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

void HoneyComb::_bind_methods(){
	bind_non_changing_methods();
	ClassDB::bind_method(D_METHOD("create_new_resource", "project", "authority", "params", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateNewResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("create_new_resource_tree", "project", "resource", "authority", "treeConfig", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createCreateNewResourceTreeTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("mint_resource", "resource", "owner", "authority", "amount", "delegateAuthority", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createMintResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("burn_resource", "resource", "amount", "authority", "owner", "payer", "delegateAuthority", "lutAddresses", "computeUnitPrice"), &HoneyComb::createBurnResourceTransaction, DEFVAL(""), DEFVAL(""), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("new_user", "wallet", "info", "payer", "lutAddresses", "computeUnitPrice"), &HoneyComb::createNewUserTransaction, DEFVAL(Variant(nullptr)), DEFVAL(""), DEFVAL(PackedStringArray()), DEFVAL(-1));
}
} // godot
