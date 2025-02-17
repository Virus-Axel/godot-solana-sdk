extends HoneyComb

var utils = load("res://common/utils.gd").new()



func _ready():
	await before_all()
	await run_project_tests()
	await run_user_tests()
	await run_character_wrap_tests()
	# await run_mission_tests()
	 

func before_all():
	add_child(utils)
	set_honeycomb_url(utils.EGDE_CLIENT_PATH)
	await utils.create_or_find_wallet(utils.ADMIN_FILE_PATH, utils.AIRDROP_AMOUNT)
	await utils.create_or_find_wallet(utils.USER_FILE_PATH, utils.AIRDROP_AMOUNT)

func run_project_tests():
	var project_flow = load("res://tests/test_project.gd").new()
	add_child(project_flow)
	await project_flow.execute()

func run_user_tests():
	var user_flow = load("res://tests/test_user.gd").new()
	add_child(user_flow)
	await user_flow.execute()

func run_character_wrap_tests():
	var character_flow = load("res://tests/test_charater_wrap.gd").new()
	add_child(character_flow)
	await character_flow.execute()

func run_mission_tests():
	var mission_flow = load("res://tests/test_missions.gd").new()
	add_child(mission_flow)
	await mission_flow.execute()
