extends Node

## Dependencies and Constants
var utils = load("res://common/utils.gd").new()
#var assets = load("res://common/assets.gd").new()
var traits = load("res://common/traits.gd").new()
var base = load("res://common/base.gd").new()
var solana_client = SolanaClient.new()
var client: HoneyComb
var project: Dictionary
var character_model: Dictionary
var character: Dictionary
var assembler_config: Dictionary
#var assets: Dictionary = {}
var passed_test_mask: int = 0 # Track passed test cases using a bitmask
const TOTAL_CASES :int = 4 # Total number of test cases
#const TOTAL_MPL_CORE_NFTS = 1
#const TOTAL_NFTS = 1
#const TOTAL_CNFTS = 1
#const TOTAL_EXTENSIONS_NFT = 0

## Main execution function
func execute():
	await before_all()
	await run_tests()
#
# Run all test cases
func run_tests():
	await assemble_character_func()
	await update_character_traits()
	await unwrap_character()
	# Additional tests will be added step by step

# Perform pre-test setup
func before_all():
	add_child(utils)
	#add_child(assets)
	add_child(traits)
	add_child(base)
	add_child(solana_client)
	client = HoneyComb.new()
	client.set_honeycomb_url(utils.EGDE_CLIENT_PATH)
	add_child(client)  # Add the client to the node tree
	#print(traits.order)
	#print(traits.traits)
	project = await utils.create_project(
		client,
		false,
		false,
		false,
		utils.user_keypair.get_public_string(),
		utils.user_keypair
		)
	assembler_config = await base.create_assembler_config(
		client, 
		project,
		traits.order,
		traits.traits,
		utils.user_keypair.get_public_string(),
		utils.user_keypair)
	#print("assembler_config: ",assembler_config)
	
	character_model = await base.create_character_model_raw(
		client,
		project,
		assembler_config.address,
		utils.admin_keypair.get_public_string(),
		utils.user_keypair.get_public_string(),
		utils.user_keypair)
	#print("character_model: ",character_model)
	#await assets.mint_assets()
	utils.PASS(0)
	



# Test: Assemble a character
func assemble_character_func():
	character = await base.assemble_character(
		client,
		project["address"],
		assembler_config["address"],
		character_model["address"],
		character_model.merkle_trees.merkle_trees[character_model.merkle_trees.active],
		utils.user_keypair.get_public_string(),
		utils.user_keypair.get_public_string(),
		utils.user_keypair,
		)
	utils.PASS(1)


# Test: Update Character Traits
func update_character_traits():
	print("Running test: Update Character Traits")
	
	# Step 1: Get Pre-Balance
	solana_client.get_balance(utils.user_keypair.get_public_string())
	var pre_balance = await solana_client.http_response_received
	print("pre_balance: ", pre_balance)

	# Step 2: Create Update Character Traits Transaction
	client.create_update_character_traits_transaction(
		character["address"],
		project["address"],
		assembler_config["address"],
		character_model["address"],
		utils.user_keypair.get_public_string(),
		"",
		[
			["Fur", "Brown"],
			["Eyes", "2 Tone"],
			["Mouth", "Agitated"],
			["Clothes", "Astronaut"]
		],
		utils.user_keypair.get_public_string(),
		)

	var transaction_result = await client.query_response_received

	# Step 3: Validate and Process Transaction
	assert(not transaction_result.is_empty(), "Transaction response is empty!")
	var encoded_tx = utils.extract_transaction(transaction_result.createUpdateCharacterTraitsTransaction)
	assert(not encoded_tx.is_empty(), "Encoded transaction is empty!")

	await utils.process_transaction(encoded_tx, [utils.user_keypair] as Array[Keypair])

	# Step 4: Get Post-Balance
	solana_client.get_balance(utils.user_keypair.get_public_string())
	var post_balance = await solana_client.http_response_received
	print("post_balance: ", post_balance)
	 ## assert(pre_balance.result.value == post_balance.result.value, "Expected pre_balance to be equal to post_balance, but they are different!")


	# Step 5: Fetch Updated Character Data
	client.find_characters(
		false,  # Placeholder for optional boolean parameters
		[],
		null,
		[character_model.merkle_trees.merkle_trees[character_model.merkle_trees.active]],  # Active Merkle tree
		[utils.user_keypair.get_public_string()]
	)

	var character_result = await client.query_response_received
	var character_obj = character_result["character"][0] if character_result.has("character") else null

	# Step 6: Assert Character Retrieval and Pass Test
	assert(character_obj != null, "Character retrieval failed!")
	
	character = character_obj
	print("Character Traits Successfully Updated: ", character)

	utils.PASS(2)  # Mark test as passed


# Test: Unwrap a character
func unwrap_character():
	print("Running test: Unwrap Character")
	
	solana_client.get_balance(utils.user_keypair.get_public_string())
	var pre_balance = await solana_client.http_response_received
	print("pre_balance: ",pre_balance)
	
	
	client.create_unwrap_assets_from_character_transactions(
		project["address"],
		[character["address"]],
		character_model["address"],
		utils.user_keypair.get_public_string()
		)

	var transaction_result = await client.query_response_received

	if not transaction_result.is_empty():
		var transactions = transaction_result.createUnwrapAssetsFromCharacterTransactions.transactions
		for tx in transactions:
			if not tx.is_empty():
				await utils.process_transaction(tx, [utils.user_keypair] as Array[Keypair])
	
	
	solana_client.get_balance(utils.user_keypair.get_public_string())
	var post_balance = await solana_client.http_response_received
	print("post_balance: ",post_balance)
	 ## assert(pre_balance.result.value == post_balance.result.value, "Expected pre_balance to be equal to post_balance, but they are different!")

	
	client.find_characters(
		false,  # Placeholder for optional boolean parameters
		[character["address"]],
	)
	
	var character_result = await client.query_response_received
	var character_obj = character_result["character"][0] if character_result.has("character") else null

	assert(character_obj != null, "Character retrieval failed!")
	print("Character Successfully Unwrapped: ", character)

	utils.PASS(3)  # Mark test as passed
	#print("=============================Finished==========================")
