extends Node
class_name SoarProgram

#for building custom program instructions, first goes function name, then array of accounts
#and then variables to pass in

#IMPORTANT:
#if no variables need to be passed in, write 'null'
#if multiple variables, put them in a dictionary
#if the variable is a class, pass it in as dictionary

@onready var soar_program:AnchorProgram = $SOAR_PROGRAM

func get_pid() -> Pubkey:
		return Pubkey.new_from_string(soar_program.get_pid())
		
func fetch_game_data(game_account:Pubkey) -> Dictionary:
	soar_program.fetch_account("Game",game_account)
	var result:Dictionary = await soar_program.account_fetched
	return result
	
func fetch_leaderboard_data(leaderboard_account:Pubkey) -> Dictionary:
	soar_program.fetch_account("LeaderBoard",leaderboard_account)
	var result:Dictionary = await soar_program.account_fetched
	return result
	
func fetch_leaderboard_scores(leaderboard_account:Pubkey) -> Dictionary:
	var leaderboard_top_entries_pda:Pubkey = SoarPDA.get_leaderboard_scores_pda(leaderboard_account,get_pid())
	soar_program.fetch_account("LeaderTopEntries",leaderboard_top_entries_pda)
	var result:Dictionary = await soar_program.account_fetched
	return result
	
func fetch_player_data(user_account:Pubkey) -> Dictionary:
	var player_pda:Pubkey = SoarPDA.get_player_pda(user_account,get_pid())
	soar_program.fetch_account("Player",player_pda)
	var result:Dictionary = await soar_program.account_fetched
	return result
	
func fetch_player_data_from_pda(player_pda:Pubkey) -> Dictionary:
	soar_program.fetch_account("Player",player_pda)
	var result:Dictionary = await soar_program.account_fetched
	return result
	
func fetch_player_scores(user_account:Pubkey,leaderboard_account:Pubkey) -> Dictionary:
	var player_account_pda:Pubkey = SoarPDA.get_player_pda(user_account,get_pid())
	var player_scores_pda:Pubkey = SoarPDA.get_player_scores_pda(player_account_pda,leaderboard_account,get_pid())
	
	soar_program.fetch_account("PlayerScoresList",player_scores_pda)
	var result:Dictionary = await soar_program.account_fetched
	return result

