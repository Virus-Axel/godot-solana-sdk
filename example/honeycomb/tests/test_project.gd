extends Node

# Utility script loaded
var utils = load("res://common/utils.gd").new()

# Configuration Variables
var solana_utils: SolanaUtils
var client: HoneyComb
var project: Dictionary
var passed_test_mask: int = 0  # Track passed test cases using a bitmask
const TOTAL_CASES := 6  # Total number of test cases

# Entry point for executing test cases
func execute():
	before_all()
	await run_tests()

# Perform pre-test setup
func before_all():
	add_child(utils)
	client = HoneyComb.new()
	client.set_honeycomb_url(utils.EGDE_CLIENT_PATH)
	add_child(client)  # Add the client to the node tree

# Run all test cases sequentially
func run_tests():
	await create_or_load_project()
	await change_project_driver()
	await create_project_delegate_authority()
	await modify_project_delegate_authority()
	await create_or_load_badge_criteria()
	await update_badge_criteria()

# Test Case: Create or Load a Project
func create_or_load_project():
	print("Running test: create_or_load_project")
	var projectObj = await _create_project(utils.admin_keypair.get_public_string(), utils.admin_keypair.get_public_string())
	print("Project:", projectObj)

	client.find_projects([projectObj.project])
	var project_response = await client.query_response_received
	project = project_response.project[0]  # Retrieve the first project

	# Validate the project creation	
	assert(project != null, "Project should be created successfully.")
	if project.has("subsidyFees"):
		var tx = utils.transfer_lamports(projectObj.project, utils.admin_keypair, 1)
		print("Transaction sent, awaiting response...")
		var response = await tx.transaction_response_received
		if response:
			utils.print_transaction_url(response.result if response.has("result") else "")
	utils.PASS(0)

# Test Case: Change Project Driver
func change_project_driver():
	print("Running test: Change Project Driver")
	client.create_change_project_driver_transaction(project.address, utils.user_keypair.get_public_string(), utils.admin_keypair.get_public_string())
	var response = await client.query_response_received
	print("Response: ", response)

	if not response.is_empty():
		var encoded_tx = utils.extract_transaction(response.createChangeProjectDriverTransaction)
		if not encoded_tx.is_empty():
			await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])

	client.find_projects([project.address])
	var project_response = await client.query_response_received
	project = project_response.project[0]

	# Validate the driver update
	assert(project != null, "Project should exist.")
	assert(project.driver == utils.user_keypair.get_public_string(), "Driver should be updated correctly.")
	utils.PASS(1)

# Test Case: Create Project Delegate Authority
func create_project_delegate_authority():
	print("Running test: Create Project Delegate Authority")
	var service_delegation: ServiceDelegationInput = load("res://resources/new_service_delegation_input.tres")
	print("Service Delegation: ", service_delegation.to_dict())
	var hive_control: ServiceDelegationHiveControl = load("res://resources/new_service_delegation_hive_control.tres")
	hive_control.permission =  HiveControlPermissionInput.get_manageservices()
	service_delegation.HiveControl = [hive_control]

	client.create_create_delegate_authority_transaction(project.address, utils.user_keypair.get_public_string(), service_delegation, utils.admin_keypair.get_public_string(), utils.admin_keypair.get_public_string())
	var response = await client.query_response_received
	print("Response: ", response)

	if not response.is_empty():
		var encoded_tx = utils.extract_transaction(response.createCreateDelegateAuthorityTransaction)
		if not encoded_tx.is_empty():
			await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])

	client.find_projects([project.address])
	var project_response = await client.query_response_received
	project = project_response.project[0]

	# Validate the delegate authority
	assert(project != null, "Project should exist.")
	assert(project.authority == utils.admin_keypair.get_public_string(), "Authority should match admin keypair.")
	utils.PASS(2)

