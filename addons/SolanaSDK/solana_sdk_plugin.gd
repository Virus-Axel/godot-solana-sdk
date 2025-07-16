#solana sdk plugin
@tool
extends EditorPlugin

const SOLANA_PLUGIN_NAME := "SolanaSDK"

var solana_service_path:String = "res://addons/" + SOLANA_PLUGIN_NAME + "/Autoload/SolanaService.tscn"
var http_request_handler_path:String = "res://addons/" + SOLANA_PLUGIN_NAME + "/Autoload/HttpRequestHandler.tscn"
var export_plugin : AndroidExportPlugin


func _enter_tree():
	export_plugin = AndroidExportPlugin.new()
	add_export_plugin(export_plugin)

	var autoload_script_paths:Array[String]
	autoload_script_paths.append(solana_service_path)
	autoload_script_paths.append(http_request_handler_path)
	
	for script in autoload_script_paths:
		var script_parts:PackedStringArray = script.split("/")
		var script_name:String = script_parts[script_parts.size()-1].split(".")[0]
		# Check if the script is not already in the autoload list
		if !ProjectSettings.has_setting("autoload/%s"%script_name):
			# Add the script to the autoload list
			add_autoload_singleton(script_name,script)
			# Save the changes to the Project Settings
			ProjectSettings.save()


func _exit_tree():
	# Clean-up of the plugin goes here.
	remove_export_plugin(export_plugin)
	export_plugin = null


class AndroidExportPlugin extends EditorExportPlugin:
	var _plugin_name = "WalletAdapterAndroid"

	func _supports_platform(platform):
		if platform is EditorExportPlatformAndroid:
			return true
		return false

	func _get_android_libraries(platform, debug):
		if debug:
			return PackedStringArray([SOLANA_PLUGIN_NAME + "/" + _plugin_name + "/bin/debug/" + _plugin_name + "-debug.aar"])
		else:
			return PackedStringArray([SOLANA_PLUGIN_NAME + "/" + _plugin_name + "/bin/release/" + _plugin_name + "-release.aar"])

	func _get_android_dependencies(platform, debug):
		if debug:
			return PackedStringArray([
						"com.solanamobile:mobile-wallet-adapter-clientlib-ktx:2.0.3",
						"com.solanamobile:rpc-core:0.2.8",
						"com.solanamobile:rpc-solana:0.2.8",
						"com.solanamobile:rpc-ktordriver:0.2.8",
						"androidx.compose.ui:ui:1.4.3",
						"androidx.compose.material:material:1.4.3",
						"androidx.compose.ui:ui-tooling-preview:1.4.3",
						"androidx.activity:activity-compose:1.8.0",
						])
		else:
			return PackedStringArray([
						"com.solanamobile:mobile-wallet-adapter-clientlib-ktx:2.0.3",
						"com.solanamobile:rpc-core:0.2.8",
						"com.solanamobile:rpc-solana:0.2.8",
						"com.solanamobile:rpc-ktordriver:0.2.8",
						"androidx.compose.ui:ui:1.4.3",
						"androidx.compose.material:material:1.4.3",
						"androidx.compose.ui:ui-tooling-preview:1.4.3",
						"androidx.activity:activity-compose:1.8.0",])

	func _get_name():
		return _plugin_name
