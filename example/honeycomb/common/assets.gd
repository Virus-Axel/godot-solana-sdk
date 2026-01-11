# extends Node
# #
# ## Dependencies and Constants
# var utils = load("res://common/utils.gd").new()
# #var client: HoneyComb
# const MINT_SIZE :int = 82
# const MINT_ACCOUNT_SIZE :int= 165


# # Define asset response structure
# var core: Dictionary
# var pnfts: Dictionary
# var cnfts: Dictionary
# var token22: Dictionary

# # New addresses
# var mint_keypair: Keypair = Keypair.new_random()
# var mint_account_keypair: Keypair = Keypair.new_random()
# var candy_machine_keypair: Keypair = Keypair.new_random()


## AssetCounts type definition
#var AssetCounts: Dictionary = {
	#"core": null, # Optional, can be an integer or null
	#"pnfts": null, # Optional, can be an integer or null
	#"cnfts": null, # Optional, can be an integer or null
	#"token22": null # Optional, can be an integer or null
#}
#
## CollectionWithItems structure
#var CollectionWithItems: Dictionary = {
	#"group": "", # String representing Pubkey
	#"mints": [], # Array for Mint values
	#"asset": "" # Asset type: "MPL_CORE", "MPL_TM", "MPL_BG", or "TOKEN_2022"
#}
#
## AssetResponse type definition
#var AssetResponse: Dictionary = {
	#"core": null, # Can be a CollectionWithItems dictionary or null
	#"pnfts": null, # Can be a CollectionWithItems dictionary or null
	#"cnfts": null, # Can be a CollectionWithItems dictionary or null
	#"token22": null # Can be a CollectionWithItems dictionary or null
#}


# Mint MPL Core Collection
#func mint_mpl_core_collection(item_count: int, beneficiary: Pubkey, group: Pubkey):
	#if item_count <= 0:
		#return null
#
	#var asset_signers = []
	#for i in range(item_count):
		#asset_signers.append(Keypair.new_random())
		#
	
	# initialize_mint_collection()
	

	#var collection_with_mints = {"group": group, "mints": [], "asset": "MPL_CORE"}
	#if group == "" and asset_signers.size() > 0:
		#var collection_signer = Keypair.new_random()
		#umi.create_collection(collection_signer, "My Collection", "https://example.com/my-collection.json")
		#collection_with_mints["group"] = collection_signer.public_key
#
	#for i in asset_signers:
		#umi.create_v1(i, {
			#"uri": "https://arweave.net/WhyRt90kgI7f0EG9GPfB8TIBTIBgX3X12QaF9ObFerE",
			#"name": "Test Nft MPL Core %d" % asset_signers.index(i),
			#"collection": collection_with_mints["group"],
			#"owner": beneficiary
		#}).send_and_confirm(umi, options)
		#collection_with_mints["mints"].append(i.public_key)

	#return collection_with_mints

## Mint MPL TM Collection
#func mint_mpl_tm_collection(umi: Umi, item_count: int, beneficiary: String, group: String = "", options: Dictionary = {"confirm": {"commitment": "finalized"}}):
	#if item_count <= 0:
		#return null
#
	#var asset_signers = []
	#for i in range(item_count):
		#asset_signers.append(umi.generate_signer())
#
	#var collection_with_mints = {"group": group, "mints": [], "asset": "MPL_TM"}
	#if group == "" and asset_signers.size() > 0:
		#var collection_signer = umi.generate_signer()
		#umi.create_nft(collection_signer, {
			#"name": "My Collection",
			#"uri": "https://example.com/my-collection.json",
			#"seller_fee_basis_points": 55, # 5.5%
			#"is_collection": true
		#}).send_and_confirm(umi, options)
		#collection_with_mints["group"] = collection_signer.public_key
#
	#for i in asset_signers:
		#umi.create_programmable_nft(i, {
			#"uri": "https://arweave.net/WhyRt90kgI7f0EG9GPfB8TIBTIBgX3X12QaF9ObFerE",
			#"name": "Test Nft MPL TM %d" % asset_signers.index(i),
			#"collection": {"key": collection_with_mints["group"], "verified": false},
			#"seller_fee_basis_points": 55,
			#"token_owner": beneficiary
		#}).add(umi.verify_collection_v1(collection_with_mints["group"], umi.find_metadata_pda(i.public_key))).send_and_confirm(umi)
		#collection_with_mints["mints"].append(i.public_key)
