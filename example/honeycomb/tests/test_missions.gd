extends Node

## Dependencies and Constants
var utils = load("res://common/utils.gd").new()
var traits = load("res://common/traits.gd").new()
var base = load("res://common/base.gd").new()
var solana_client = SolanaClient.new()
var client: HoneyComb
var collection: String;
var resource_address: String;
var mission_pool_address: String;
var mission_address: String;
var lookup_table_address: String;
var access_token: String;
var merkle_tree: String;
var mission_pool: Dictionary;
var characters_on_mission: Array = []
var mission;

var profile: Dictionary;
var project_address: String;
var user: Dictionary;
var resource: Dictionary;
var project: Dictionary
var character_model: Dictionary
var character: Dictionary
var character_model_address: String;
var assembler_config: Dictionary

# PROFILE PART
var user_info = {
	"username": utils.user_keypair.get_public_string(),
	"name": "Honeycomb Developer",
	"bio": "This user is created for testing purposes",
	"pfp": "https://lh3.googleusercontent.com/-Jsm7S8BHy4nOzrw2f5AryUgp9Fym2buUOkkxgNplGCddTkiKBXPLRytTMXBXwGcHuRr06EvJStmkHj-9JeTfmHsnT0prHg5Mhg",
}

var profile_info = {
	"name": "(Profile) " + user_info["username"],
	"bio": "This is profile of " + user_info["username"],
	"pfp": "https://lh3.googleusercontent.com/-Jsm7S8BHy4nOzrw2f5AryUgp9Fym2buUOkkxgNplGCddTkiKBXPLRytTMXBXwGcHuRr06EvJStmkHj-9JeTfmHsnT0prHg5Mhg",
}


