#ifndef GODOT_SOLANA_SDK_MENUBAR_HELPER_HPP
#define GODOT_SOLANA_SDK_MENUBAR_HELPER_HPP
#include "godot_cpp/classes/editor_plugin.hpp"
#include "godot_cpp/classes/http_client.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/classes/wrapped.hpp"

#include "dialogs/add_custom_idl.hpp"

namespace godot {

typedef enum {
	IDL_TO_NODE = 0,
} MenuID;

class MenuBarHelper : public Node {
	GDCLASS(MenuBarHelper, Node);

private:
	static PopupMenu *solana_menu_group;
	static AddCustomIdlDialog *add_custom_idl_dialog;
	static void initialize_dialogs(Node *parent);

protected:
	static void _bind_methods();
	static void menu_pressed_callback(int64_t menu_id);

public:
	void _notification(int32_t p_what, bool p_reversed = false);
	MenuBarHelper() = default;
	static void ready_callback();
	static void deinitialize_dialogs();
	~MenuBarHelper() = default;
};

//void add_menu_item(const PopupMenu *menu);
} //namespace godot

#endif