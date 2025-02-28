extends Node

## Dependencies and Constants
var utils = load("res://common/utils.gd").new()
var traits = load("res://common/traits.gd").new()
var base = load("res://common/base.gd").new()
var solana_client = SolanaClient.new()
var client: HoneyComb
var collection: String
var access_token: String
var merkle_tree: String

var profile: Dictionary
var resource: Dictionary
var project: Dictionary
var character_model: Dictionary
var character: Dictionary
var assembler_config: Dictionary

var mission_address: String =""
var resource_address: String =""
var character_address: String ="" 
var character_model_address: String ="" 
var staking_pool_address: String =""
var project_address: String =""
var assembler_config_address: String ="" 
var multipliers_address: String ="" 
  
var lookup_table_address: String  = ""

## Main execution function
func execute():
	await before_all()
	await run_tests()
#
# Run all test cases
func run_tests():
	await create_or_load_staking_pool()
	await update_staking_pool()
	await create_or_load_multipliers()
	await add_multipliers()
	await stake_characters()
	await create_or_load_lut_address()
	await unstake_characters()

# Perform pre-test setup
func before_all():
	add_child(utils)
	add_child(traits)
	add_child(base)
	add_child(solana_client)
	client = HoneyComb.new()
	client.set_honeycomb_url(utils.EGDE_CLIENT_PATH)
	add_child(client)  # Add the client to the node tree
	#print(traits.order)
	#print(traits.traits)
	
	
	if project_address.is_empty():
		project = await utils.create_project(
			client,
			false,
			true,
			false,
			utils.admin_keypair.get_public_string(),
			utils.admin_keypair)
		project_address = project["address"]
	else:
		client.find_projects([project_address])
		var project_resp = await client.query_response_received
		project = project_resp.project[0]
	

	if assembler_config_address.is_empty():
		assembler_config = await base.create_assembler_config(
			client,
			project_address,
			traits.order,
			traits.traits,
			utils.admin_keypair.get_public_string(),
			utils.admin_keypair
			)
		assembler_config_address= assembler_config["address"]
	else:
		client.find_assembler_config([assembler_config_address])
		var assembler_config_result = await client.query_response_received
		assembler_config = assembler_config_result["assemblerConfig"][0]
	
	
	if character_model_address.is_empty():
		character_model = await base.create_character_model_raw(
			client,
			project_address,
			assembler_config_address,
			utils.admin_keypair.get_public_string(),
			utils.admin_keypair.get_public_string(),
			utils.admin_keypair
			)
		character_model_address =character_model["address"]
	else:
		client.find_character_models([character_model_address])
		var character_model_result = await client.query_response_received
		character_model = character_model_result["characterModel"][0]
	
	
	if character_address.is_empty():
		character = await base.assemble_character(
			client,
			project_address,
			assembler_config_address,
			character_model_address,
			character_model.merkle_trees.merkle_trees[character_model.merkle_trees.active],
			utils.user_keypair.get_public_string(), # owner should be user
			utils.admin_keypair.get_public_string(),
			utils.admin_keypair
			)
		character_address = character["address"]
	else:
		client.find_characters(
		false,
		[],
		null,
		[character_model.merkle_trees.merkle_trees[character_model.merkle_trees.active]],
		[utils.user_keypair.get_public_string()]
		)
		var character_result = await client.query_response_received
		character = character_result["character"][0]
	
	
	await create_resource()
	#await assets.mint_assets()
	#utils.PASS(0)
	
	print("project: ",project)
	print("assembler_config: ",assembler_config)
	print("character_model: ",character_model)
	print("character: ",character)
	print("resource: ",resource)
	print("profile: ",profile)
	
	
