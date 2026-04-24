#solana sdk plugin
@tool
extends EditorPlugin

const SOLANA_PLUGIN_NAME := "SolanaSDK"

# Story 2-1 T7 — DD-18: MWA facade is a non-optional autoload registered the
# moment the plugin enables. Unlike `SolanaService` (Optional/ — opt-in), the
# MWA entry point is always wired. The plugin is responsible for adding +
# removing the singleton entry in `project.godot`; users don't need to
# manually edit ProjectSettings → Autoload.
const MWA_AUTOLOAD_NAME := "MWA"
const MWA_AUTOLOAD_PATH := "res://addons/" + SOLANA_PLUGIN_NAME + "/mwa/MWA.gd"

var solana_service_path:String = "res://addons/" + SOLANA_PLUGIN_NAME + "/Optional/SolanaService/Autoload/SolanaService.tscn"
var http_request_handler_path:String = "res://addons/" + SOLANA_PLUGIN_NAME + "/Optional/SolanaService/Autoload/HttpRequestHandler.tscn"
var export_plugin : AndroidExportPlugin


func _enter_tree():
	export_plugin = AndroidExportPlugin.new()
	add_export_plugin(export_plugin)
	# DD-18: register the MWA singleton so GDScript callers can use
	# `MWA.connect(...)` without per-project autoload configuration. Exported
	# games read the [autoload] block from project.godot at runtime, so this
	# edit persists across editor sessions (Godot writes the change through).
	add_autoload_singleton(MWA_AUTOLOAD_NAME, MWA_AUTOLOAD_PATH)


func _exit_tree():
	# Clean-up of the plugin goes here.
	remove_export_plugin(export_plugin)
	export_plugin = null
	remove_autoload_singleton(MWA_AUTOLOAD_NAME)


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
