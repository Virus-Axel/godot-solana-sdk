@tool
extends VBoxContainer


func _on_check_box_toggled(toggled_on: bool) -> void:
	$OptionButton.visible = !toggled_on

func get_selected_address():
	if $CheckBox.button_pressed:
		var default_url = ProjectSettings.get_setting("solana_sdk/client/default_url")
		var parsed_url = SolanaClient.parse_url(default_url)
		parsed_url["port"] = ProjectSettings.get_setting("solana_sdk/client/default_http_port")
		
		return SolanaClient.assemble_url(parsed_url)
	
	else:
		if $OptionButton.selected == 0:
			return 	"https://api.devnet.solana.com:443"
		elif $OptionButton.selected == 1:
			return 	"https://api.mainnet-beta.solana.com:443"
		elif $OptionButton.selected == 2:
			return 	"https://api.testnet.solana.com:443"
	
	return ""
