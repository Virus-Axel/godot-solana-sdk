extends Resource
class_name NFTCollection

@export var mainnet_collection_id:String
@export var devnet_collection_id:String

@export var first_verified_creator_mode:bool
@export var collection_name:String

var nfts:Array[Nft]

var collection_mint:Pubkey

func set_key()-> void:
	if SolanaService.rpc_cluster == SolanaService.RpcCluster.Mainnet:
		collection_mint = Pubkey.new_from_string(mainnet_collection_id)
	elif SolanaService.rpc_cluster == SolanaService.RpcCluster.Devnet:
		collection_mint = Pubkey.new_from_string(devnet_collection_id)

func get_collection_key() -> Pubkey:
	if collection_mint==null:
		set_key()
	
	return collection_mint	
		

func belongs_to_collection(nft:Nft) -> bool:
	if collection_mint==null:
		set_key()
		
	if first_verified_creator_mode:
#		for creator in nft.metadata.get_creators():
#			print("creator: ",creator.address.to_string())
		if nft.metadata.get_creators().size() == 0:
			return false
		return nft.metadata.get_creators()[0].address.to_string() == collection_mint.to_string()
	else:
		if nft.metadata.get_collection() == null:
			return false
		return nft.metadata.get_collection().key.to_string() == collection_mint.to_string()
