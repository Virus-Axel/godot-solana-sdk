extends VBoxContainer


func PASS(unique_identifier: int):
	print("[OK]: ", unique_identifier)


func random_keypair_demo():
	var random_keypair: Keypair = Keypair.new_random()
	var pk_bytes: PackedByteArray = random_keypair.get_public_bytes()
	var sk_bytes: PackedByteArray = random_keypair.get_private_bytes()
	var pk_string: String = random_keypair.get_public_string()
	
	var another_random_keypair: Keypair = Keypair.new_random()
	
	assert(pk_bytes.size() == 32)
	assert(sk_bytes.size() == 64)
	assert(not pk_string.is_empty())
	assert(another_random_keypair != random_keypair)
	
	$RandomKey.text += pk_string
	PASS(0)


func keypair_from_seed_demo():
	var empty_seed = PackedByteArray()
	empty_seed.resize(32)
	var seed_kp: Keypair = Keypair.new_from_seed(empty_seed)
	var compare_seed_kp: Keypair = Keypair.new_from_seed(empty_seed)

	assert(seed_kp.get_public_bytes() == compare_seed_kp.get_public_bytes())
	assert(seed_kp.get_public_string() == "4zvwRjXUKGfvwnParsHAS3HuSVzV5cA4McphgmoCtajS")
	
	$SeededKey.text += seed_kp.get_public_string()
	PASS(1)


func keypair_from_file_demo():
	var random_keypair: Keypair = Keypair.new_random()
	
	var compare_bytes: PackedByteArray = random_keypair.get_public_bytes()
	
	random_keypair.save_to_file("keypair.json")
	var load_keypair = Keypair.new_from_file("keypair.json")
	
	assert(compare_bytes == load_keypair.get_public_bytes())
	
	$LoadedKey.text += load_keypair.get_public_string()
	PASS(2)


func keypair_from_bytes():
	# This is the string you get when showing private key in Phantom wallet.
	var kp = Keypair.new_from_bytes("3wUbDHMtMVQSWLprioM4RKdKdJH5d4B6Sjah3JSsFVz7AUxWnSExLtrePtocxhYzvGziCLo42kX742yo9LZLt6Zd")
	
	# Copied from a dummy phantom wallet.
	const DUMMY_PHANTOM_PUBKEY := "CzaehrBFzMUfxv7rmiExFtKSLNSeq9UYoyTR8Wz9qSvT"
	assert(kp.get_public_string() == DUMMY_PHANTOM_PUBKEY)
	PASS(3)


func sign_and_verify_message_demo():
	var kp: Keypair = Keypair.new_random()
	var message: PackedByteArray = String("message to sign/verify").to_ascii_buffer()
	
	assert(kp.verify_signature(kp.sign_message(message), message))
	PASS(4)


func keypair_to_pubkey():
	var kp: Keypair = Keypair.new_random()
	var pk: Pubkey = kp.to_pubkey()
	
	assert(kp.get_public_bytes() == pk.to_bytes())
	PASS(5)


func _ready():
	random_keypair_demo()
	keypair_from_seed_demo()
	keypair_from_file_demo()
	keypair_from_bytes()
	sign_and_verify_message_demo()
	keypair_to_pubkey()

	print("ALL TESTS PASSED")
	get_tree().quit(0)