#var project: Dictionary= { "associatedPrograms": [], "authority": "5M6Q2rAQbwG8CWi9Yn25K4JZfGoVUxcRGq4TfSa5Zxew", "bump": 255, "driver": "11111111111111111111111111111111", "address": "8D7aA9VFaXNMAMYT5uFqHjQQud7spV2QJEeur5zYg9z6", "key": "G4vXtE3zjEdm64e2hzCu7gMpLSiNbYTpZch8xrfKpVz6", "name": "Test Project", "profileDataConfig": { "achievements": [], "customDataFields": [] }, "profileTrees": { "active": 0, "merkle_trees": ["8uzdMfwsHED8qcDGLmiXearBcpzf2YpUoLeqLrxQ2rod"], "schema": { "__schema": { "__kind": "object", "params": { "info": { "__kind": "object", "params": { "bio": { "type": { "__kind": "string" }, "__kind": "option" }, "pfp": { "type": { "__kind": "string" }, "__kind": "option" }, "name": { "type": { "__kind": "string" }, "__kind": "option" } } }, "project": { "__kind": "pubkey" }, "user_id": { "__kind": "number" }, "identity": { "__kind": "string" }, "custom_data": { "__kind": "object", "params": { "inner": { "__kind": "map", "key_type": { "__kind": "string" }, "value_type": { "type": { "__kind": "string" }, "__kind": "array" } } } }, "platform_data": { "__kind": "object", "params": { "xp": { "__kind": "number" }, "custom": { "__kind": "map", "key_type": { "__kind": "string" }, "value_type": { "type": { "__kind": "string" }, "__kind": "array" } }, "achievements": { "type": { "__kind": "number" }, "__kind": "array" } } } } } } }, "badgeCriteria": null, "services": [], "subsidyFees": false }
#var project_address: String = project["address"]
#var assembler_config: Dictionary= { "address": "EEDZBTy8YJeuAse3G2YLFav2t31wTKAJumQ2YHrnT2fU", "program_id": "ChRCtrG7X5kb9YncA4wuyD68DXXL8Szt3zBCCGiioBTg", "discriminator": "qfkdnCWJGG", "ticker": "rdApK", "project": "8D7aA9VFaXNMAMYT5uFqHjQQud7spV2QJEeur5zYg9z6", "order": ["Fur", "Eyes", "Mouth", "Clothes"], "merkle_trees": { "active": 0, "merkle_trees": ["4FnZFBAmyktvJAdZvScoW4Swjbi9kVXpP2uH5tR5Yhua"], "schema": { "__schema": { "__kind": "object", "params": { "uri": { "__kind": "string" }, "name": { "__kind": "string" }, "layer": { "__kind": "string" } } } } } }
#var character_model: Dictionary= { "address": "9rfuemFtaSbNekwsoMGqhMu6rMLdsKKZh46MeZoqdXcM", "program_id": "ChRCtrG7X5kb9YncA4wuyD68DXXL8Szt3zBCCGiioBTg", "discriminator": "9BU2p2XDqmS", "bump": 255, "key": "AkZhgNR73H6xzSMxegaMwFQwC5Y1Q66Jmcf3Xri4jMwk", "equipableCriteria": [], "project": "8D7aA9VFaXNMAMYT5uFqHjQQud7spV2QJEeur5zYg9z6", "attributes": { "__schema": { "__kind": "null" } }, "merkle_trees": { "active": 0, "merkle_trees": ["3vG39inZziYKUAaoa4ew2efmYZHWmW9ec9wFDLx8iqy4"], "schema": { "__schema": { "__kind": "object", "params": { "owner": { "__kind": "pubkey" }, "source": { "__kind": "enum", "variants": { "Wrapped": { "__kind": "object", "params": { "kind": { "__kind": "enum", "variants": { "MplMetadata": { "__kind": "null" }, "MplBubblegum": { "__kind": "null" }, "MplCoreAsset": { "__kind": "null" }, "TokenExtensions": { "__kind": "null" } } }, "mint": { "__kind": "pubkey" }, "criteria": { "__kind": "enum", "variants": { "Creator": { "__kind": "object", "params": { "0": { "__kind": "pubkey" } } }, "Collection": { "__kind": "object", "params": { "0": { "__kind": "pubkey" } } }, "MerkleTree": { "__kind": "object", "params": { "0": { "__kind": "pubkey" } } }, "Prepopulated": { "__kind": "null" } } } } }, "Assembled": { "__kind": "object", "params": { "uri": { "__kind": "string" }, "hash": { "type": { "__kind": "pubkey" }, "__kind": "option" }, "mint": { "__kind": "pubkey" }, "attributes": { "__kind": "map", "key_type": { "__kind": "string" }, "value_type": { "__kind": "string" } }, "assembler_config": { "__kind": "pubkey" } } } } }, "used_by": { "__kind": "enum", "variants": { "None": { "__kind": "null" }, "Guild": { "__kind": "object", "params": { "id": { "__kind": "pubkey" }, "role": { "__kind": "enum", "variants": { "Chief": { "__kind": "null" }, "Member": { "__kind": "null" } } }, "order": { "__kind": "number" } } }, "Custom": { "__kind": "object", "params": { "data": { "__kind": "null" }, "user": { "__kind": "pubkey" } } }, "Ejected": { "__kind": "null" }, "Mission": { "__kind": "object", "params": { "rewards": { "type": { "__kind": "object", "params": { "delta": { "__kind": "number" }, "collected": { "__kind": "bool" }, "reward_idx": { "__kind": "number" } } }, "__kind": "array" }, "end_time": { "__kind": "number" }, "mission_id": { "__kind": "pubkey" }, "participation_id": { "__kind": "number" } } }, "Staking": { "__kind": "object", "params": { "pool": { "__kind": "pubkey" }, "staker": { "__kind": "pubkey" }, "staked_at": { "__kind": "number" }, "claimed_at": { "__kind": "number" } } } } }, "cooldown": { "__kind": "object", "params": { "ejection": { "__kind": "number" } } }, "equipments": { "__kind": "map", "key_type": { "__kind": "string" }, "value_type": { "__kind": "number" } } } } } }, "cooldown": { "ejection": 0 }, "config": { "kind": "Assembled", "assemblerConfig": "EEDZBTy8YJeuAse3G2YLFav2t31wTKAJumQ2YHrnT2fU", "name": "MplCore #0", "symbol": "MPL_CORE", "description": "Creating this MplCore Asset with assembler", "creators": [{ "address": "5M6Q2rAQbwG8CWi9Yn25K4JZfGoVUxcRGq4TfSa5Zxew", "share": 100 }], "sellerFeeBasisPoints": 0, "collectionName": "Collection", "mintAs": { "kind": "MplCore", "params": null } } }
#var character_model_address: String =  character_model["address"]
#var character: Dictionary= { "source": { "kind": "Assembled", "params": { "hash": "FSnUQjqRK19oVNfX53dpBJbs21US4s9c4SRJ9WeJU9ua", "mint": "11111111111111111111111111111111", "uri": "http://localhost:4000/cdn/json/GuPjPZG4nkzD9Uhv4Z7ZFctuodAsZTxZAoWg6cT1kdXw.json", "attributes": { "Fur": "Black", "Eyes": "2 Tone", "Mouth": "Agitated", "Clothes": "Astronaut" }, "assemblerConfig": "EEDZBTy8YJeuAse3G2YLFav2t31wTKAJumQ2YHrnT2fU" } }, "owner": "5M6Q2rAQbwG8CWi9Yn25K4JZfGoVUxcRGq4TfSa5Zxew", "proof": null, "usedBy": { "kind": "None", "params": null }, "equipments": {  }, "leaf_idx": "0", "tree_id": "3vG39inZziYKUAaoa4ew2efmYZHWmW9ec9wFDLx8iqy4", "address": "wyx2PPbqVbmbRGYxSyhskhHvbwGWq5EhfEW4Td8Rrni", "asset": null, "cooldown": { "ejection": 0 } }
#var resource: Dictionary= { "address": "18v5JncG1jviSd3VQNN5QTBRWxbtsjLjog4dhHgVbm3", "bump": 255, "kind": { "kind": "HplFungible", "params": { "__typename": "ResourceKindParamsHplFungible", "decimals": 6 } }, "mint": "A4iDn7VmXZtJbKSz9nRq3R7TTMEiyaWj54eRCmS3eUCq", "project": "8D7aA9VFaXNMAMYT5uFqHjQQud7spV2QJEeur5zYg9z6", "tags": [], "storage": { "params": null, "kind": "AccountState" } }
#var user: Dictionary= { "address": "6WMCN2FqpmrANuGq7WwVqNg8XfZK5AxQ2rGdEPRaFKfb", "id": 1, "info": { "bio": "This user is created for testing purposes", "name": "Honeycomb Developer", "pfp": "https://lh3.googleusercontent.com/-Jsm7S8BHy4nOzrw2f5AryUgp9Fym2buUOkkxgNplGCddTkiKBXPLRytTMXBXwGcHuRr06EvJStmkHj-9JeTfmHsnT0prHg5Mhg", "username": "" }, "socialInfo": { "discord": null, "twitter": null, "civic": [], "steam": null }, "wallets": { "shadow": "2KM85qr1PFqDx1HroGSSdH5QbYtAgcb1H4chakTzVGw7", "wallets": ["Go8RXKh8ud1kB9d2diRLaxgDQ3qP3B5Lzj9E39wnG7n2", "6JAKiYWUhv3J3UpM3Sxj5i2gYyVfVKEVTpH1uegNEe54", "GrxJZ1PK4vuuUQJZyaRwBr1Xat4yiEqVFiRw6xiLZs6i", "G5C4rb6Uf9Lt5UUTbjJ67fH9cACk6z5QYbKpqExR7kEM", "D4kXJ57fAzPwx6VKQcUpBvihXhGFiJvFawQSFevmCYkY", "CxiPbjzuhdMDoB1mdsKxEW21iChTwUsmfu42wrFajYwj", "FQY93AFtxzDoZAd847MpKiwNRLfEenqMDtzEBkYe968h", "J88uCjwwktRYBSJWkmugz47svcjtRu7hu54ceRCCZ1s8", "9ZXTPAMUn449QUJqZJ42y3ivDiWtBWgcpuCgB1PzYQDd", "DGYMpkGfeVjU31BhBq1sA8QHsqvbnhDbamW1XuVSVX26"] }, "profiles": null, "leaf_idx": "0", "tree_id": "HkgwKEphBYFcUsDJRe9Yoz3CsaKmXjcJps9yXJrSEjE", "proof": null }
#var profile: Dictionary= { "customData": {  }, "address": "4QgXjXfEpX6TqprBSxh8N8u9NNGJEKSHtF1MnNgRihbM", "identity": "Main", "info": { "bio": "This is profile of This user is created for testing purposes", "name": "(Profile) Honeycomb Developer", "pfp": "https://lh3.googleusercontent.com/-Jsm7S8BHy4nOzrw2f5AryUgp9Fym2buUOkkxgNplGCddTkiKBXPLRytTMXBXwGcHuRr06EvJStmkHj-9JeTfmHsnT0prHg5Mhg" }, "platformData": { "achievements": [], "custom": {  }, "xp": "0" }, "project": "8D7aA9VFaXNMAMYT5uFqHjQQud7spV2QJEeur5zYg9z6", "userId": 1, "leaf_idx": "0", "tree_id": "8uzdMfwsHED8qcDGLmiXearBcpzf2YpUoLeqLrxQ2rod", "proof": null, "user": null }