#
	#return collection_with_mints
#
## Mint MPL BG Collection
#func mint_mpl_bg_collection(connection: Connection, admin_keypair: Keypair, item_count: int, collection_mint: String, beneficiary: String, group: String = "", options: Dictionary = {"confirm": {"commitment": "finalized"}}):
	#if item_count <= 0:
		#return null
#
	#var collection_with_mints = {"group": group, "mints": [], "asset": "MPL_BG"}
	#if group == "" and item_count > 0:
		#collection_with_mints["group"] = connection.create_new_tree(admin_keypair)[0]
#
	#for i in range(item_count):
		#connection.mint_one_cnft(admin_keypair, {
			#"drop_wallet_key": beneficiary,
			#"name": "cNFT #%d" % i,
			#"symbol": "cNFT",
			#"uri": "https://arweave.net/WhyRt90kgI7f0EG9GPfB8TIBTIBgX3X12QaF9ObFerE",
			#"collection_mint": collection_mint,
			#"merkle_tree": collection_with_mints["group"]
		#})
#
	#collection_with_mints["mints"] = connection.fetch_helius_assets({
		#"wallet_address": beneficiary,
		#"collection_address": collection_mint
	#})
	##.filter(lambdaasset: asset.has("compression"))
	#return collection_with_mints
#
## Mint Token 2022 Collection
#func mint_token2022_collection(connection: Connection, admin_keypair: Keypair, item_count: int, beneficiary: String, group: Dictionary = null, options: Dictionary = {"confirm": {"commitment": "finalized"}}):
	#if item_count <= 0:
		#return null
#
	#if group == null:
		#group = connection.create_2022_group({
			#"name": "Extensions Group",
			#"symbol": "Extensions",
			#"uri": "https://example.com/my-collection.json",
			#"max_size": item_count
		#}, admin_keypair)
#
	#var collection_with_mints = {"group": group["group_address"], "mints": [], "asset": "TOKEN_2022"}
	#for i in range(item_count):
		#var mint = connection.mint_one_2022_nft({
			#"name": "Extensions #%d" % i,
			#"symbol": "Extensions",
			#"uri": "https://arweave.net/WhyRt90kgI7f0EG9GPfB8TIBTIBgX3X12QaF9ObFerE"
		#}, beneficiary, admin_keypair, group, options)
		#collection_with_mints["mints"].append(mint)
#
	#return collection_with_mints

# Mint assets function
# func mint_assets(count: Dictionary, beneficiery: Pubkey, collection: Pubkey = null,tree : Pubkey = null):
  

#func mint_assets(count: Dictionary, beneficiery: Pubkey, collection: Pubkey = null, core_collection: Pubkey = null, tree: Pubkey = null):
#func mint_assets():
	#var response = {
		#"core": null,
		#"pnfts": null,
		#"cnfts": null,
		#"token22": null
	#}

	# Execute minting tasks in parallel and handle errors
	#response.core = mint_mpl_core_collection(count.get("core"), beneficiery, core_collection)
	#await initialize_mint_collection()
	#await initialize_candy_machine()
	
	#response.pnfts = mint_mpl_tm_collection(count.get("pnfts"), beneficiery, collection).catch(resist_error)
	#response.token22 = mint_token2022_collection(connection, admin_keypair, count.get("token22"), beneficiery, null).catch(resist_error)

	# Check if pnfts group or collection exists, and mint cnfts if applicable
	#var c = response.pnfts and response.pnfts.get("group") or collection
	#if c:
		#response.cnfts = mint_mpl_bg_collection(connection, admin_keypair, count.get("cnfts"), c, beneficiery).catch(resist_error)
#
	#return response
	

	#response.core = yield (mint_mpl_core_collection(umi, count.get("core", 0), beneficiary, core_collection, options), "completed") or resist_error()
	#response.pnfts = yield (mint_mpl_tm_collection(umi, count.get("pnfts", 0), beneficiary, collection, options), "completed") or resist_error()
	#response.token22 = yield (mint_token2022_collection(connection, admin_keypair, count.get("token22", 0), beneficiary, null, options), "completed") or resist_error()

	#var c = response.pnfts and response.pnfts.group or collection
	#if c:
		#response.cnfts = yield (mint_mpl_bg_collection(connection, admin_keypair, count.get("cnfts", 0), c, beneficiary), "completed") or resist_error()

	#return response


	# Function to handle errors and return null