# Test Case: Modify Project Delegate Authority
func modify_project_delegate_authority():
	print("Running test: Modify Project Delegate Authority")
	var modify_delegation: ModifyDelegationInput = load("res://resources/new_modify_delegation_input.tres")
	var modify_service_delegation_input: ModifyServiceDelegationInput = load("res://resources/new_modify_service_delegation_input.tres")
	modify_delegation.delegation = modify_service_delegation_input
	var hive_control: ServiceDelegationHiveControl = load("res://resources/new_service_delegation_hive_control.tres")
	hive_control.permission =  HiveControlPermissionInput.get_managecriterias()
	modify_service_delegation_input.HiveControl = [hive_control]
	print(modify_delegation.to_dict())

	client.create_modify_delegation_transaction(project.address, utils.user_keypair.get_public_string(), modify_delegation, utils.admin_keypair.get_public_string(), utils.admin_keypair.get_public_string())
	var response = await client.query_response_received
	print("Response: ", response)

	if not response.is_empty():
		var encoded_tx = utils.extract_transaction(response.createModifyDelegationTransaction)
		if not encoded_tx.is_empty():
			await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])

	client.find_projects([project.address])
	var project_response = await client.query_response_received
	project = project_response.project[0]

	# Validate the modified authority
	assert(project != null, "Project should exist.")
	assert(project.authority == utils.admin_keypair.get_public_string(), "Authority should match admin keypair.")
	utils.PASS(3)

# Test Case: Create or Load Badge Criteria
func create_or_load_badge_criteria():
	print("Running test: Create/Load Badge Criteria")
	var badge_criteria_input: CreateBadgeCriteriaInput = load("res://resources/new_create_badge_criteria_input.tres")
	badge_criteria_input.authority = utils.admin_keypair.get_public_string()
	badge_criteria_input.projectAddress = project.address
	badge_criteria_input.endTime = int(Time.get_unix_time_from_system() + 60 * 60 * 24 * 7)
	badge_criteria_input.startTime = int(Time.get_unix_time_from_system())
	badge_criteria_input.badgeIndex = 0
	badge_criteria_input.payer = utils.admin_keypair.get_public_string()
	badge_criteria_input.condition = BadgesCondition.get_public()

	client.create_initialize_badge_criteria_transaction(badge_criteria_input)
	var response = await client.query_response_received
	print("Response: ", response)

	if not response.is_empty():
		var encoded_tx = utils.extract_transaction(response.createInitializeBadgeCriteriaTransaction)
		if not encoded_tx.is_empty():
			await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])

	client.find_projects([project.address])
	var project_response = await client.query_response_received
	project = project_response.project[0]

	# Validate badge criteria creation
	assert(project.badgeCriteria and project.badgeCriteria[0] != null, "Badge Criteria should exist.")
	utils.PASS(4)

# Test Case: Update Badge Criteria
func update_badge_criteria():
	print("Running test: Update Badge Criteria")
	var badge_criteria_input: UpdateBadgeCriteriaInput = load("res://resources/new_update_badge_criteria_input.tres")
	badge_criteria_input.authority = utils.admin_keypair.get_public_string()
	badge_criteria_input.projectAddress = project.address
	badge_criteria_input.endTime = int(Time.get_unix_time_from_system() + 60 * 60 * 24 * 7)
	badge_criteria_input.startTime = int(Time.get_unix_time_from_system())
	badge_criteria_input.criteriaIndex = 0
	badge_criteria_input.payer = utils.admin_keypair.get_public_string()
	badge_criteria_input.condition = BadgesCondition.get_public()

	client.create_update_badge_criteria_transaction(badge_criteria_input)
	var response = await client.query_response_received
	print("Response: ", response)

	if not response.is_empty():
		var encoded_tx = utils.extract_transaction(response.createUpdateBadgeCriteriaTransaction)
		if not encoded_tx.is_empty():
			await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])

	client.find_projects([project.address])
	var project_response = await client.query_response_received
	project = project_response.project[0]

	# Validate badge criteria update
	assert(project.badgeCriteria and project.badgeCriteria[0] != null, "Updated Badge Criteria should exist.")
	utils.PASS(5)

# Helper Functions
func _create_project(authority: String, owner_address: String):
	print("Creating new project...")
	var project_data = await create_project(
		authority,
		"First Project",
		"",  # Description
		[],  # Tags
		null,  # Category
		false,  # Is private
		owner_address  # Owner
	)

	if not project_data.is_empty():
		var encoded_tx = utils.extract_transaction(project_data.createCreateProjectTransaction)
		if not encoded_tx.is_empty():
			var sd = await utils.process_transaction(encoded_tx, [utils.admin_keypair] as Array[Keypair])
			print("cha", sd)
			return project_data.createCreateProjectTransaction


func create_project(authority: String, project_name: String, description: String = "", tags: Array = [], category = null, is_private: bool = false, owner_address: String = ""):
	client.create_create_project_transaction(
		authority,
		project_name,
		description,
		tags,
		category,
		is_private,
		owner_address if owner_address else authority
	)
	var response = await client.query_response_received
	return response