## Main execution function
func execute():
	await before_all()
	await run_tests()
#
# Run all test cases
func run_tests():
	await create_or_load_mission_pool()
	await update_mission_pool()
	await create_mission()
	await update_mission()
	await create_or_load_lut_address()
	await participate_in_mission()
	#await collect_rewards_and_recall()
	# Additional tests will be added step by step

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
	project = await utils.create_project(
		client,
		false,
		true,
		false,
		utils.admin_keypair.get_public_string(),
		utils.admin_keypair)
	project_address = project["address"]
	assembler_config = await base.create_assembler_config(
		client,
		project,
		traits.order,
		traits.traits,
		utils.admin_keypair.get_public_string(),
		utils.admin_keypair
		)
	
	character_model = await base.create_character_model_raw(
		client,
		project,
		assembler_config.address,
		utils.admin_keypair.get_public_string(),
		utils.admin_keypair.get_public_string(),
		utils.admin_keypair
		)
	character_model_address =character_model["address"]
	
	
	character = await base.assemble_character(
		client,
		project_address,
		assembler_config["address"],
		character_model_address,
		character_model.merkle_trees.merkle_trees[character_model.merkle_trees.active],
		utils.user_keypair.get_public_string(), # owner should be user
		utils.admin_keypair.get_public_string(),
		utils.admin_keypair
		)
	
	await create_resource()	
	await create_user_with_profile_func()
	#await assets.mint_assets()
	#utils.PASS(0)
	
	print("project: ",project)
	print("assembler_config: ",assembler_config)
	print("character_model: ",character_model)
	print("character: ",character)
	print("resource: ",resource)
	print("user: ",user)
	print("profile: ",profile)
	
	
