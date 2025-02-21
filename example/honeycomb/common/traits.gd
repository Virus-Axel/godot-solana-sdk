extends Node

# Trait configuration
var config = {
	"Clothes": {
		"Astronaut": "https://arweave.net/cpU6oOV7Bf1MM8VDKWp9tYMl0Zq7QB3Axsa5goZYtGg",
		"Bandit": "https://arweave.net/NcGmsL90_eOYn4Hq70cFKXFQIgbp0A7yocxFNin-HmU",
		"Bomber Jacket": "https://arweave.net/lvdH6BK8nB5qC9baAb7SeGetay-dYWsOGw6vxAPNick"
	},
	"Eyes": {
		"2 Tone": "https://arweave.net/ovy1LAnBgqoKmxPgoXU1g6ZZuXPA_AakIvmcqGMuO2I",
		"Blue Eyes": "https://arweave.net/dtzO9PhSRITbxwRJ32hlS_lcHOvlr04zQ98DdPFy0gY",
		"Cheerful": "https://arweave.net/m-cSGIhwGyJWCR45LaJduXJa2KsYZJ4omIRdmKmb69E",
		"Evil": "https://arweave.net/jhhwkUHtupJ8KMlIQa0rVfPOL5VL0xdUFhEsHSWb8Vo"
	},
	"Fur": {
		"Black": "https://arweave.net/R8yJGtBtMOFadXr-9mqt5Oo1J3UddprhvXFimrixsVk",
		"Brown": "https://arweave.net/CxmNiIFsNs1v9Vq_DwawaJUtmacOWwUadZsC4D8vYVQ",
		"Dark Grey": "https://arweave.net/mQgF05MYus1dTf29AzAty0LgUsT4QHo3DW8kcuGOigE"
	},
	"Mouth": {
		"Agitated": "https://arweave.net/F1vK1k-T2_xw-O1wX6egsyaqMYDnEZZZmAIfre8sPTs",
		"Contented": "https://arweave.net/jjaabb-OpV-XsVhzVv4CMZ3ys4hzD35EpZsrWn8FyjM",
		"Mad": "https://arweave.net/kCv7UXSQhryWpgq3yQkV2w_uEAYPq0TKtUWwhYTZS8g"
	}
}

# Function to transform traits data
func transform_traits_data(input_data: Dictionary) -> Array:
	var result = []
	
	for label in input_data.keys():
		var items = input_data[label]
		
		for name_str in items.keys():
			result.append({
				"layer": label,
				"name": name_str,
				"uri": items[name_str]
			})
	
	return result

# Define trait order
var order: PackedStringArray = PackedStringArray(["Fur", "Eyes", "Mouth", "Clothes"])

# Store transformed trait data
var traits = transform_traits_data(config)
