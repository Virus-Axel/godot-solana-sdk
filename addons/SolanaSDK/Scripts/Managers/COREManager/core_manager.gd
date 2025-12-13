extends Node
class_name CoreManager

@onready var collection_manager:CoreCollectionManager = $CoreCollectionManager
@onready var asset_manager:CoreAssetManager = $CoreAssetManager
@onready var authority_signer:OracleSigner = $AuthoritySigner

enum CoreManageState{Add,Edit,Remove}

func get_mpl_core_instance() -> mpl_core_program:
	var instance:mpl_core_program = mpl_core_program.new()
	add_child(instance)
	return instance

#	it's either a keypair if payer in app is authority, or pubkey if its gonna be a an authority signing from the server later
func get_authority_signer():
	var authority_key:Pubkey = authority_signer.get_signer_key()
	var signer
	
	if authority_key == null:
		signer = SolanaService.wallet.get_kp()
	else:
		signer = authority_key
		
	return signer
	
	
func process_transaction(instructions:Array[Instruction], custom_signer_key:Pubkey=null, extra_signers:Array[Keypair]=[]) -> TransactionData:
	var tx_data:TransactionData
	
	if custom_signer_key!=null:
		var signers:Array = [custom_signer_key,SolanaService.wallet.get_kp()]
		for extra_signer in extra_signers:
			signers.append(extra_signer.to_pubkey())
			
		var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
		transaction.set_signers(signers)
		transaction.partially_sign(extra_signers)
		
		#get the signature from oracle. it returns tx_bytes. so turn to transaction again 
		var tx_bytes:PackedByteArray = await authority_signer.add_oracle_signature(transaction)
		transaction.queue_free()
		if tx_bytes.size()==0:
			push_error("Failed to sign with the oracle keypair!")
			var failed_tx:TransactionData = TransactionData.new({},{"error":"failed to receive oracle signature..."})
			SolanaService.transaction_manager.force_finish_transaction(failed_tx)
			return failed_tx
	
		var signed_transaction:Transaction = await SolanaService.transaction_manager.sign_transaction_serialized(tx_bytes,SolanaService.wallet.get_kp(),signers)
		tx_data = await SolanaService.transaction_manager.send_transaction(signed_transaction)
	else:
		var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
		for signer in extra_signers:
			transaction = await SolanaService.transaction_manager.add_signature(transaction,signer)
		await SolanaService.transaction_manager.add_signature(transaction,SolanaService.wallet.get_kp())
		tx_data = await SolanaService.transaction_manager.send_transaction(transaction,TransactionManager.Commitment.FINALIZED)
		
	return tx_data
	
func copy_to_args(args, plugin:PluginAuthorityPair):
	#if "initAuthority" in args:
		#args.initAuthority = plugin.authority
	args.plugin = plugin.plugin
	
	args.AddBlocker_value_0 = plugin.AddBlocker_value_0
	args.Attributes_value_0 = plugin.Attributes_value_0
	args.Autograph_value_0 = plugin.Autograph_value_0
	args.BurnDelegate_value_0 = plugin.BurnDelegate_value_0
	args.Edition_value_0 = plugin.Edition_value_0
	args.FreezeDelegate_value_0 = plugin.FreezeDelegate_value_0
	args.ImmutableMetadata_value_0 = plugin.ImmutableMetadata_value_0
	args.MasterEdition_value_0 = plugin.MasterEdition_value_0
	args.PermanentBurnDelegate_value_0 = plugin.PermanentBurnDelegate_value_0
	args.PermanentFreezeDelegate_value_0 = plugin.PermanentFreezeDelegate_value_0
	args.Royalties_value_0 = plugin.Royalties_value_0
	args.TransferDelegate_value_0 = plugin.TransferDelegate_value_0
	args.UpdateDelegate_value_0 = plugin.UpdateDelegate_value_0
	args.VerifiedCreators_value_0 = plugin.VerifiedCreators_value_0
	
	return args
	
func get_asset_core_data(asset:Pubkey) -> Dictionary:
	var full_data:Dictionary

	var core_asset:CoreAsset = SolanaService.asset_manager.get_owned_asset(asset)
	if core_asset != null:
		full_data = core_asset.das_metadata
	else:
		full_data = await SolanaService.get_asset_data(asset)
		
	var core_data:Dictionary = {
		"id":full_data["id"],
		"name":full_data["content"]["metadata"]["name"],
		"uri":full_data["content"]["json_uri"],
		"authority":full_data["authorities"][0]["address"],
		"plugins":full_data["plugins"],
		"external_plugins":full_data["external_plugins"]
	}
	return core_data
	