#func resist_error(err):
	## print("Error: ", err)
	#push_error("Failed in assets: %s" % err)
	#return null




#func initialize_mint_collection():
	#var payer: Keypair = utils.user_keypair
	## Ask our RPC server how much we need to transfer to our new mint account.
	#var mint_lamports = await minumum_balance_for_rent_extemtion(MINT_SIZE)
	#var mint_account_lamports = await minumum_balance_for_rent_extemtion(MINT_ACCOUNT_SIZE)
	#
	## var token_account_keypair = Keypair.new_random()
#
	#
	## Create a transaction
	#var tx := Transaction.new()
	#add_child(tx)
	#
	## Append instructions. 
	#var ix: Instruction = SystemProgram.create_account(payer, mint_keypair, mint_lamports, MINT_SIZE, TokenProgram.get_pid())
	#tx.add_instruction(ix)
	#ix = TokenProgram.initialize_mint(mint_keypair, payer, payer, 0)
	#tx.add_instruction(ix)
	#ix = SystemProgram.create_account(payer, mint_account_keypair, mint_account_lamports, MINT_ACCOUNT_SIZE, TokenProgram.get_pid())
	#tx.add_instruction(ix)
	#ix = TokenProgram.initialize_account(mint_account_keypair, mint_keypair, payer)
	#tx.add_instruction(ix)
	#ix = TokenProgram.mint_to(mint_keypair, mint_account_keypair, payer, payer, 1)
	#tx.add_instruction(ix)
	#ix = MplTokenMetadata.create_metadata_account(mint_keypair, payer, payer, load("res://resources/new_create_meta_data_args.tres"),true)
	#tx.add_instruction(ix)
	#ix = MplTokenMetadata.update_metadata_account(MplTokenMetadata.new_associated_metadata_pubkey(mint_keypair), payer)
	#tx.add_instruction(ix)
	#ix = MplTokenMetadata.create_master_edition(mint_keypair, payer, payer, payer, 0)
	#tx.add_instruction(ix)
	#
	#tx.set_payer(payer)
	#
	#tx.update_latest_blockhash()
#
	#tx.sign_and_send()
	#var result = await tx.transaction_response_received
	#print("result: ",result)
	#assert(result.has("result"))
	#
	#utils.print_transaction_url(result.result)
#
	#await tx.confirmed
#
#
#func minumum_balance_for_rent_extemtion(data_size):
	#var solana_client = SolanaClient.new()
	#add_child(solana_client)
	#solana_client.get_minimum_balance_for_rent_extemption(data_size)
	#var result = (await solana_client.http_request_completed)[1]
	#assert(result.has("result"))
	#remove_child(solana_client)
	#return result['result']
#
#
#
#
#
#func initialize_candy_machine():
	#print("pohonch gaya....")
	#var payer: Keypair = utils.user_keypair
	##var candy_machine_size = load("res://resources/new_candy_machine_data.tres").get_space_for_candy()
	#
	## Create a transaction
	#var tx := Transaction.new()
	#add_child(tx)
	#
	#var info: CandyMachineData = load("res://resources/new_candy_machine_data.tres")
	##info.token_standard = 0
	#info.items_available = 1
	#info.seller_fee_basis_points = 800
	#info.symbol = "GSS"
	##info.creators = [payer]
	#info.config_line_setting = load("res://resources/new_config_line_setting.tres")
	#var candy_machine_size = info.get_space_for_candy()
	#var candy_machine_lamports = await minumum_balance_for_rent_extemtion(candy_machine_size)
	#var pid = MplCandyMachine.get_pid()
	#print(pid)
	#print(MplTokenMetadata.get_pid())
	#var ix: Instruction = SystemProgram.create_account(payer, candy_machine_keypair, candy_machine_lamports, candy_machine_size, pid)
	#
	#tx.add_instruction(ix)
	#ix = MplCandyMachine.initialize(payer, candy_machine_keypair, mint_keypair, info, true)
	#tx.add_instruction(ix)
	#
	#tx.set_payer(payer)
	#tx.update_latest_blockhash()
#
	#tx.sign_and_send()
	#var result = await tx.transaction_response_received
	#print("result: ",result)
	#assert(result.has("result"))
	#
	#utils.print_transaction_url(result.result)
#
#
	#await tx.confirmed