func create_resource():
	if resource_address == null or resource_address.is_empty():
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

func create_user_with_profile_func():
	if user == null or user.is_empty():
		# Step 1: Check if user exists by wallet
		client.find_users([], [], [utils.user_keypair.get_public_string()])
		var user_response = await client.query_response_received
		user = user_response.user[0] if user_response.user.size() > 0 else {}
		print("user: ", user)

		# Step 2: If user doesn't exist, create a new user with profile
		if not user.has("address"):
			client.create_new_user_with_profile_transaction(
				project.address,
				utils.user_keypair.get_public_string(),
				user_info,
				utils.user_keypair.get_public_string(),
			)
			var res = await client.query_response_received
			var txn = utils.extract_transaction(res.createNewUserWithProfileTransaction)
			if not txn.is_empty():
				await utils.process_transaction(txn, [utils.user_keypair] as Array[Keypair])

			# Re-fetch user after creation
			client.find_users([], [], [utils.user_keypair.get_public_string()])
			var user_resp = await client.query_response_received
			user = user_resp.user[0] if user_resp.user.size() > 0 else {}

			# Assertions to validate user creation
			assert(user != null, "User should be created successfully.")
			assert(user.info.name == user_info.name, "User name should match.")
			assert(user.info.bio == user_info.bio, "User bio should match.")
			assert(user.info.pfp == user_info.pfp, "User profile picture should match.")

	if profile == null or profile.is_empty():
		# Step 3: Authorize the user if not already authorized
		if access_token.is_empty():
			await utils.initiate_auth_request(client, utils.user_keypair, utils.user_keypair.get_public_string())
			access_token = utils.load_auth_token()
			client.set_auth_token(access_token)
			var headers = client.get_headers()
			print("headers: ", headers)

		# Step 4: Check if a profile exists for the user
		client.find_profiles([], [project.address], [user.id])
		var profile_response = await client.query_response_received
		print("profile_response: ", profile_response)
		profile = profile_response.profile[0] if profile_response.profile.size() > 0 else {}

		# Step 5: If profile exists, validate it matches the user info
		if profile.has("info"):
			assert(profile.info.name == user_info.name, "Profile name should match.")
			assert(profile.info.bio == user_info.bio, "Profile bio should match.")
			assert(profile.info.pfp == user_info.pfp, "Profile picture should match.")
			return

		# Step 6: Create a new profile if it doesn't exist
		var profileInfo: ProfileInfoInput = load("res://resources/new_profile_info_input.tres")
		profileInfo.name = "(Profile) %s" % user_info["name"]
		profileInfo.bio = "This is profile of %s" % user_info["bio"]
		profileInfo.pfp = user_info.pfp

		client.create_new_profile_transaction(
			project.address,
			utils.user_keypair.get_public_string(),
			"",
			profileInfo,
		)
		var tx_response = await client.query_response_received
		var encoded_tx = utils.extract_transaction(tx_response.createNewProfileTransaction)
		if not encoded_tx.is_empty():
			await utils.process_transaction(encoded_tx, [utils.user_keypair] as Array[Keypair])

		# Step 7: Fetch and validate the new profile
		client.find_profiles([], [project.address], [user.id])
		var profile_res = await client.query_response_received
		profile = profile_res.profile[0] if profile_res.profile.size() > 0 else {}
		
		# Assertions to validate profile creation
		assert(profile != null, "Profile should exist.")
		assert(profile.info.name == "(Profile) %s" % user_info["name"], "Profile name should match.")
		assert(profile.info.bio == "This is profile of %s" % user_info["bio"], "Profile bio should match.")
		assert(profile.info.pfp == user_info.pfp, "Profile picture should match.")

		print("User and Profile successfully created or loaded.")


