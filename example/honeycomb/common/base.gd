extends Node

## Dependencies and Constants
var utils = load("res://common/utils.gd").new()


# Function to create assembler config
func create_assembler_config(
	client: HoneyComb,
	 project_obj: Dictionary,
	 order: Array,
	 traits_arr: Array,
	 authority: String,
	 payer_keypair: Keypair
	):
	print("Creating Assembler Config...")
	add_child(utils)
	var tree_config: TreeSetupConfig = load("res://resources/new_tree_setup_config.tres")
	var basic_tree_config: BasicTreeConfig  = load("res://resources/new_basic_tree_config.tres")
	basic_tree_config.numAssets = 100000
	tree_config.basic = basic_tree_config
	
	# Step 1: Create Assembler Config Transaction
	client.create_create_assembler_config_transaction(
		utils.make_id(5),
		project_obj.address,
		authority,
		tree_config,
		order,
		payer_keypair.get_public_string(),
		)
	var response = await client.query_response_received
	#print("Response: ", response)

	if not response.is_empty():
		var encoded_tx = utils.extract_transaction(response.createCreateAssemblerConfigTransaction)
		if not encoded_tx.is_empty():
			# Step 2: Send Transaction
			await utils.process_transaction(encoded_tx, [payer_keypair] as Array[Keypair])

	var assembler_config_address = response.createCreateAssemblerConfigTransaction.assemblerConfig
	
	# Step 3: Fetch Assembler Config
	client.find_assembler_config([assembler_config_address])
	var assembler_config_result = await client.query_response_received
	#print("assembler_config_result:",assembler_config_result)
	var assembler_config = assembler_config_result["assemblerConfig"][0]
	#print("assembler_config: ",assembler_config)
	
	
	assert(not assembler_config.is_empty(), "Assembler Config retrieval failed!")
	

	# Step 4: If Traits Exist, Add Character Traits
	if traits_arr.size() > 0:
		client.create_add_character_traits_transactions(
			assembler_config["address"],
			traits_arr,
			authority,
			payer_keypair.get_public_string()
			)
		
		# Step 5: Send Transactions
		var trait_tx_result = await client.query_response_received
		#print("Response: ", trait_tx_result)
		
		if not trait_tx_result.is_empty():
			var transactions = trait_tx_result.createAddCharacterTraitsTransactions.transactions
			for tx in transactions:
				if not tx.is_empty():
					await utils.process_transaction(tx, [payer_keypair] as Array[Keypair])

		# Step 6: Fetch and Validate Character Traits
		client.find_character_traits([],assembler_config["merkle_trees"]["merkle_trees"])
		var character_traits_result = await client.query_response_received
		#print("character_traits_result",character_traits_result)
		
		
		
		var character_traits = character_traits_result["characterTrait"]
		
		assert(not character_traits.is_empty() and character_traits.size() == traits_arr.size(), "Character traits validation failed!")

	return assembler_config


