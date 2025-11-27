extends Node
class_name AccountInspector

enum InspectSite {SOLSCAN,EXPLORER,ORB, TENSOR, DEXSCREENER}

func get_tx_link(tx_id:String,inspect_site:InspectSite=InspectSite.ORB) -> String:
	var link = get_link_base(inspect_site)
	link += "tx/"
	link += tx_id
	link += get_cluster_extension(inspect_site)
	return link

func get_account_link(key_to_check:Pubkey,inspect_site:InspectSite=InspectSite.ORB) -> String:
	var link = get_link_base(inspect_site)
	link += "address/"
	link += key_to_check.to_string()
	link += get_cluster_extension(inspect_site)
	return link
	
func get_inspect_link(key_to_check:Pubkey,inspect_site:InspectSite=InspectSite.DEXSCREENER) -> String:
	var link = get_link_base(inspect_site)
	link += key_to_check.to_string()
	link += get_cluster_extension(inspect_site)
	return link
	
func get_link_base(inspect_site:InspectSite) -> String:
	var base:String
	match inspect_site:
		InspectSite.SOLSCAN:
			base = "https://solscan.io/"
		InspectSite.EXPLORER:
			base = "https://explorer.solana.com/"
		InspectSite.ORB:
			base = "https://orb.helius.dev/"
		InspectSite.TENSOR:
			base = "https://tensor.trade/trade/"
		InspectSite.DEXSCREENER:
			base = "https://dexscreener.com/solana/"
	return base
			
func get_cluster_extension(inspect_site:InspectSite) -> String:
	var extension:String
	if SolanaService.rpc_cluster == SolanaService.RpcCluster.MAINNET:
		extension = ""
	elif SolanaService.rpc_cluster == SolanaService.RpcCluster.DEVNET:
		extension = "?cluster=devnet"
	else:
		extension = "?cluster=custom&customUrl="
		if SolanaService.rpc_cluster == SolanaService.RpcCluster.MAGICBLOCK:
			extension += SolanaService.magicblock_rpc.uri_encode()
		elif SolanaService.rpc_cluster == SolanaService.RpcCluster.HONEYNET:
			extension += SolanaService.honeycomb_rpc.uri_encode()
		
	return extension