func create_or_load_mission_pool():
	if project_address == null or project_address.is_empty():
		printerr("Project is not created, a valid project is needed to create a Mission Pool")
		return
	
	if character_model_address == null or character_model_address.is_empty():
		printerr("Character Model is not created, a valid character model is needed to create a Mission Pool")
		return

	if mission_pool_address == null or mission_pool_address.is_empty():
		print("Creating Mission Pool ...")
		var mission_pool_data: NewMissionPoolData = load("res://resources/new_new_mission_pool_data.tres")
		mission_pool_data.name = "Test Mission Pool"
		mission_pool_data.project = project.address
		mission_pool_data.payer = utils.admin_keypair.get_public_string()
		mission_pool_data.authority = utils.admin_keypair.get_public_string()
		#mission_pool_data.delegateAuthority = utils.admin_keypair.get_public_string()
		mission_pool_data.characterModel = character_model_address

		# Create Mission Pool Transaction
		client.create_create_mission_pool_transaction(mission_pool_data)
		var transaction_result = await client.query_response_received

		if not transaction_result.is_empty():
			var encoded_tx = utils.extract_transaction(transaction_result["createCreateMissionPoolTransaction"])
			if not encoded_tx.is_empty():
				await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])


		mission_pool_address = transaction_result["createCreateMissionPoolTransaction"]["missionPoolAddress"]
		print("Mission Pool Address: ", mission_pool_address)

	# Fetch the Mission Pool
	client.find_mission_pools([mission_pool_address])
	var mission_pool_res = await client.query_response_received
	mission_pool = mission_pool_res.missionPool[0] if mission_pool_res.missionPool.size() > 0 else {}

	# Validate Mission Pool Creation
	assert(mission_pool != null, "Mission Pool retrieval failed!")
	print("Mission Pool successfully created or loaded.")
	

func update_mission_pool():
	if project_address == null or project_address.is_empty():
		printerr("Project is not created, a valid project is needed to update a Mission Pool")
		return
	
	if mission_pool_address == null or mission_pool_address.is_empty():
		printerr("Mission Pool not created, a valid mission pool is needed to update a Mission Pool")
		return
	
	if character_model_address == null or character_model_address.is_empty():
		printerr("Character Model is not created, a valid character model is needed to update a Mission Pool")
		return

	print("Updating Mission Pool...")

	# Prepare Mission Pool Update Data
	var update_mission_pool_data: UpdateMissionPoolData = load("res://resources/new_update_mission_pool_data.tres")
	update_mission_pool_data.project = project.address
	update_mission_pool_data.missionPool = mission_pool_address
	update_mission_pool_data.authority = utils.admin_keypair.get_public_string()
	update_mission_pool_data.payer = utils.admin_keypair.get_public_string()

	# Create Transaction for Updating Mission Pool
	client.create_update_mission_pool_transaction(update_mission_pool_data)
	var transaction_result = await client.query_response_received

	if not transaction_result.is_empty():
		var encoded_tx = utils.extract_transaction(transaction_result["createUpdateMissionPoolTransaction"])
		if not encoded_tx.is_empty():
			await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])

	# Add a short delay for finalization
	#await get_tree().create_timer(5.0).timeout

	# Fetch Updated Mission Pool
	client.find_mission_pools([mission_pool_address])
	var mission_pool_res = await client.query_response_received
	mission_pool = mission_pool_res.missionPool[0] if mission_pool_res.missionPool.size() > 0 else {}

	# Validate Mission Pool Update
	assert(mission_pool != null, "Mission Pool update failed!")
	print("Mission Pool successfully updated.")
	