func create_resource():
	if resource_address.is_empty():
		print("Creating Resource ....")
		var resource_input: InitResourceInput = load("res://resources/new_init_resource_input.tres")
		resource_input.name = "Test Resource"
		resource_input.symbol = "TC"
		resource_input.decimals = 6
		resource_input.uri = "https://arweave.net/1VxSzPEOwYlTo3lU5XSQWj-9Ldt3dB68cynDDjzeF-c"
		resource_input.storage = ResourceStorageEnum.get_accountstate()
		
		client.create_create_new_resource_transaction(
			project_address,
			utils.admin_keypair.get_public_string(),
			resource_input,
			"",
			utils.admin_keypair.get_public_string(),
			)
		var transaction_result = await client.query_response_received
		if not transaction_result.is_empty():
			var encoded_tx = utils.extract_transaction(transaction_result["createCreateNewResourceTransaction"])
			if not encoded_tx.is_empty():
				await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])
		
		
		resource_address = transaction_result["createCreateNewResourceTransaction"]["resource"]
		print("Resource Address: ", resource_address)
		
		# Mint Resource to User
		client.create_mint_resource_transaction(
			resource_address,
			utils.user_keypair.get_public_string(),
			utils.admin_keypair.get_public_string(),
			int(1000 * pow(10, 6))
			)
		var transaction_res = await client.query_response_received
		if not transaction_res.is_empty():
			var encoded_tx = utils.extract_transaction(transaction_res["createMintResourceTransaction"])
			if not encoded_tx.is_empty():
				await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])
	
	client.find_resources([resource_address])
	var resource_resp = await client.query_response_received
	resource = resource_resp.resources[0] if resource_resp.resources.size() > 0 else {}
	assert(resource != null, "Resource retrieval failed!")

func create_or_load_staking_pool():
	if project_address.is_empty():
		printerr("Project is not created, a valid project is needed to create a Staking Pool")
		return
	
	if resource_address.is_empty():
		printerr("Resource is not created, a valid resource is needed to create a Staking Pool")
		return

	if staking_pool_address.is_empty():
		print("Creating Staking Pool ...")
		
		var staking_pool_meta_data:CreateStakingPoolMetadataInput = load("res://resources/new_create_staking_pool_metadata_input.tres")
		staking_pool_meta_data.name =  "Staking"
		staking_pool_meta_data.rewardsPerDuration =  1
		staking_pool_meta_data.rewardsDuration =  1
		staking_pool_meta_data.maxRewardsDuration =  0
		staking_pool_meta_data.minStakeDuration =  0
		staking_pool_meta_data.cooldownDuration =  0
		staking_pool_meta_data.resetStakeDuration =  false
		staking_pool_meta_data.startTime =  Time.get_unix_time_from_system()
		staking_pool_meta_data.endTime =  0
		staking_pool_meta_data.lockType =  "Freeze"
		
		client.create_create_staking_pool_transaction(
			project_address,
			resource_address,
			staking_pool_meta_data,
			utils.admin_keypair.get_public_string(),
			)
			
		var transaction_result = await client.query_response_received
		
		if not transaction_result.is_empty():
			staking_pool_address = transaction_result["createCreateStakingPoolTransaction"]["stakingPoolAddress"]
			print("Staking Pool Address: ", staking_pool_address)
			var staking_pool_txns = transaction_result["createCreateStakingPoolTransaction"]["transactions"]
			for tx in staking_pool_txns.transactions:
				await utils.process_transaction(tx, [utils.admin_keypair] as Array[Keypair])
	
	client.find_staking_pools([staking_pool_address])
	var staking_pool_res = await client.query_response_received
	var pool = staking_pool_res.stakingPools[0] if staking_pool_res.stakingPools.size() > 0 else {}
	assert(pool != null, "Staking Pool retrieval failed!")
	print("Staking Pool successfully created or loaded.")
	utils.PASS(0)

