#include "dialogs/menubar_helper.hpp"

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/editor_interface.hpp"
#include "godot_cpp/classes/menu_bar.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

#include "dialogs/add_custom_idl.hpp"

namespace godot {

AddCustomIdlDialog *MenuBarHelper::add_custom_idl_dialog = nullptr;
GenericDialog *MenuBarHelper::mint_manager_dialog = nullptr;
PopupMenu *MenuBarHelper::solana_menu_group = nullptr;
Dictionary *MenuBarHelper::menu_items = nullptr;

static Node *find_menu_bar(const Node *tree) {
	for (unsigned int j = 0; j < tree->get_child_count(); j++) {
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

void MenuBarHelper::initialize_dialogs(Node *parent) {
	menu_items = memnew(Dictionary);

	add_custom_idl_dialog = memnew(AddCustomIdlDialog);
	add_custom_idl_dialog->connect("close_requested", Callable(add_custom_idl_dialog, "hide"));
	add_custom_idl_dialog->connect("about_to_popup", callable_mp(add_custom_idl_dialog, &AddCustomIdlDialog::clear));
	parent->add_child(add_custom_idl_dialog);

	mint_manager_dialog = memnew(GenericDialog);
	GenericDialog* mint_manager_dialog_ptr = Object::cast_to<GenericDialog>(mint_manager_dialog);
	mint_manager_dialog_ptr->connect("close_requested", Callable(mint_manager_dialog_ptr, "hide"));
	mint_manager_dialog_ptr->connect("about_to_popup", callable_mp(mint_manager_dialog_ptr, &GenericDialog::clear));
	parent->add_child(mint_manager_dialog_ptr);

	Ref<DirAccess> dir = DirAccess::open("res://addons/SolanaSDK/Dialogs/");

    
    if (dir.is_null()) {
        return;
    }

    dir->list_dir_begin();

    String file_name;
    while ((file_name = dir->get_next()).length() > 0) {
        if (dir->current_is_dir()) {
            continue; // Skip directories.
        } else if (file_name.ends_with(".tscn")) {
			const int64_t FILE_ENDING_LENGTH = 5; 
			const Array words = file_name.left(-FILE_ENDING_LENGTH).to_snake_case().split("_");
			const String label = String(" ").join(words).capitalize();
			(*menu_items)[label] = memnew(GenericDialog);
			GenericDialog* generic_dialog_ptr = Object::cast_to<GenericDialog>((*menu_items)[label]);
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

void MenuBarHelper::menu_pressed_callback(int64_t menu_id) {
	switch (menu_id) {
		case MenuID::IDL_TO_NODE:
			add_custom_idl_dialog->popup_centered(Vector2i(400, 300));
			break;
		case MenuID::MINT_MANAGER:
			mint_manager_dialog->popup_centered(Vector2i(600, 400));
			break;
		default:
			const uint32_t generic_menu_id = menu_id - MenuID::MAX_MENU_ID;

			ERR_FAIL_COND_EDMSG(generic_menu_id >= menu_items->size(), "Unknown menu item");
			const String key = menu_items->keys()[generic_menu_id];
			Object::cast_to<GenericDialog>((*menu_items)[key])->popup_centered(Vector2i(600, 400));
			break;
	}
}

void MenuBarHelper::_notification(int32_t p_what, bool p_reversed) {
	if (p_what == 3) {
		godot::MenuBarHelper::ready_callback();
	}
}

void MenuBarHelper::ready_callback() {
	Control *root = EditorInterface::get_singleton()->get_base_control();
	Node *menu = find_menu_bar(root);

	solana_menu_group = memnew(PopupMenu);
	ERR_FAIL_COND_EDMSG(solana_menu_group == nullptr, "Failed to initialize Solana menu");

	initialize_dialogs(root);

	solana_menu_group->add_item("Add Node from IDL", MenuID::IDL_TO_NODE);
	solana_menu_group->add_item("Mint Manager", MenuID::MINT_MANAGER);

	for(unsigned int i = 0; i < menu_items->size(); i++) {
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