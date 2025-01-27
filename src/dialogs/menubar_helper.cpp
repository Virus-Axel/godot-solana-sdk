#include "dialogs/menubar_helper.hpp"

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/editor_interface.hpp"
#include "godot_cpp/classes/menu_bar.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/variant/string.hpp"

#include "dialogs/add_custom_idl.hpp"

namespace godot {

AddCustomIdlDialog *MenuBarHelper::add_custom_idl_dialog = nullptr;
PopupMenu *MenuBarHelper::solana_menu_group = nullptr;

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
	add_custom_idl_dialog = memnew(AddCustomIdlDialog);
	add_custom_idl_dialog->connect("close_requested", Callable(add_custom_idl_dialog, "hide"));
	add_custom_idl_dialog->connect("about_to_popup", callable_mp(add_custom_idl_dialog, &AddCustomIdlDialog::clear));
	parent->add_child(add_custom_idl_dialog);
}

void MenuBarHelper::_bind_methods() {
}

void MenuBarHelper::menu_pressed_callback(int64_t menu_id) {
	switch (menu_id) {
		case MenuID::IDL_TO_NODE:
			add_custom_idl_dialog->popup_centered(Vector2i(400, 300));
			break;
		default:
			ERR_FAIL_EDMSG("Unknown menu item");
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
	solana_menu_group->connect("id_pressed", callable_mp_static(&MenuBarHelper::menu_pressed_callback));
	solana_menu_group->set_name("Solana");
	menu->add_child(solana_menu_group);
}

void MenuBarHelper::deinitialize_dialogs() {
	//Variant deinit(add_custom_idl_dialog);
	//Variant deinit2(solana_menu_group);
}

} //namespace godot