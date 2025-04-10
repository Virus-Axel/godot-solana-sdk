extends OptionButton


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var curr_cluster = SolanaService.rpc_cluster
	match curr_cluster:
		SolanaService.RpcCluster.MAINNET:
			select(0)
		SolanaService.RpcCluster.DEVNET:
			select(1)
			
	item_selected.connect(on_network_selected)
	pass # Replace with function body.


func on_network_selected(selection:int) -> void:
	var rpc_cluster
	match selection:
		0:
			rpc_cluster = SolanaService.RpcCluster.MAINNET
		1:
			rpc_cluster = SolanaService.RpcCluster.DEVNET
	SolanaService.set_rpc_cluster(rpc_cluster)
