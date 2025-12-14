#include "dialogs/menubar_helper.hpp"

#include <cstdint>
#include <string>

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/editor_interface.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/menu_bar.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "dialogs/add_custom_idl.hpp"
#include "dialogs/generic_dialog.hpp"
#include "solana_utils.hpp"

namespace godot {

AddCustomIdlDialog *MenuBarHelper::add_custom_idl_dialog = nullptr;
GenericDialog *MenuBarHelper::mint_manager_dialog = nullptr;
PopupMenu *MenuBarHelper::solana_menu_group = nullptr;
Dictionary *MenuBarHelper::menu_items = nullptr;

namespace {

const std::string ENABLED_SOLANA_SERVICE_PATH = "res://addons/SolanaSDK/Optional/SolanaService";
const std::string DISABLED_SOLANA_SERVICE_PATH = "res://addons/SolanaSDK/Optional/.SolanaService";

bool is_solana_service_enabled() {
	const Ref<DirAccess> dir = DirAccess::open(String(ENABLED_SOLANA_SERVICE_PATH.c_str()));
	return !dir.is_null();
}

Node *find_menu_bar(const Node *tree) {
	for (int32_t j = 0; j < tree->get_child_count(); j++) {
		Node *child = tree->get_child(j);
		if (child->get_class() == "MenuBar") {
			return child;
		}
		Node *ret = find_menu_bar(child);
		if (ret != nullptr) {
			return ret;
		}
	}
	return nullptr;
}
} //namespace

void MenuBarHelper::initialize_dialogs(Node *parent) {
	menu_items = memnew_custom(Dictionary);

	add_custom_idl_dialog = memnew_custom(AddCustomIdlDialog);
	add_custom_idl_dialog->connect("close_requested", Callable(add_custom_idl_dialog, "hide"));
	add_custom_idl_dialog->connect("about_to_popup", callable_mp(add_custom_idl_dialog, &AddCustomIdlDialog::clear));
	parent->add_child(add_custom_idl_dialog);

	mint_manager_dialog = memnew_custom(GenericDialog);
	auto *mint_manager_dialog_ptr = Object::cast_to<GenericDialog>(mint_manager_dialog);
	mint_manager_dialog_ptr->connect("close_requested", Callable(mint_manager_dialog_ptr, "hide"));
	mint_manager_dialog_ptr->connect("about_to_popup", callable_mp(mint_manager_dialog_ptr, &GenericDialog::clear));
	parent->add_child(mint_manager_dialog_ptr);

	const Ref<DirAccess> dir = DirAccess::open("res://addons/SolanaSDK/Dialogs/");

	if (dir.is_null()) {
		return;
	}

	dir->list_dir_begin();

	String file_name;
	while ((file_name = dir->get_next()).length() > 0) {
		if (dir->current_is_dir()) {
			continue; // Skip directories.
		}
		if (file_name.ends_with(".tscn")) {
			const int64_t FILE_ENDING_LENGTH = 5;
			const Array words = file_name.left(-FILE_ENDING_LENGTH).to_snake_case().split("_");
			const String label = String(" ").join(words).capitalize();
			(*menu_items)[label] = memnew_custom(GenericDialog);
			auto *generic_dialog_ptr = Object::cast_to<GenericDialog>((*menu_items)[label]);
			generic_dialog_ptr->set_scene_path("res://addons/SolanaSDK/Dialogs/" + file_name);
			generic_dialog_ptr->connect("close_requested", Callable(generic_dialog_ptr, "hide"));
			generic_dialog_ptr->connect("about_to_popup", callable_mp(generic_dialog_ptr, &GenericDialog::clear));
			parent->add_child(generic_dialog_ptr);
		}
	}

	dir->list_dir_end();
}

void MenuBarHelper::_bind_methods() {
}

void MenuBarHelper::toggle_solana_service() {
	const String enabled_path = String(ENABLED_SOLANA_SERVICE_PATH.c_str());
	const String disabled_path = String(DISABLED_SOLANA_SERVICE_PATH.c_str());

	const String solana_service_path_relative = "Autoload/SolanaService.tscn";
	const String http_request_handler_path_relative = "Autoload/HttpRequestHandler.tscn";

	const String solana_service_setting = String(SOLANA_SERVICE_SETTING_LOCATION.c_str());
	const String solana_service_autoload_setting = "autoload/SolanaService";
	const String http_request_handler_autoload_setting = "autoload/HttpRequestHandler";

	if (!is_solana_service_enabled()) {
		const Ref<DirAccess> dir = DirAccess::open(disabled_path);
		ERR_FAIL_COND_EDMSG_CUSTOM(dir.is_null(), "Failed to find .SolanaService directory to enable SolanaService");
		const Error error_status = dir->rename(disabled_path, enabled_path);
		ERR_FAIL_COND_EDMSG_CUSTOM(error_status != OK, "Failed to rename SolanaService directory");
		solana_menu_group->set_item_text(MenuID::TOGGLE_SOLANA_SERVICE, "Disable SolanaService");
		ProjectSettings::get_singleton()->set_setting(solana_service_setting, true);
		ProjectSettings::get_singleton()->set_setting(solana_service_autoload_setting, vformat("*%s/%s", enabled_path, solana_service_path_relative));
		ProjectSettings::get_singleton()->set_setting(http_request_handler_autoload_setting, vformat("*%s/%s", enabled_path, http_request_handler_path_relative));
		ProjectSettings::get_singleton()->save();
	} else {
		const Ref<DirAccess> dir = DirAccess::open(enabled_path);
		ERR_FAIL_COND_EDMSG_CUSTOM(dir.is_null(), "Failed to find .SolanaService directory to disable SolanaService");
		const Error error_status = dir->rename(enabled_path, disabled_path);
		ERR_FAIL_COND_EDMSG_CUSTOM(error_status != OK, "Failed to rename SolanaService directory");
		solana_menu_group->set_item_text(MenuID::TOGGLE_SOLANA_SERVICE, "Enable SolanaService");
		ProjectSettings::get_singleton()->set_setting(solana_service_setting, false);
		ProjectSettings::get_singleton()->clear(solana_service_autoload_setting);
		ProjectSettings::get_singleton()->clear(http_request_handler_autoload_setting);
		ProjectSettings::get_singleton()->save();
	}
	UtilityFunctions::print("Restart the editor to apply the changes to SolanaService.");
}

void MenuBarHelper::menu_pressed_callback(int64_t menu_id) {
	const int32_t DEFAULT_WIDTH = 600;
	const int32_t DEFAULT_HEIGHT = 400;
	const int32_t IDL_DIALOG_WIDTH = 400;
	const int32_t IDL_DIALOG_HEIGHT = 300;

	switch (menu_id) {
		case MenuID::TOGGLE_SOLANA_SERVICE:
			toggle_solana_service();
			break;
		case MenuID::IDL_TO_NODE:
			add_custom_idl_dialog->popup_centered(Vector2i(IDL_DIALOG_WIDTH, IDL_DIALOG_HEIGHT));
			break;
		case MenuID::MINT_MANAGER:
			mint_manager_dialog->popup_centered(Vector2i(DEFAULT_WIDTH, DEFAULT_HEIGHT));
			break;
		default:
			const uint32_t generic_menu_id = menu_id - MenuID::MAX_MENU_ID;

			ERR_FAIL_COND_EDMSG_CUSTOM(generic_menu_id >= menu_items->size(), "Unknown menu item");
			const String key = menu_items->keys()[generic_menu_id];
			Object::cast_to<GenericDialog>((*menu_items)[key])->popup_centered(Vector2i(DEFAULT_WIDTH, DEFAULT_HEIGHT));
			break;
	}
}

void MenuBarHelper::_notification(int32_t p_what, bool p_reversed) { // NOLINT(readability-convert-member-functions-to-static)
	(void)p_reversed;
	if (p_what == 3) {
		godot::MenuBarHelper::ready_callback();
	}
}

void MenuBarHelper::ready_callback() {
	Control *root = EditorInterface::get_singleton()->get_base_control();
	Node *menu = find_menu_bar(root);

	solana_menu_group = memnew_custom(PopupMenu);
	ERR_FAIL_COND_EDMSG_CUSTOM(solana_menu_group == nullptr, "Failed to initialize Solana menu");

	initialize_dialogs(root);

	if (is_solana_service_enabled()) {
		solana_menu_group->add_item("Disable SolanaService", MenuID::TOGGLE_SOLANA_SERVICE);
	} else {
		solana_menu_group->add_item("Enable SolanaService", MenuID::TOGGLE_SOLANA_SERVICE);
	}
	solana_menu_group->add_item("Add Node from IDL", MenuID::IDL_TO_NODE);
	solana_menu_group->add_item("Mint Manager", MenuID::MINT_MANAGER);

	for (int32_t i = 0; i < menu_items->size(); i++) {
		const String item_name = menu_items->keys()[i];
		solana_menu_group->add_item(item_name, MenuID::MAX_MENU_ID + i);
	}

	solana_menu_group->connect("id_pressed", callable_mp_static(&MenuBarHelper::menu_pressed_callback));
	solana_menu_group->set_name("Solana");

	menu->add_child(solana_menu_group);
}

void MenuBarHelper::deinitialize_dialogs() {
	//Variant deinit(add_custom_idl_dialog);
	//Variant deinit2(solana_menu_group);
	memfree(menu_items);
}

} //namespace godot