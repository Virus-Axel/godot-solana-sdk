extends Node
class_name AccountInspector

enum InspectSite {SOLSCAN,EXPLORER,SOLANAFM}

func get_tx_link(tx_id:String,inspect_site:InspectSite=InspectSite.SOLSCAN) -> String:
	var link = get_link_base(inspect_site)
	link += "tx/"
	link += tx_id
	link += get_cluster_extension(inspect_site)
	return link

func get_account_link(key_to_check:Pubkey,inspect_site:InspectSite=InspectSite.SOLSCAN) -> String:
	var link = get_link_base(inspect_site)
	link += "address/"
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
		InspectSite.SOLANAFM:
			base = "https://solana.fm/"
	return base
			
func get_cluster_extension(inspect_site:InspectSite) -> String:
	var extension:String
	if SolanaService.rpc_cluster == SolanaService.RpcCluster.DEVNET:
		extension = "?cluster=devnet"
		if inspect_site == InspectSite.SOLANAFM:
			extension += "-alpha"
			
	return extension
	
