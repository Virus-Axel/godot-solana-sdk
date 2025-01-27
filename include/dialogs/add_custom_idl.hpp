#ifndef GODOT_SOLANA_SDK_ADD_CUSTOM_IDL_HPP
#define GODOT_SOLANA_SDK_ADD_CUSTOM_IDL_HPP

#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/window.hpp"
#include "godot_cpp/classes/h_box_container.hpp"

namespace godot {

class AddCustomIdlDialog : public Window {
	GDCLASS(AddCustomIdlDialog, Window)
private:
	const char *SELECT_IDL_TEXT = "Select IDL";
	Label *select_idl_label = nullptr;
	HBoxContainer *parent_window = nullptr;

	void clear();

protected:
	static void _bind_methods();

public:
	AddCustomIdlDialog();
	~AddCustomIdlDialog() = default;
};
} //namespace godot

#endif