func create_mission():
	if project_address == null or project_address.is_empty():
		printerr("Project not created, a valid project is needed to create a Mission")
		return
	
	if mission_pool_address == null or mission_pool_address.is_empty():
		printerr("Mission Pool not created, a valid mission pool is needed to create a Mission")
		return

	if mission_address == null or mission_address.is_empty():
		print("Creating Mission ...")

		# Load Mission Data
		var mission_data: NewMissionData = load("res://resources/new_new_mission_data.tres")
		var new_mission_cost: NewMissionCost = load("res://resources/new_new_mission_cost.tres")
		new_mission_cost.address = resource_address
		new_mission_cost.amount = 100 * pow(10, 6)  # Convert to string
		
		# Cost Object
		mission_data.project = project.address
		mission_data.name = "Test mission"
		mission_data.cost = new_mission_cost
		
		# Mission Duration & Requirements
		mission_data.duration = 1 # 1 second
		mission_data.minXp = 0

		# Rewards Array
		mission_data.rewards = [
			{
				"kind": RewardKind.get_xp(),
				"max": "100",
				"min": "100",
			},
			{
				"kind": RewardKind.get_resource(),
				"max": str(500 * pow(10, 6)),
				"min": str(100 * pow(10, 6)),
				"resource": resource.address,
			},
		]

		# Mission Pool & Authority
		mission_data.missionPool = mission_pool_address
		mission_data.authority = utils.admin_keypair.get_public_string()
		mission_data.payer = utils.admin_keypair.get_public_string()

		# Create Transaction
		client.create_create_mission_transaction(mission_data)
		var transaction_result = await client.query_response_received

		if not transaction_result.is_empty():
			var encoded_tx = utils.extract_transaction(transaction_result["createCreateMissionTransaction"])
			if not encoded_tx.is_empty():
				await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])

		mission_address = transaction_result["createCreateMissionTransaction"]["missionAddress"]
		print("Mission Address: ", mission_address)

	# Fetch the Mission
	client.find_missions([mission_address])
	var mission_res = await client.query_response_received
	mission = mission_res.mission[0] if mission_res.mission.size() > 0 else {}

	# Validate Mission Creation
	assert(mission != null, "Mission retrieval failed!")
	print("Mission successfully created or loaded.")


func update_mission():
	if project_address == null or project_address.is_empty():
		printerr("Project not created, a valid project is needed to update a Mission")
		return
	
	if mission_pool_address == null or mission_pool_address.is_empty():
		printerr("Mission Pool not created, a valid mission pool is needed to update a Mission")
		return

	if mission_address != null and !mission_address.is_empty():
		print("Updating Mission ...")

		# Prepare Mission Update Data
		var update_mission_data: UpdateMissionInput = load("res://resources/new_update_mission_input.tres")
		update_mission_data.newRewards = []
		update_mission_data.updateRewards = [
			{
					"kind": RewardKind.get_xp(),
					"max": "300",
					"min": "100",
			}
		]
		update_mission_data.removeRewards = []
		update_mission_data.minXp = 0
		update_mission_data.duration = 1
		update_mission_data.cost = {
				"amount": "0",
				"address": resource.address,
			}


		# Create Update Mission Transaction
		client.create_update_mission_transaction(
			mission_address,
			utils.admin_keypair.get_public_string(),
			update_mission_data,
			utils.admin_keypair.get_public_string()
			)
		var transaction_result = await client.query_response_received

		if not transaction_result.is_empty():
			var encoded_tx = utils.extract_transaction(transaction_result["createUpdateMissionTransaction"])
			if not encoded_tx.is_empty():
				await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])

		# Fetch the Updated Mission
		client.find_missions([mission_address])
		var mission_res = await client.query_response_received
		mission = mission_res.mission[0] if mission_res.mission.size() > 0 else {}

		# Validate Mission Update
		assert(mission != null, "Mission update failed!")
		assert(mission.has("rewards"), "Mission rewards not found!")
		assert(mission.rewards.size() == 2, "Mission should have 2 rewards after update!")

		print("Mission successfully updated.")



