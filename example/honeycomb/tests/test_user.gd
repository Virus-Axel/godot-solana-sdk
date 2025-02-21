extends Node

# Dependencies and Constants
var utils = load("res://common/utils.gd").new()
var client: HoneyComb
var project: Dictionary
var profile: Dictionary
var user: Dictionary
var access_token: String = ""
var passed_test_mask: int = 0 # Track passed test cases using a bitmask
const TOTAL_CASES :int = 5 # Total number of test cases

# Load user information from resources
var user_info: UserInfoInput = load("res://resources/new_user_info_input.tres")

# Main execution function
func execute():
	await before_all()
	await run_tests()

# Run all test cases
func run_tests():
	await create_or_load_user_with_profile()
	await update_profile()
	await add_wallet()
	await remove_wallet()
	await claim_badge_criteria()

# Setup function to initialize required data
func before_all():
	add_child(utils)
	client = HoneyComb.new()
	client.set_honeycomb_url(utils.EGDE_CLIENT_PATH)
	add_child(client)  # Add the client to the node tree
	project = await utils.create_project(client)

# Test: Create or Load User with Profile
func create_or_load_user_with_profile():
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
	utils.PASS(0)

# Test: Update Profile
func update_profile():
	var update_info: ProfileInfoInput = load("res://resources/new_profile_info_input.tres").duplicate()
	update_info.name = user.info.name + "_profile"
	var custom_data_input: CustomDataInput = load("res://resources/new_custom_data_input.tres")
	custom_data_input.add = {"customField": ["customValue"]}

	# Create the transaction using the resource
	client.create_update_profile_transaction(
		profile.address,
		utils.user_keypair.get_public_string(),
		update_info,
		custom_data_input,
	)
	var tx_response = await client.query_response_received

	# Send the transaction using the provided utility function
	var encoded_tx = utils.extract_transaction(tx_response.createUpdateProfileTransaction)
	if not encoded_tx.is_empty():
		await utils.process_transaction(encoded_tx, [utils.user_keypair] as Array[Keypair])

	# Fetch the updated profile
	client.find_profiles([], [project.address], [user.id])
	var profile_response = await client.query_response_received
	profile = profile_response.profile[0] if profile_response.profile.size() > 0 else {}

	# Assertions to verify the updates
	assert(profile.info.name == user.info.name + "_profile", "Profile name should be updated.")
	assert(profile.customData.customField[0] == "customValue", "Custom field should be updated.")

	print("Profile successfully updated.")
	utils.PASS(1)

# Test: Add Wallet
func add_wallet():
	var update_wallet_input: UpdateWalletInput = load("res://resources/new_update_wallet_input.tres")
	var new_wallet: Keypair = Keypair.new_random()
	print("new_wallet: ", new_wallet.get_public_string())
	update_wallet_input.add = [new_wallet.get_public_string()] as PackedStringArray

	client.create_update_user_transaction(
		utils.user_keypair.get_public_string(),
		null,
		update_wallet_input,
		true
	)
	var tx_response = await client.query_response_received
	var encoded_tx = utils.extract_transaction(tx_response.createUpdateUserTransaction)
	if not encoded_tx.is_empty():
		await utils.process_transaction(encoded_tx, [utils.user_keypair] as Array[Keypair])
	
	client.find_users([], [], [utils.user_keypair.get_public_string()])
	var user_response = await client.query_response_received
	user = user_response.user[0] if user_response.user.size() > 0 else {}
	assert(user != null, "User should still exist.")
	print(user.wallets.wallets)
	print(new_wallet.get_public_string())
	assert(user.wallets.wallets[-1] == new_wallet.get_public_string(), "User should contain new wallet")
	utils.PASS(2)

# Test: Remove Wallet
func remove_wallet():
	var update_wallet_input: UpdateWalletInput = load("res://resources/new_update_wallet_input.tres").duplicate(true)
	update_wallet_input.remove = user.wallets.wallets[-1]
	update_wallet_input.add = []

	client.create_update_user_transaction(
		utils.user_keypair.get_public_string(),
		null,
		update_wallet_input,
		false
	)
	var tx_response = await client.query_response_received

	var encoded_tx = utils.extract_transaction(tx_response.createUpdateUserTransaction)
	if not encoded_tx.is_empty():
		await utils.process_transaction(encoded_tx, [utils.user_keypair] as Array[Keypair])

	client.find_users([], [], [utils.user_keypair.get_public_string()])
	var user_response = await client.query_response_received
	user = user_response.user[0] if user_response.user.size() > 0 else {}

	assert(user != null, "User should still exist after wallet removal.")
	assert(user.wallets.wallets.size() > 0, "Only one wallet should remain.")
	assert(user.wallets.wallets.has(utils.user_keypair.get_public_string()), "Remaining wallet should match the original public key.")

	print("Wallet successfully removed. Remaining wallet: ", user.wallets.wallets)
	utils.PASS(3)

# Test: Claim Badge Criteria
func claim_badge_criteria():
	var claim_badge_input: ClaimBadgeCriteriaInput = load("res://resources/new_claim_badge_criteria_input.tres")
	claim_badge_input.criteriaIndex = 0
	claim_badge_input.profileAddress = profile.address
	claim_badge_input.projectAddress = project.address
	claim_badge_input.proof = "Public"
	claim_badge_input.payer = utils.user_keypair.get_public_string()

	client.create_claim_badge_criteria_transaction(claim_badge_input)
	var tx_response = await client.query_response_received

	var encoded_tx = utils.extract_transaction(tx_response.createClaimBadgeCriteriaTransaction)
	if not encoded_tx.is_empty():
		await utils.process_transaction(encoded_tx, [utils.user_keypair] as Array[Keypair])

	client.find_profiles([], [project.address], [user.id])
	var profile_response = await client.query_response_received
	profile = profile_response.profile[0] if profile_response.profile.size() > 0 else {}

	assert(profile != null, "Profile should exist.")
	assert(profile.platformData.achievements != null, "Achievements should exist.")
	assert(profile.platformData.achievements.size() > 0, "Achievements should not be empty.")

	print("Badge criteria successfully claimed. Achievements: ", profile.platformData.achievements)
	utils.PASS(4)
