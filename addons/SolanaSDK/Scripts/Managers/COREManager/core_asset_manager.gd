extends Node
class_name CoreAssetManager

var core_manager:CoreManager

func _ready() -> void:
	core_manager = get_parent() as CoreManager

func get_mint_asset_ix(asset_kp:Keypair,name:String, uri:String, collection:Pubkey=null,custom_recipient:Pubkey=null, plugins:Array[PluginAuthorityPair]=[]) -> Instruction:
	var update_auth:Pubkey = null
#	Mint authority is only set if the asset doesnt have a collection, so minter is set to be authority
	var mint_authority = null
		
	if collection == null:
		update_auth = SolanaService.wallet.get_pubkey()
	else:
		mint_authority = core_manager.get_authority_signer()
		
	var asset_owner:Pubkey = SolanaService.wallet.get_pubkey()
	if custom_recipient!=null:
		asset_owner = custom_recipient
		
	var args:CreateV2Args = CreateV2Args.new()
	args.name = name
	args.uri = uri
	
	if plugins.size() >0:
		args.enable_plugins=true
		args.plugins = plugins
	
	var core_instance:mpl_core_program = core_manager.get_mpl_core_instance()
	var mint_asset_ix:Instruction = core_instance.CreateV2(
		asset_kp,
		collection,
		mint_authority,
		SolanaService.wallet.get_kp(),
		asset_owner,
		update_auth,
		SystemProgram.get_pid(),
		null,
		args
	)
	core_instance.queue_free()
	
	return mint_asset_ix
	
func get_edit_asset_ix(asset:Pubkey, new_name:String="", new_uri:String="", collection:Pubkey=null,new_collection:Pubkey=null,authority:Pubkey=null,new_authority:Pubkey=null) -> Instruction:
	var core_instance:mpl_core_program = core_manager.get_mpl_core_instance()
	
	var args:UpdateV2Args = UpdateV2Args.new()
	args.enable_newName = true
	if new_name.length() > 0:
		args.enable_newName = true
		args.newName = new_name
	if new_uri.length() > 0:
		args.enable_newUri = true
		args.newUri = new_uri
	if new_authority!=null:
		args.enable_newUpdateAuthority = true
		#args.newUpdateAuthority = new_authority

	var edit_asset_ix:Instruction = core_instance.UpdateV2(
		asset,
		collection,
		SolanaService.wallet.get_kp(),
		authority,
		new_collection,
		SystemProgram.get_pid(),
		null,
		args
	)
	
	core_instance.queue_free()
	return edit_asset_ix	

	
func get_burn_asset_ix(asset:Pubkey,collection:Pubkey=null) -> Instruction:
	var args:BurnV1Args = BurnV1Args.new()
	
	var core_instance:mpl_core_program = core_manager.get_mpl_core_instance()
	var burn_asset_ix:Instruction = core_instance.BurnV1(
		asset,
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		SystemProgram.get_pid(),
		null,
		args
		)
	core_instance.queue_free()
	
	return burn_asset_ix
	
func get_manage_plugin_ix(asset:Pubkey,collection:Pubkey, plugin:PluginAuthorityPair, manage_state:CoreManager.CoreManageState) -> Instruction:
	var core_instance:mpl_core_program = core_manager.get_mpl_core_instance()
	var args
	var ix:Instruction
	match manage_state:
		CoreManager.CoreManageState.Add:
			args = AddPluginV1Args.new()
			args = core_manager.copy_to_args(args,plugin)
			ix = get_add_plugin_ix(asset,collection,args,core_instance)
			pass
		CoreManager.CoreManageState.Edit:
			args = UpdatePluginV1Args.new()
			args = core_manager.copy_to_args(args,plugin)
			ix = get_edit_plugin_ix(asset,collection,args,core_instance)
			pass
		CoreManager.CoreManageState.Remove:
			args = RemovePluginV1Args.new()
			args.pluginType = plugin.plugin
			ix = get_remove_plugin_ix(asset,collection,args,core_instance)
			pass
			
	core_instance.queue_free()
	return ix
	
	
func get_add_plugin_ix(asset:Pubkey,collection:Pubkey, args, core_instance:mpl_core_program) -> Instruction:
	var add_plugin_ix:Instruction = core_instance.AddPluginV1(
		asset,
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		SystemProgram.get_pid(),
		null,
		args
		)
	return add_plugin_ix
	
func get_edit_plugin_ix(asset:Pubkey,collection:Pubkey, args, core_instance:mpl_core_program)  -> Instruction:
	var edit_plugin_ix:Instruction = core_instance.UpdatePluginV1(
		asset,
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		SystemProgram.get_pid(),
		null,
		args
		)
	return edit_plugin_ix
	
func get_remove_plugin_ix(asset:Pubkey, collection:Pubkey, args, core_instance:mpl_core_program)  -> Instruction:
	var remove_plugin_ix:Instruction = core_instance.RemovePluginV1(
		asset,
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		SystemProgram.get_pid(),
		null,
		args
		)
	return remove_plugin_ix
