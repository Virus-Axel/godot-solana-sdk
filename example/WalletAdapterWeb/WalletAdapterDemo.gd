extends Control


const TOTAL_CASES := 1
var passed_test_mask := 0
		

func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)

	
func wallet_adapter_sign():
	$WalletAdapter.connect_wallet()
	await $WalletAdapter.connection_established
	
	$Transaction.set_payer($WalletAdapter)
	
	var ix = SystemProgram.transfer($WalletAdapter, Pubkey.new_random(), 1000000)
	$Transaction.add_instruction(ix)
	$Transaction.update_latest_blockhash()
	
	$Transaction.sign()
	await $Transaction.fully_signed
	var signature : PackedByteArray = $Transaction.serialize().slice(1, 65)
	var empty_signature : PackedByteArray
	empty_signature.resize(64)
	assert(signature != empty_signature)
	$Label.text += str(signature)
	
	PASS(0)


func _ready():
	await wallet_adapter_sign()


func _on_timeout_timeout():
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			print("[FAIL]: ", i)
