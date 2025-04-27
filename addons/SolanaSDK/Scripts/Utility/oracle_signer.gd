extends Node
class_name OracleSigner
#If your server is with TS, use the following code there:
#In a nutshell, you send a serialized transaction, and in server you deserialize
#then sign with a keypair which you hold in environmental value
#the serialize again and send back to godot, where it can be processed further
#EASY MODE

#const privateKey = new Uint8Array(JSON.parse(process.env.PRIVATE_KEY as string))
#let keypair = anchor.web3.Keypair.fromSecretKey(privateKey);

#const connection = new anchor.web3.Connection(process.env.RPC_URL || "https://api.devnet.solana.com");

#const txBytes = createTransactionBytes(transaction); 
#const buffer = Buffer.from(txBytes);
#const tx = Transaction.from(buffer);

#const signature = nacl.sign.detached(tx.serializeMessage(), keypair.secretKey);
#tx.addSignature(keypair.publicKey, Buffer.from(signature));
#
#const serializedTransaction = tx.serialize({
	#requireAllSignatures: false,
	#});
	
	#res.status(200).json({
		#message: "SUCCESS",
		#transaction: serializedTransaction,
	#});
#} catch (error) {
	#console.log(error);
	#res.status(500).json({
		#message: error instanceof Error ? error.message : "ERROR",
	#});
#}

@export var server_link:String
@export var link_slug:String
@export var use_localhost:bool=false
@export var localhost_port:int = 5000

@export var default_signer_address:String

func _ready() -> void:
	if use_localhost:
		server_link = "http://localhost:%s/" % str(localhost_port)
		

func add_oracle_signature(transaction:Transaction) -> PackedByteArray:
	if server_link == "":
		push_error("server link not provided!")
		return []
		
	var serialized_tx:PackedByteArray = transaction.serialize()
	var headers:Array = ["Content-type: application/json"]
	var body:Dictionary = {
		"transaction":serialized_tx
	}
	var response:Dictionary = await HttpRequestHandler.send_post_request(JSON.stringify(body),headers,server_link+link_slug)
	if response.has("error"):
		push_error("failed to receive signature by the server. ",response)
		return []
		
	return response["body"]["transaction"]["data"]


func get_signer_key() -> Pubkey:
	if default_signer_address=="":
		return null
	return Pubkey.new_from_string(default_signer_address)
