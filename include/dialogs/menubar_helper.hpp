#ifndef GODOT_SOLANA_SDK_MENUBAR_HELPER_HPP
#define GODOT_SOLANA_SDK_MENUBAR_HELPER_HPP

#include <cstdint>

#include "godot_cpp/classes/editor_plugin.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/variant/dictionary.hpp"

#include "dialogs/add_custom_idl.hpp"
#include "dialogs/generic_dialog.hpp"
#include "solana_utils.hpp"

namespace godot {

using MenuID = enum: uint8_t {
	IDL_TO_NODE = 0,
	MINT_MANAGER = 1,
	MAX_MENU_ID = 2
};

/**
 * @brief Helper class to inject objects to the Godot menu bar.
 */
class MenuBarHelper : public Node {
	GDCLASS_CUSTOM(MenuBarHelper, Node);

private:
	static PopupMenu *solana_menu_group;
	static AddCustomIdlDialog *add_custom_idl_dialog;
	static GenericDialog *mint_manager_dialog;
	static Dictionary *menu_items;
	static void initialize_dialogs(Node *parent);

protected:
	/**
	 * @brief Bind methods for MenuBarHelper Node.
	 */
	static void _bind_methods();

	/**
	 * @brief Emited when a menu item is pressed from the menu.
	 *
	 * @param menu_id ID of item from menu.
	 */
	static void menu_pressed_callback(int64_t menu_id);

public:
	/**
	 * @brief Call bound notification function.
	 *
	 * @param p_what Notification type.
	 * @param p_reversed Reversed.
	 */
	void _notification(int32_t p_what, bool p_reversed = false);
	MenuBarHelper() = default;

	/**
	 * @brief Emited when Solana menu can be created and injected.
	 */
	static void ready_callback();

	/**
	 * @brief Cleans up resources related to MenuBar.
	 */
	static void deinitialize_dialogs();
	~MenuBarHelper() override = default;
};

} //namespace godot

#endif