func update_staking_pool():
	if staking_pool_address.is_empty():
		printerr("Staking Pool not created, a valid staking pool is needed to update")
		return
	
	print("Updating Staking Pool...")
	
	var staking_pool_meta_data:UpdateStakingPoolMetadataInput = load("res://resources/new_update_staking_pool_metadata_input.tres")
	staking_pool_meta_data.name =  "Honeycomb Staking"
	
	client.create_update_staking_pool_transaction(
		project_address,
		staking_pool_address,
		utils.admin_keypair.get_public_string(),
		staking_pool_meta_data,
		character_model_address,
		)
	
	var transaction_result = await client.query_response_received
	
	if not transaction_result.is_empty():
		var encoded_tx = utils.extract_transaction(transaction_result["createUpdateStakingPoolTransaction"])
		if not encoded_tx.is_empty():
			await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])
	
	client.find_staking_pools([staking_pool_address])
	var staking_pool_res = await client.query_response_received
	var pool = staking_pool_res.stakingPools[0] if staking_pool_res.stakingPools.size() > 0 else {}
	assert(pool != null, "Staking Pool update failed!")
	print("Staking Pool successfully updated.")
	utils.PASS(1)

func create_or_load_multipliers():
	if project_address.is_empty():
		printerr("Project is not created, a valid project is needed to create Multipliers")
		return
	
	if staking_pool_address.is_empty():
		printerr("Staking Pool is not created, a valid staking pool is needed to create Multipliers")
		return

	if multipliers_address.is_empty():
		print("Creating Multipliers ...")
		
		client.create_init_multipliers_transaction(
			project_address,
			staking_pool_address,
			3,
			[],
			utils.admin_keypair.get_public_string(),
		)
		var transaction_result = await client.query_response_received
		
		if not transaction_result.is_empty():
			multipliers_address = transaction_result["createInitMultipliersTransaction"]["multipliersAddress"]
			print("Multipliers Address: ", multipliers_address)
			var init_multiplier_tx = utils.extract_transaction(transaction_result["createInitMultipliersTransaction"])
			await utils.process_transaction(init_multiplier_tx, [utils.admin_keypair] as Array[Keypair])
	
	#await get_tree().create_timer(10.0).timeout
	
	client.find_multipliers([multipliers_address])
	var multipliers_res = await client.query_response_received
	var multipliers = multipliers_res.multipliers[0] if multipliers_res.multipliers.size() > 0 else {}
	assert(!multipliers.is_empty(), "Multipliers retrieval failed!")
	print("Multipliers successfully created or loaded.")
	utils.PASS(2)

func add_multipliers():
	if project_address.is_empty():
		printerr("Project is not created, a valid project is needed to add multipliers")
		return
	
	if staking_pool_address.is_empty():
		printerr("Staking Pool is not created, a valid staking pool is needed to add multipliers")
		return
	
	if multipliers_address.is_empty():
		printerr("Multipliers not created, a valid multipliers object is needed")
		return
	
	print("Adding Multipliers...")
	
	var multipliers_metadata = [
		#{"value": 100, "type": {"name": "collection" ,"value":"MT7kUCmOi5xYZZwXmDkwyoXK35la1bn9wD4c9zbzQzs"}},
		{"value": 300, "type": {"name": "creator" ,"value": utils.user_keypair.get_public_string()}},
		{"value": 300, "type": {"name": "minStakeDuration" ,"value":1}},
		#{"value": "300", "type": {"minNftCount": "1"}},
	]
	
	for i in 2:
		var multipliers_metadata_resource: AddMultiplierMetadataInput = load("res://resources/new_add_multiplier_metadata_input.tres")
		var multiplier_type_input: MultiplierTypeInput = load("res://resources/new_multiplier_type_input.tres")
		multiplier_type_input[multipliers_metadata[i].type.name] = multipliers_metadata[i].type.value
		multipliers_metadata_resource.type = multiplier_type_input 
		multipliers_metadata_resource.value = multipliers_metadata[i].value
		
		client.create_add_multiplier_transaction(
			project_address,
			multipliers_address,
			multipliers_metadata_resource,
			utils.admin_keypair.get_public_string(),
		)
		var transaction_result = await client.query_response_received
		
		if not transaction_result.is_empty():
			var encoded_tx = utils.extract_transaction(transaction_result["createAddMultiplierTransaction"])
			if not encoded_tx.is_empty():
				await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])
	
	client.find_multipliers([multipliers_address])
	var multipliers_res = await client.query_response_received
	var multipliers = multipliers_res.multipliers[0] if multipliers_res.multipliers.size() > 0 else {}
	assert(multipliers != null, "Multipliers retrieval failed!")
	print("Multipliers successfully added.")
	utils.PASS(3)

