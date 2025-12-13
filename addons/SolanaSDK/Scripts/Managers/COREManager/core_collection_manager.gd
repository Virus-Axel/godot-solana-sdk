extends Node
class_name CoreCollectionManager

var core_manager:CoreManager

func _ready() -> void:
	core_manager = get_parent() as CoreManager

func get_mint_collection_ix(collection_kp:Keypair,name:String, uri:String, authority_key:Pubkey, plugins:Array[PluginAuthorityPair]=[]) -> Instruction:
	var args:CreateCollectionV2Args = CreateCollectionV2Args.new()
	args.name = name
	args.uri = uri
	
	if plugins.size()>0:
		args.enable_plugins=true
		args.plugins = plugins
	
	var core_instance:mpl_core_program = core_manager.get_mpl_core_instance()
	var mint_collection_ix:Instruction = core_instance.CreateCollectionV2(
		collection_kp,
		authority_key,
		SolanaService.wallet.get_kp(),
		SystemProgram.get_pid(),
		args
		)
	core_instance.queue_free()
		
	return mint_collection_ix
	
func get_edit_collection_ix(collection:Pubkey,new_name:String="", new_uri:String="", new_update_authority:Pubkey = null) -> Instruction:
	print("Updating Collection %s" % collection.to_string())
	var core_instance:mpl_core_program = core_manager.get_mpl_core_instance()
	
	var args:UpdateCollectionV1Args = UpdateCollectionV1Args.new()
	args.enable_newName = true
	if new_name.length() > 0:
		args.enable_newName = true
		args.newName = new_name
	if new_uri.length() > 0:
		args.enable_newUri = true
		args.newUri = new_uri
	
	var edit_collection_ix:Instruction = core_instance.UpdateCollectionV1(
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		new_update_authority,
		SystemProgram.get_pid(),
		null,
		args
		)
	core_instance.queue_free()
	
	return edit_collection_ix
	
func get_burn_collection_ix(collection:Pubkey) -> Instruction:
	var args:BurnCollectionV1Args = BurnCollectionV1Args.new()
	
	var core_instance:mpl_core_program = core_manager.get_mpl_core_instance()
	var burn_collection_ix:Instruction = core_instance.BurnCollectionV1(
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		null,
		args
		)
	core_instance.queue_free()
	
	return burn_collection_ix
	
func get_manage_collection_plugin_ix(collection, plugin:PluginAuthorityPair, manage_state:CoreManager.CoreManageState) -> Instruction:
	var core_instance:mpl_core_program = core_manager.get_mpl_core_instance()
	var args
	var ix:Instruction
	match manage_state:
		CoreManager.CoreManageState.Add:
			args = AddCollectionPluginV1Args.new()
			args = core_manager.copy_to_args(args,plugin)
			ix = get_add_collection_plugin_ix(collection,args,core_instance)
			pass
		CoreManager.CoreManageState.Edit:
			args = UpdateCollectionPluginV1Args.new()
			args = core_manager.copy_to_args(args,plugin)
			ix = get_edit_collection_plugin_ix(collection,args,core_instance)
			pass
		CoreManager.CoreManageState.Remove:
			args = RemoveCollectionPluginV1Args.new()
			args.pluginType = plugin.plugin
			ix = get_remove_collection_plugin_ix(collection,args,core_instance)
			pass
			
	core_instance.queue_free()
	return ix
	
	
func get_add_collection_plugin_ix(collection:Pubkey, args, core_instance:mpl_core_program) -> Instruction:
	var add_collection_plugin_ix:Instruction = core_instance.AddCollectionPluginV1(
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		SystemProgram.get_pid(),
		null,
		args
		)
	return add_collection_plugin_ix
	
func get_edit_collection_plugin_ix(collection:Pubkey, args, core_instance:mpl_core_program)  -> Instruction:
	var edit_collection_plugin_ix:Instruction = core_instance.UpdateCollectionPluginV1(
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		SystemProgram.get_pid(),
		null,
		args
		)
	return edit_collection_plugin_ix
	
func get_remove_collection_plugin_ix(collection:Pubkey, args, core_instance:mpl_core_program)  -> Instruction:
	var remove_collection_plugin_ix:Instruction = core_instance.RemoveCollectionPluginV1(
		collection,
		SolanaService.wallet.get_kp(),
		core_manager.get_authority_signer(),
		SystemProgram.get_pid(),
		null,
		args
		)
	return remove_collection_plugin_ix