# Function to create a Character Model
func create_character_model_raw(client: HoneyComb, project_obj: Dictionary,assembler_config_address: String,nft_creator_address: String, authority: String, payer_keypair: Keypair):
	print("Creating Character Model Raw...")
	
	if !is_inside_tree() or !utils.is_inside_tree():
		add_child(utils)
	
	var character_config: CharacterConfigInput = load("res://resources/new_character_config_input.tres")
	
	var new_nft_creator_input: NftCreatorInput = load("res://resources/new_nft_creator_input.tres")
	new_nft_creator_input.address = nft_creator_address
	new_nft_creator_input.share = 100
	
	var new_assembler_config_input: AssemblerConfigInput  =load("res://resources/new_assembler_config_input.tres")
	new_assembler_config_input.assemblerConfig= assembler_config_address
	new_assembler_config_input.name= "MplCore #0"
	new_assembler_config_input.symbol= "MPL_CORE"
	new_assembler_config_input.description= "Creating this MplCore Asset with assembler"
	new_assembler_config_input.creators= [new_nft_creator_input]
	new_assembler_config_input.sellerFeeBasisPoints= 0
	new_assembler_config_input.collectionName= "Collection"
	
	character_config.kind = "Assembled"
	character_config.assemblerConfigInput = new_assembler_config_input
	
	
	var mint_as_input: MintAsInput = load("res://resources/new_mint_as_input.tres")
	mint_as_input.kind = "MplCore"
	
	
	# Step 1: Create Character Model Transaction
	client.create_create_character_model_transaction(
		character_config,
		project_obj["address"],
		authority,
		[],
		mint_as_input,
		null,
		[],
		payer_keypair.get_public_string()
		)
	
	var transaction_result = await client.query_response_received

	
	if not transaction_result.is_empty():
		var encoded_tx = utils.extract_transaction(transaction_result.createCreateCharacterModelTransaction)
		if not encoded_tx.is_empty():
			# Step 2: Send Transaction
			await utils.process_transaction(encoded_tx, [payer_keypair] as Array[Keypair])

	var character_model_address = transaction_result["createCreateCharacterModelTransaction"]["characterModel"]
	

	var tree_setup_config: TreeSetupConfig =load("res://resources/new_tree_setup_config.tres")
	var advanced_tree_config: AdvancedTreeConfig =load("res://resources/new_advanced_tree_config.tres")
	advanced_tree_config.maxDepth = 3
	advanced_tree_config.maxBufferSize = 8 
	advanced_tree_config.canopyDepth =  3
	tree_setup_config.advanced = advanced_tree_config
	
	# Step 1: Create Characters Tree Transaction
	client.create_create_characters_tree_transaction(
		tree_setup_config,
		project_obj["address"],
		character_model_address,
		authority,
		payer_keypair.get_public_string(),
	)

	var tx_result = await client.query_response_received

	if not tx_result.is_empty():
		var encoded_tx = utils.extract_transaction(tx_result.createCreateCharactersTreeTransaction)
		if not encoded_tx.is_empty():
			# Step 2: Send Transaction
			await utils.process_transaction(encoded_tx, [payer_keypair] as Array[Keypair])

	# Step 3: Find Character Model
	client.find_character_models([character_model_address])
	var character_model_result = await client.query_response_received
	
	var character_model_obj = character_model_result["characterModel"][0] if character_model_result.has("characterModel") else null
	
	assert(character_model_obj != null, "Character retrieval failed!")
	
	return character_model_obj


func assemble_character(client: HoneyComb, project_address: String, assembler_config_address: String, character_model_address: String, tree_address: String, owner: String, authority: String, payer_keypair: Keypair):
	print("Assemble Character...")
	var solana_client = SolanaClient.new()
	add_child(solana_client)
	# Step 1: Get Pre-Balance
	solana_client.get_balance(payer_keypair.get_public_string())
	var pre_balance = await solana_client.http_response_received
	print("pre_balance: ",pre_balance)
	
	# Step 2: Create Assemble Character Transaction
	client.create_assemble_character_transaction(
		project_address,
		assembler_config_address,
		character_model_address,
		owner,
		authority,
		"",
		[
			["Fur", "Black"],
			["Eyes", "2 Tone"],
			["Mouth", "Agitated"],
			["Clothes", "Astronaut"]
		],
		payer_keypair.get_public_string()
		)
	var transaction_result = await client.query_response_received
	
	if not transaction_result.is_empty():
		var encoded_tx = utils.extract_transaction(transaction_result.createAssembleCharacterTransaction)
		if not encoded_tx.is_empty():
			# Step 2: Send Transaction
			await utils.process_transaction(encoded_tx, [payer_keypair] as Array[Keypair])

	# Step 4: Get Post-Balance
	solana_client.get_balance(payer_keypair.get_public_string())
	var post_balance = await solana_client.http_response_received
	print("post_balance: ",post_balance)
	 ## assert(pre_balance.result.value == post_balance.result.value, "Expected pre_balance to be equal to post_balance, but they are different!")

	

	# Step 5: Wait (Equivalent to `await wait(10)`)
	await get_tree().create_timer(10.0).timeout

	# Step 6: Find Character
	client.find_characters(
		false,
		[],
		null,
		[tree_address],
		[owner]
	)

	var character_result = await client.query_response_received
	var character_obj = character_result["character"][0] if character_result.has("character") else null
	
	assert(character_obj != null, "Character retrieval failed!")
	
	return character_obj
