extends Control


const TOTAL_CASES := 2
var passed_test_mask := 0
		

func PASS(unique_identifier: int):
	passed_test_mask += (1 << unique_identifier)
	print("[OK]: ", unique_identifier)

	
func wallet_adapter_sign(wallet_type, test_id):
	$WalletAdapter.set_wallet_type(wallet_type)
	$WalletAdapter.connect_wallet()
	await $WalletAdapter.connection_established
	
	$Transaction.set_payer($WalletAdapter)
	
	var ix = SystemProgram.transfer($WalletAdapter, Pubkey.new_random(), 1000000)
	$Transaction.add_instruction(ix)
	$Transaction.update_latest_blockhash()
	
	
	$Transaction.sign()
	await $Transaction.fully_signed
	print($Transaction.serialize())
	var signature : PackedByteArray = $Transaction.serialize().slice(1, 65)
	var empty_signature : PackedByteArray
	empty_signature.resize(64)
	assert(signature != empty_signature)
	$Label.text += str(signature)
	
	$Transaction.send()
	var response = await $Transaction.transaction_response_received
	
	# Wallets may add instructions for programs we don't have locally.
	# So we just make sure the error is not due to transaciton signature failure.
	if response.has("error"):
		var error_message = response["error"]["message"]
		assert(error_message.find("signature verification") == -1)
	
	PASS(test_id)


func _ready():
	await wallet_adapter_sign(20, 0)
	await wallet_adapter_sign(36, 1)


func _on_timeout_timeout():
	var all_tests_passed = true
	for i in range(TOTAL_CASES):
		if ((1 << i) & passed_test_mask) == 0:
			all_tests_passed = false
			print("[FAIL]: ", i)
			
	if all_tests_passed:
		print("[ALL TESTS PASSED]")