func mint_collection(name:String, uri:String, plugins:Array[PluginAuthorityPair]=[]) -> TransactionData:
	var collection_kp:Keypair = Keypair.new_random()
	print("Minting CORE Collection: %s" % collection_kp.get_public_string())
	
	var authority_key:Pubkey = SolanaService.wallet.get_pubkey()
	if authority_signer.get_signer_key() !=null:
		authority_key = authority_signer.get_signer_key()
		
	var ix:Instruction = collection_manager.get_mint_collection_ix(collection_kp,name,uri,authority_key,plugins)
	var tx_data:TransactionData = await process_transaction([ix],null,[collection_kp])
	tx_data.set_custom_data({"mint":collection_kp.to_pubkey()})
	if tx_data.is_successful():
		print("CORE Collection minted successfully")
	return tx_data
	
func edit_collection(collection:Pubkey,new_name:String="", new_uri:String="", new_update_authority:Pubkey = null) -> TransactionData:
	var ix:Instruction = collection_manager.get_edit_collection_ix(collection,new_name,new_uri,new_update_authority)
	var tx_data:TransactionData = await process_transaction([ix],authority_signer.get_signer_key())
	if tx_data.is_successful():
		print("CORE Collection %s updated successfully" % collection.to_string())
	return tx_data
	
func burn_collection(collection:Pubkey) -> TransactionData:
	var ix:Instruction = collection_manager.get_burn_collection_ix(collection)
	var tx_data:TransactionData = await process_transaction([ix],authority_signer.get_signer_key())
	if tx_data.is_successful():
		print("CORE Collection %s burned successfully" % collection.to_string())
	return tx_data
	
func manage_collection_plugin(collection:Pubkey, plugin:PluginAuthorityPair,manage_state:CoreManageState) -> TransactionData:
	var ix:Instruction = collection_manager.get_manage_collection_plugin_ix(collection, plugin,manage_state)
	var tx_data:TransactionData = await process_transaction([ix],authority_signer.get_signer_key())
	if tx_data.is_successful():
		print("CORE Collection %s plugin updated successfully" % collection.to_string())
	return tx_data
	
func mint_asset(name:String, uri:String, collection:Pubkey=null,custom_recipient:Pubkey=null, plugins:Array[PluginAuthorityPair]=[]) -> TransactionData:
	var asset_kp:Keypair = Keypair.new_random()
	print("Minting CORE Asset: %s" % asset_kp.get_public_string())
	
	var signer_key:Pubkey = null
	if collection != null:
		signer_key = authority_signer.get_signer_key()
		
	var ix:Instruction = asset_manager.get_mint_asset_ix(asset_kp,name,uri,collection,custom_recipient,plugins)
	var tx_data:TransactionData = await process_transaction([ix],signer_key,[asset_kp])
	tx_data.set_custom_data({"mint":asset_kp.to_pubkey()})
	if tx_data.is_successful():
		print("CORE Asset minted successfully")
	return tx_data
	
func edit_asset(asset:Pubkey,new_name:String="", new_uri:String="", collection:Pubkey=null, new_collection:Pubkey=null, authority:Pubkey=null,new_authority:Pubkey=null) -> TransactionData:
	var ix:Instruction = asset_manager.get_edit_asset_ix(asset,new_name,new_uri,collection,new_collection,authority, new_authority)
	var tx_data:TransactionData = await process_transaction([ix],authority_signer.get_signer_key())
	if tx_data.is_successful():
		print("CORE asset %s updated successfully" % asset.to_string())
	return tx_data
	
func burn_asset(asset:Pubkey,collection:Pubkey=null) -> TransactionData:
	var ix:Instruction = asset_manager.get_burn_asset_ix(asset,collection)
	var tx_data:TransactionData = await process_transaction([ix],authority_signer.get_signer_key())
	if tx_data.is_successful():
		print("CORE Asset %s burned successfully" % asset.to_string())
	return tx_data
	
func manage_asset_plugin(asset:Pubkey,collection:Pubkey, plugin:PluginAuthorityPair,manage_state:CoreManageState) -> TransactionData:
	var ix:Instruction = asset_manager.get_manage_plugin_ix(asset, collection, plugin,manage_state)
	var tx_data:TransactionData = await process_transaction([ix],authority_signer.get_signer_key())
	if tx_data.is_successful():
		print("CORE asset %s plugin updated successfully" % asset.to_string())
	return tx_data