func stake_characters():
	var filters: CharactersFilter = load("res://resources/new_characters_filter.tres")
	filters.owner = utils.user_keypair.get_public_string()
	var used_by: CharacterUsedByFilter = load("res://resources/new_character_used_by_filter.tres")
	used_by.kind = "None"
	filters.usedBy = used_by
	client.find_characters(
		false,
		[],
		filters,
		character_model.merkle_trees.merkle_trees,
	)
	var characters_result = await client.query_response_received
	var characters = characters_result.character if characters_result.has("character") else []
	
	if characters.size() == 0:
		printerr("No characters to stake")
		return
	client.create_stake_characters_transactions(
		characters.map(func(x): return x["address"]),
		character_model_address,
		staking_pool_address,
		project_address,
		utils.user_keypair.get_public_string(),
		)
	
	var transaction_result = await client.query_response_received
	if not transaction_result.is_empty():
		var transactions = transaction_result.createStakeCharactersTransactions.transactions
		for tx in transactions:
			if not tx.is_empty():
				await utils.process_transaction(tx, [utils.user_keypair] as Array[Keypair])

	await get_tree().create_timer(10.0).timeout
	client.find_characters(
		false,
		[],
		null,
		character_model.merkle_trees.merkle_trees
		)
	var refetched_characters = await client.query_response_received
	assert(refetched_characters.character.size() == characters.size(), "Character staking validation failed!")
	for x in refetched_characters.character:
		assert(x.usedBy.kind == "Staking", "Character staking state mismatch!")
	print("Characters successfully staked!")
	utils.PASS(4)

func create_or_load_lut_address():
	print("Skipping lut addres for now. Due to no support. Participate in mission will not possible without lookup table. You can generate it using edge client.")
	utils.PASS(5)

func unstake_characters():
	var filters: CharactersFilter = load("res://resources/new_characters_filter.tres")
	filters.owner = utils.user_keypair.get_public_string()
	var used_by: CharacterUsedByFilter = load("res://resources/new_character_used_by_filter.tres")
	used_by.kind = "Staking"
	filters.usedBy = used_by
	
	client.find_characters(
		false,
		[],
		filters,
		character_model.merkle_trees.merkle_trees
	)
	var characters_result = await client.query_response_received 
	var characters = characters_result.character if characters_result.has("character") else []
	
	if characters.size() == 0:
		printerr("No characters to unstake")
		return

	client.create_unstake_characters_transactions(
		characters.map(func(x): return x["address"]),
		character_model_address,
		utils.user_keypair.get_public_string(),
		#[lookup_table_address]
	)
	
	var transaction_result = await client.query_response_received
	if not transaction_result.is_empty():
		var transactions = transaction_result.createSendCharactersOnMissionTransaction.transactions
		for tx in transactions:
			if not tx.is_empty():
				await utils.process_transaction(tx, [utils.user_keypair] as Array[Keypair])

	client.find_characters(false,characters.map(func(x): return x["address"]))
	var refetched_characters = await client.query_response_received
	assert(refetched_characters.character.size() == characters.size(), "Character unstaking validation failed!")
	for x in refetched_characters.character:
		assert(x.usedBy.kind == "None", "Character unstaking state mismatch!")
	print("Characters successfully unstaked!")
	utils.PASS(6)
