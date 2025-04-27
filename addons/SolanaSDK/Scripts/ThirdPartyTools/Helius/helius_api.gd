extends Node
class_name HeliusAPI
#can add unsafeMax as well
enum PriorityFeeLevel {min,low,medium,high,veryHigh}

@export var helius_api_key:String
## this is important to know whether we can force a staked connection to send a transaction in case of congestion
@export var using_paid_plan:bool

@export var priority_fee_level:PriorityFeeLevel
@export var use_recommended_fee:bool = true
@export var fee_to_consider_congested:int = 10000
@export var override_das_rpc:bool=false

func _ready() -> void:
	if override_das_rpc:
		SolanaService.on_rpc_cluster_set.connect(override_default_das_rpc)
		
func override_default_das_rpc() -> void:
	SolanaService.das_compatible_rpc = get_rpc_url()

func get_rpc_url(staked:bool=false) -> String:
	if helius_api_key == "":
		print("Couldn't get a helius RPC because the API key is missing")	
		return ""
		
	var url_type:String = ""
	if staked:
		if using_paid_plan:
			url_type = "staked"
		else:
			return ""
	else:
		match SolanaService.rpc_cluster:
			SolanaService.RpcCluster.MAINNET:
				url_type = "mainnet"
			SolanaService.RpcCluster.DEVNET:
				url_type = "devnet"
	
	if url_type == "":
		print("Couldn't get a helius RPC from selected RPC cluster")	
		return ""
		
	var rpc_url:String = "https://%s.helius-rpc.com/?api-key=%s" % [url_type,helius_api_key]
	return rpc_url
	
func is_network_congested(calculated_fee:int) -> bool:
	return calculated_fee >= fee_to_consider_congested
	

func get_estimated_priority_fee(transaction:Transaction) -> int:
	var rpc_url = get_rpc_url()
	if rpc_url == "":
		return 0
		
	var options:Dictionary = {"evaluateEmptySlotAsZero":true, "includeDetails":true}
	if use_recommended_fee:
		options["recommended"] = true
	else:
		options["includeAllPriorityFeeLevels"] = true
		
	var serialized_tx:String = SolanaUtils.bs58_encode(transaction.serialize())
	var headers:Array = ["Content-type: application/json"]
	var body:Dictionary = {
		"jsonrpc": "2.0",
		"id": "1",
		"method": "getPriorityFeeEstimate",
		"params": [{
			"transaction":serialized_tx,
			"options": options,
		}]
	}
	var response:Dictionary = await HttpRequestHandler.send_post_request(JSON.stringify(body),headers,rpc_url)
	if response.size() == 0 or response["body"].has("error"):
		push_error(response)
		return 0
	
	var fee_estimate:int
	if use_recommended_fee:
		fee_estimate = int(response["body"]["result"]["priorityFeeEstimate"])
	else:
		var level:String = str(PriorityFeeLevel.keys()[priority_fee_level])
		fee_estimate = int(response["body"]["result"]["priorityFeeLevels"][level])
		
	return fee_estimate
