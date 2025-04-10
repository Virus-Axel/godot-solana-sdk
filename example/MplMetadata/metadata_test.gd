extends Control

const TOTAL_CASES := 4
var passed_test_mask := 0

const MINT_SIZE := 82
var mint_keypair = Keypair.new_random()
var payer : Keypair = Keypair.new_from_file("payer.json")


func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)


func minumum_balance_for_rent_extemtion(data_size):
	var client = SolanaClient.new()
	add_child(client)
	client.get_minimum_balance_for_rent_extemption(data_size)
	var result = await client.http_response_received
	assert(result.has("result"))
	remove_child(client)
	return result['result']


func test_create_metadata():
	var mint_lamports = await minumum_balance_for_rent_extemtion(MINT_SIZE)

	var meta_data = load("res://create_meta_data_args.tres")
	var tx : Transaction = Transaction.new()
	add_child(tx)
	
	tx.set_payer(payer)
	var ix: Instruction = SystemProgram.create_account(payer, mint_keypair, mint_lamports, MINT_SIZE, TokenProgram.get_pid())
	tx.add_instruction(ix)
	ix = TokenProgram.initialize_mint(mint_keypair, payer, payer, 0)
	tx.add_instruction(ix)
	ix = MplTokenMetadata.create_metadata_account(mint_keypair, payer, payer, meta_data, true)
	tx.add_instruction(ix)
	
	tx.update_latest_blockhash()
	
	tx.sign()
	tx.send()
	var response = await tx.transaction_response_received
	assert(response.has("result"))
	await tx.confirmed
	PASS(0)


func test_fetch_metadata():
	$MplTokenMetadata.get_mint_metadata(mint_keypair)
	var metadata = await $MplTokenMetadata.metadata_fetched
	assert(metadata.get_class() == "MetaData")
	
	var expected_metadata = load("res://create_meta_data_args.tres")
	
	assert(metadata.get_token_name() == expected_metadata.get_token_name())
	assert(metadata.get_symbol() == expected_metadata.get_symbol())
	assert(metadata.get_uri() == expected_metadata.get_uri())
	assert(metadata.get_seller_fee_basis_points() == expected_metadata.get_seller_fee_basis_points())
	
	var creators : Array = metadata.get_creators()
	var expected_creators : Array = expected_metadata.get_creators()

	for i in range(creators.size()):
		assert(creators[i].get_address().to_string() == expected_creators[i].get_address().to_string())
		assert(creators[i].get_verified() == expected_creators[i].get_verified())
		assert(creators[i].get_share() == expected_creators[i].get_share())
	
	assert(metadata.get_token_standard() == 1)
	assert(metadata.get_mint().to_string() == mint_keypair.get_public_string())
	assert(metadata.get_update_authority().to_string() == payer.get_public_string())
	PASS(1)


func fetch_problematic_metadatas():
	var problematic_nfts = [
		"5duyy3TK4BKqFgkDKTLxBbEhvGUYw7MtiXjqn3HYu8B",
		"DRoM1mVPrCQzd4TmMZtS5oKJwbfdMnjumuNBn6nPCCwK",
		"6xcLZd5eqQWqt292bJxkBanoxKMjPSc8KomayQ4j2M8m",
	]
	
	for problem_nft in problematic_nfts:
		$MplTokenMetadata.get_mint_metadata(mint_keypair)
		var metadata = await $MplTokenMetadata.metadata_fetched

	PASS(3)


func test_fetch_usdc_metadata():
	var usdc_address = Pubkey.new_from_string("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v")
	$MplTokenMetadata.get_mint_metadata(usdc_address)
	var metadata = await $MplTokenMetadata.metadata_fetched
	assert(metadata.get_class() == "MetaData")
	
	assert(metadata.get_token_name() == "USD Coin")
	assert(metadata.get_symbol() == "USDC")
	assert(metadata.get_uri() == "")
	assert(metadata.get_seller_fee_basis_points() == 0)
	
	assert(metadata.get_creators().size() == 0)
	assert(metadata.get_token_standard() == null)
	assert(metadata.get_mint().to_string() == usdc_address.to_string())
	assert(metadata.get_update_authority().to_string() == "2wmVCSfPxGPjrnMMn7rchp4uaeoTqN39mXFC2zhPdri9")
	assert(metadata.get_is_mutable() == true)
	assert(metadata.get_edition_nonce() == 252)

	PASS(2)


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	await test_create_metadata()
	await test_fetch_metadata()
	await test_fetch_usdc_metadata()
	await fetch_problematic_metadatas()


func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
