#solana sdk plugin
@tool
extends EditorPlugin

var solana_service_path:String = "res://addons/SolanaSDK/Autoload/SolanaService.tscn"

func _enter_tree():
	# This method is called when the editor starts
	
	var autoload_script_paths:Array[String]
	autoload_script_paths.append(solana_service_path)
	
	for script in autoload_script_paths:
		var script_parts:PackedStringArray = script.split("/")
		var script_name:String = script_parts[script_parts.size()-1].split(".")[0]
		# Check if the script is not already in the autoload list
		if !ProjectSettings.has_setting("autoload/%s"%script_name):
			# Add the script to the autoload list
			add_autoload_singleton(script_name,script)
			# Save the changes to the Project Settings
			ProjectSettings.save()
