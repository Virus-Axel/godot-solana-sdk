extends WalletAsset
class_name Token

var decimals:int
var lamport_balance:int

func get_balance() -> float:
	return float(lamport_balance)/(10**decimals)
	
