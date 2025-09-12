# Example usage of Pubkeys, by Virus-Axel

extends ItemList


func PASS(unique_identifier: int):
	print("[OK]: ", unique_identifier)


func pubkey_demo_from_string():
	# Construct a Pubkey from a base58 encoded text string.
	var pk: Pubkey = Pubkey.new_from_string("4n8Pxz55FsvArzjjX7idYcvfyBaZtu2Qmh86AfCVLiJU")
	assert(!pk.to_string().is_empty())
	set_item_text(1, pk.to_string())
	PASS(0)


func pubkey_demo_from_bytes():
	# Construct a Pubkey from a byte array of length 32.
	var empty_bytes := PackedByteArray()
	empty_bytes.resize(32)
	var pk: Pubkey = Pubkey.new_from_bytes(empty_bytes)
	assert(!pk.to_string().is_empty())
	set_item_text(3, pk.to_string())
	PASS(1)


func pubkey_demo_metadata_program():
	# Get program ID from Metaplex token metadata program.
	var pk: Pubkey = MplTokenMetadata.get_pid()
	assert(!pk.to_string().is_empty())
	set_item_text(5, pk.to_string())
	PASS(2)


func pubkey_demo_token_program():
	# Common Programs have their own Node representation.
	# Use them to get program ID and other common operations.
	var pk: Pubkey = TokenProgram.get_pid()
	assert(!pk.to_string().is_empty())
	set_item_text(7, pk.to_string())
	PASS(3)


func pubkey_demo_candy_machine():
	# Get program ID from Metaplex CandyMachine program.
	var pk: Pubkey = MplCandyMachine.get_pid()
	assert(!pk.to_string().is_empty())
	set_item_text(9, pk.to_string())
	PASS(4)


func pubkey_demo_pubkey_bytes():
	# Get byte representation of any Pubkey with to_bytes().
	# In this case we use CandyMachine program ID.
	var pk: Pubkey = MplCandyMachine.get_pid()
	var byte_array: PackedByteArray = pk.to_bytes()
	assert(!pk.to_string().is_empty())
	set_item_text(11, "{0}".format([byte_array]))
	PASS(5)


func pubkey_demo_associated_token():
	# Create an associated token address with an owner and a mint address.
	var token_owner: Keypair = Keypair.new_random()
	var mint = Pubkey.new_from_string("2WLPJWkNGVrM3GVJ1KeeFFBqEDStKCaA34sPPvQFz4VB");
	var pk: Pubkey = Pubkey.new_associated_token_address(token_owner, mint, TokenProgram.get_pid())
	
	assert(!pk.to_string().is_empty())
	set_item_text(13, pk.to_string())
	PASS(6)


func pubkey_demo_program_derived_address():
	# Find a program derrived address of a program (System Program in this case),
	# and an array of seeds.
	var program_id: Pubkey = Pubkey.new_from_string("11111111111111111111111111111111")
	var pk: Pubkey = Pubkey.new_pda(["Your", "seeds", "here"], program_id)
	assert(!pk.to_string().is_empty())
	set_item_text(15, pk.to_string())
	PASS(7)


func _ready():
	pubkey_demo_from_string()
	pubkey_demo_from_bytes()
	pubkey_demo_metadata_program()
	pubkey_demo_token_program()
	pubkey_demo_candy_machine()
	pubkey_demo_pubkey_bytes()
	pubkey_demo_associated_token()
	pubkey_demo_program_derived_address()
	
	print("ALL TESTS PASSED")
	get_tree().quit(0)
