extends Node
class_name JupiterAPI

enum TokenStatus{VERIFIED, UNKNOWN}

var JUP_TOKEN_API:String = "https://tokens.jup.ag/token/"
var JUP_PRICE_API:String = "https://price.jup.ag/v6/price?"
var JUP_QUOTE_API:String = "https://quote-api.jup.ag/v6/quote?"
var JUP_SWAP_API:String = "https://quote-api.jup.ag/v6/swap"


func _ready() -> void:
	#var token_status:TokenStatus = await get_token_status(Pubkey.new_from_string("Ds52CDgqdWbTWsua1hgT3AuSSy4FNx2Ezge1br3jQ14a"))
	#var token_price:float =  await get_token_unit_price(Pubkey.new_from_string("Ds52CDgqdWbTWsua1hgT3AuSSy4FNx2Ezge1br3jQ14a"),Pubkey.new_from_string("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"))
	#var response = await get_swap_quote(Pubkey.new_from_string("So11111111111111111111111111111111111111112"),
	#Pubkey.new_from_string("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"),
	#0.02,1.0)
	#print(response)
	#var tx:TransactionData = await swap_token(SolanaService.wallet.get_pubkey(),response)
	#print(tx)
	pass
	
func get_swap_quote(token_to_send:Pubkey,token_to_receive:Pubkey,amount_to_send:float,slippage_percentage:float,fee_account:Pubkey=null,fee_percentage:float=0):
	var input_mint:String = "inputMint="+token_to_send.to_string()
	var mint_data:Dictionary = await get_token_data(token_to_send)
	if mint_data.has("error"):
		return null
	
	var output_mint:String = "outputMint="+token_to_receive.to_string()
	
	var amount_in_decimals:float = amount_to_send*pow(10,mint_data["decimals"])
	var amount:String = "amount="+str(amount_in_decimals)
	var slippage:String = "slippageBps="+str(percentage_to_bps(slippage_percentage))

	var quote_slug:String = "%s&%s&%s&%s" % [input_mint,output_mint,amount,slippage]
	var transfer_quote:Dictionary = await HttpRequestHandler.send_get_request(JUP_QUOTE_API+quote_slug)
	return transfer_quote["body"]
	
func swap_token(payer:Pubkey,swap_quote:Dictionary) -> TransactionData:
	var headers:Array = ["Content-type: application/json"]
	var body:Dictionary = {
		"quoteResponse":swap_quote,
		"userPublicKey":payer.to_string(),
		"wrapAndUnwrapSol":true
	}
	var response:Dictionary = await HttpRequestHandler.send_post_request(JSON.stringify(body),headers,JUP_SWAP_API)
	var serialized_tx_data:PackedByteArray = SolanaUtils.bs64_decode(response["body"]["swapTransaction"])
	var priority_fee:float = response["body"]["prioritizationFeeLamports"]
	#return null
	var transaction:Transaction = await SolanaService.transaction_manager.sign_transaction_serialized(serialized_tx_data,SolanaService.wallet.get_kp(),[SolanaService.wallet.get_kp()])
	var tx_data:TransactionData = await SolanaService.transaction_manager.send_transaction(transaction)
	return tx_data

func get_token_data(token_mint:Pubkey) -> Dictionary:
	var response:Dictionary = await HttpRequestHandler.send_get_request(JUP_TOKEN_API+token_mint.to_string())
	if response.has("error"):
		response
	return response["body"]
	
func get_token_status(token_mint:Pubkey) -> TokenStatus:
	var response:Dictionary = await get_token_data(token_mint)
	if response.has("error"):
		return TokenStatus.UNKNOWN
		
	if "verified" in response["tags"] or "strict" in response["tags"]:
		return TokenStatus.VERIFIED
	else:
		return TokenStatus.UNKNOWN
		
func get_token_unit_price(token_mint:Pubkey,price_against:Pubkey=null) -> float:
	var ids:String="ids="+token_mint.to_string()
	var vs_token:String = ""
	if price_against!=null:
		vs_token = "&vsToken="+price_against.to_string()
		
	var response:Dictionary = await HttpRequestHandler.send_get_request(JUP_PRICE_API+ids+vs_token)
	if response.has("error"):
		return 0
	return response["body"]["data"][token_mint.to_string()]["price"]

	
func percentage_to_bps(value:float) -> int:
	return roundi(value*10)