func create_or_load_lut_address():
	print("Skipping lut addres for now. Due to no support")


func participate_in_mission():
	if project_address == null or project_address.is_empty():
		printerr("Project not created, a valid project is needed to participate in a Mission")
		return
	
	if mission_address == null or mission_address.is_empty():
		printerr("Mission not created, a valid mission is needed to participate in a Mission")
		return

	# Find Characters associated with the user
	#var character_query = {
		#"wallets": [utils.user_keypair.get_public_string()],
		#"trees": character_model.merkle_trees.merkle_trees
	#}
	client.find_characters(false,[],null,character_model.merkle_trees.merkle_trees,[utils.user_keypair.get_public_string()])
	#client.find_characters(character_query)
	var character_response = await client.query_response_received
	if character_response.character.size() > 0:
		characters_on_mission.append(character_response.character[0])

	print("Character ", characters_on_mission[0].address, "Participating")

	# Check if all characters are already on a mission
	var all_characters_on_mission = true
	for character in characters_on_mission:
		if character.usedBy.kind != "Mission":
			all_characters_on_mission = false
			break

	if all_characters_on_mission:
		print("Characters already on mission")
		return

	print("Character ", characters_on_mission[0].address, "Participating")

	# Create Send Characters On Mission Transaction
	var participate_on_mission_data: ParticipateOnMissionData = load("res://resources/new_participate_on_mission_data.tres")
	participate_on_mission_data.mission = mission_address
	participate_on_mission_data.characterAddresses = [characters_on_mission[0].address]
	participate_on_mission_data.authority = utils.user_keypair.get_public_string()
	participate_on_mission_data.userId = user.id


	client.create_send_characters_on_mission_transaction(participate_on_mission_data)
	var transaction_result = await client.query_response_received
	if not transaction_result.is_empty():
		var transactions = transaction_result.createSendCharactersOnMissionTransaction.transactions
		for tx in transactions:
			if not tx.is_empty():
				await utils.process_transaction(tx, [utils.user_keypair] as Array[Keypair])
	

	# Verify if character is now used in a mission
	for character in characters_on_mission:
		client.find_characters(
			false,
			[character.address]
		)
		var updated_character_response = await client.query_response_received
		if updated_character_response.character.size() > 0:
			var updated_character = updated_character_response.character[0]
			assert(updated_character.has("usedBy"), "Character should have 'usedBy' after mission participation")

	print("Characters successfully sent on mission!")


func collect_rewards_and_recall():
	if project_address == null or project_address.is_empty():
		printerr("Project not created, a valid project is needed to claim a Mission")
		return
	
	if mission_address == null or mission_address.is_empty():
		printerr("Mission not created, a valid mission is needed to claim a Mission")
		return

	# Wait for mission's end
	print("Waiting for mission to end (Collect Rewards Scenario)")
	await get_tree().create_timer(5).timeout
	
	var recall_from_mission_data :RecallFromMissionData= load("res://resources/new_recall_from_mission_data.tres")
	recall_from_mission_data.mission = mission_address
	recall_from_mission_data.characterAddresses = [characters_on_mission[0].address]
	recall_from_mission_data.authority =utils.user_keypair.get_public_string()
	recall_from_mission_data.userId = user.id

	

	client.create_recall_characters_transaction(recall_from_mission_data)
	var transaction_result = await client.query_response_received
	
	if not transaction_result.is_empty():
		var transactions = transaction_result.createRecallCharactersTransaction.transactions
		for tx in transactions:
			if not tx.is_empty():
				await utils.process_transaction(tx, [utils.user_keypair] as Array[Keypair])
				await get_tree().create_timer(2).timeout

	print("Rewards collected, and characters recalled successfully!")