func init_game(game_attributes:SoarUtils.GameAttributes) -> TransactionData:
	var game_account:Keypair = SolanaService.generate_keypair()
	var instructions:Array[Instruction]
	var init_game_ix:Instruction = soar_program.build_instruction("initializeGame",[
		SolanaService.wallet.get_kp(), #creator
		game_account, #game
		Pubkey.new_from_string("11111111111111111111111111111111") #system program
	],{
		"GameMeta":game_attributes.get_value(),
		"GameAuth":[SolanaService.wallet.get_pubkey()]
	})
	
	print("Creating Game Account with ID: %s"%game_account.get_public_string())
	instructions.append(init_game_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	transaction = await SolanaService.transaction_manager.add_signature(transaction,game_account)
	transaction = await SolanaService.transaction_manager.add_signature(transaction,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.send_transaction(transaction)
	return tx_data
		

	
func add_leaderboard(game_address:String,leaderboard_data:SoarUtils.LeaderboardData,game_auth:Keypair) -> TransactionData:
	var game_account:Pubkey=Pubkey.new_from_string(game_address)
	var game_data:Dictionary = await fetch_game_data(game_account)
	
	if game_data.size() == 0:
		push_error("Failed to fetch the game data")
		return TransactionData.new({})
		
	var leaderboard_id:int = game_data["leaderboardCount"]+1
	var leaderboard:Pubkey = SoarPDA.get_leaderboard_pda(game_account,leaderboard_id,get_pid())
	var leaderboard_top_entries:Pubkey = SoarPDA.get_leaderboard_scores_pda(leaderboard, get_pid())
	
	var instructions:Array[Instruction]
	var add_leaderboard_ix:Instruction = soar_program.build_instruction("addLeaderboard",[
		game_auth, #game authority
		SolanaService.wallet.get_kp(), #payer
		game_account, #game
		leaderboard, #leaderboard PDA
		leaderboard_top_entries, #top entries PDA
		SystemProgram.get_pid() #system program
	],
	leaderboard_data.get_value())
	
	print("Creating Leaderboard with ID: %s"%leaderboard.get_value())
	
	instructions.append(add_leaderboard_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_and_send(transaction)
	return tx_data
	
	
func update_leaderboard(game_address:String,leaderboard_address:String,leaderboard_data:SoarUtils.LeaderboardData,game_auth:Keypair) -> TransactionData:
	var game_account:Pubkey=Pubkey.new_from_string(game_address)
	var leaderboard_account:Pubkey = Pubkey.new_from_string(leaderboard_address)
	var leaderboard_top_entries:Pubkey = SoarPDA.get_leaderboard_scores_pda(leaderboard_account, get_pid())

	var instructions:Array[Instruction]
	var update_leaderboard_ix:Instruction = soar_program.build_instruction("updateLeaderboard",[
		game_auth, #game authority
		game_account, #game
		leaderboard_account, #leaderboard 
		leaderboard_top_entries #top entries PDA
	],{
		"newDescription":AnchorProgram.option(leaderboard_data.get_value()["description"]),
		"newNftMeta":AnchorProgram.option(leaderboard_data.get_value()["nft_meta"]),
		"newMinScore":leaderboard_data.get_value()["min_score"],
		"newMaxScore":leaderboard_data.get_value()["max_score"],
		"newIsAscending":AnchorProgram.option(leaderboard_data.get_value()["is_ascending"]),
		"newAllowMultipleScores":AnchorProgram.option(leaderboard_data.get_value()["allow_multiple_scores"])
	})
	
	print("Updating Leaderboard with ID: %s"%leaderboard_account.get_value())
	
	instructions.append(update_leaderboard_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_and_send(transaction)
	return tx_data
	

func initialize_player(username:String, user_nft:Pubkey) -> TransactionData:
	var player_account:Pubkey = SoarPDA.get_player_pda(SolanaService.wallet.get_pubkey(),get_pid())
	var instructions:Array[Instruction]
	var init_player_ix:Instruction = soar_program.build_instruction("initializePlayer",[
		SolanaService.wallet.get_kp(), #payer
		SolanaService.wallet.get_kp(), #user
		player_account, #player PDA
		SystemProgram.get_pid() #system program
	],{
		"username":username,
		"nftMeta":user_nft
	})
	
	print("Initializing Player account with ID: %s"%player_account.get_value())
	instructions.append(init_player_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_and_send(transaction)
	return tx_data
	
		
func update_player(username:String, user_nft:Pubkey) -> TransactionData:
	var player_account:Pubkey = SoarPDA.get_player_pda(SolanaService.wallet.get_pubkey(),get_pid())
	
	var instructions:Array[Instruction]
	var update_player_ix:Instruction = soar_program.build_instruction("updatePlayer",[
		SolanaService.wallet.get_kp(), #user
		player_account, #player PDA
	],{
		"username":AnchorProgram.option(username),
		"nftMeta":AnchorProgram.option(user_nft)
	})
	
	print("Updating Player account with ID: %s"%player_account.get_value())
	instructions.append(update_player_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_and_send(transaction)
	return tx_data
		

func submit_score_to_leaderboard(game_account:Pubkey,leaderboard_account:Pubkey,game_auth:Keypair,score:int) -> TransactionData:
	var player_account_pda:Pubkey = SoarPDA.get_player_pda(SolanaService.wallet.get_pubkey(),get_pid())
	var player_scores_pda:Pubkey = SoarPDA.get_player_scores_pda(player_account_pda,leaderboard_account,get_pid())
	var instructions:Array[Instruction]
	
	#check if player already has a scores account for this leaderboard and if not, add ix of registering them
	#player scores list returns info on player's score on a specific leaderboard. even if it's empty, the account may exists
	var player_scores_list:Dictionary = await fetch_player_scores(SolanaService.wallet.get_pubkey(),leaderboard_account)
	#for result in player_scores_list["scores"]:
		#print(int(result["score"]))

	if player_scores_list.size()==0:
		var register_player_ix:Instruction = soar_program.build_instruction("registerPlayer",[
		SolanaService.wallet.get_kp(), #payer
		SolanaService.wallet.get_kp(), #user
		player_account_pda, #player PDA
		game_account, #game account 
		leaderboard_account, #leaderboard
		player_scores_pda, #new list of scores pda for the player
		SystemProgram.get_pid() #system program
		],null)
		
		instructions.append(register_player_ix)
		
	var leaderboard_data:Dictionary = await fetch_leaderboard_data(leaderboard_account)
	var leaderboard_top_entries_pda:Pubkey = SoarPDA.get_leaderboard_scores_pda(leaderboard_account,get_pid())
	var player_score = AnchorProgram.u64(score * pow(10,leaderboard_data["decimals"]))
	
	var submit_score_ix:Instruction = soar_program.build_instruction("submitScore",[
		SolanaService.wallet.get_kp(), #payer
		game_auth, #authority
		player_account_pda, #player PDA
		game_account, #game account 
		leaderboard_account, #leaderboard 
		player_scores_pda, #player scores PDA
		leaderboard_top_entries_pda, #nleaderboard's score sheet to add the player's score to
		SystemProgram.get_pid() #system program
		],{
			"score":player_score
		})
		
	instructions.append(submit_score_ix)
	var transaction:Transaction = await SolanaService.transaction_manager.create_transaction(instructions,SolanaService.wallet.get_kp())
	var tx_data:TransactionData = await SolanaService.transaction_manager.sign_and_send(transaction)
	return tx_data
	
