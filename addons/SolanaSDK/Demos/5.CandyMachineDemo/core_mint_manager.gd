extends Node
class_name CoreMintManager

@export var candy_machine_id:String
@export var guard_base_id:String
@export var guard_settings:CandyGuardData
@export var mint_plugins:MintAssetArgs

@export var candy_machine_display:CandyMachineDisplay
@export var loading_screen:Control
@export var mint_display_scn:PackedScene

@export var load_on_start:bool=true
@export var fetch_data_on_become_visible:bool=true

var cm_key:Pubkey
var base_guard_key:Pubkey

var cm_data:Dictionary

signal on_cm_data_fetched

signal on_asset_mint_started
signal on_asset_minted(nft_account:Pubkey)
signal on_asset_mint_failed
# Called when the node enters the scene tree for the first time.
func _ready() -> void:	
	cm_key = Pubkey.new_from_string(candy_machine_id)
	base_guard_key = Pubkey.new_from_string(guard_base_id)
	
	if mint_display_scn!=null:
		on_asset_minted.connect(display_minted_asset)
	
	if mint_plugins == null:
		mint_plugins = MintAssetArgs.new()
		mint_plugins.plugins = []
		
	if load_on_start:
		await refresh_candy_machine_data()
	if fetch_data_on_become_visible:
		self.visibility_changed.connect(on_visibility_changed)
	
func on_visibility_changed() -> void:
	if self.visible:
		await refresh_candy_machine_data()
			
func refresh_candy_machine_data() -> void:		
	if loading_screen!=null:
		loading_screen.visible=true
		
	cm_data = await SolanaService.candy_machine_manager.fetch_core_candy_machine(cm_key)
	print(cm_data)
	if cm_data!={}:
		candy_machine_display.load_cm_display(cm_data)
	
	on_cm_data_fetched.emit()
	
	if loading_screen!=null:
		loading_screen.visible=false


func try_mint(group_name:String, custom_mint_plugins:MintAssetArgs=null) -> void:
	on_asset_mint_started.emit()
	var asset_account:Keypair = SolanaService.generate_keypair()

	if guard_settings==null:
		push_error("Missing Candy Guard File")
		return
		
	var group_id:int = get_group_id_from_name(group_name)
#	made for default, can be overriden to a group
	var mint_group = null
	var meta_accounts:Array = guard_settings.default.get_extra_account_metas()
	if group_id>=0 and guard_settings.enable_groups:
		if guard_settings.enable_groups:
			var guard_group:Group = guard_settings.groups[group_id]
			mint_group = guard_group.label
			meta_accounts = guard_group.guards.get_extra_account_metas()
		
	var plugins:MintAssetArgs = mint_plugins
	if custom_mint_plugins!=null:
		plugins = mint_plugins
	
	#print("MINTING")
	#return 
	var tx_data:TransactionData = await SolanaService.candy_machine_manager.mint_core_nft_with_guards(
		cm_key,
		base_guard_key,
		SolanaService.wallet.get_kp(),
		SolanaService.wallet.get_pubkey(),
		cm_data,
		meta_accounts,
		mint_group,
		plugins,
		TransactionManager.Commitment.FINALIZED,
		asset_account
	)
	
	if !tx_data.is_successful():
		on_asset_mint_failed.emit()
		return
		
	await refresh_candy_machine_data()
		
	var asset_key:Pubkey = Pubkey.new_from_bytes(asset_account.get_public_bytes())
	on_asset_minted.emit(asset_key)
		
		
func display_minted_asset(asset_key:Pubkey) -> void:
	var mint_display:MintedAssetDisplay = mint_display_scn.instantiate()
	get_tree().root.add_child(mint_display)
	mint_display.display_asset(asset_key)
	
func get_group_id_from_name(group_name:String) -> int:
	if !guard_settings.enable_groups or group_name == "default":
		return -1
	
	for i in guard_settings.groups.size():
		var group:Group = guard_settings.groups[i]
		if group.label == group_name:
			return i
			
	return -1
		
