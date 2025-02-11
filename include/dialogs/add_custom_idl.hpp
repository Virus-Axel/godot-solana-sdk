#ifndef GODOT_SOLANA_SDK_ADD_CUSTOM_IDL_HPP
#define GODOT_SOLANA_SDK_ADD_CUSTOM_IDL_HPP

#include "godot_cpp/classes/file_dialog.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/window.hpp"

namespace godot {

const std::string CUSTOM_IDL_LOCATION_SETTING = "solana_sdk/anchor/custom_anchor_programs";

class AddCustomIdlDialog : public Window {
	GDCLASS(AddCustomIdlDialog, Window)
private:
	const char *SELECT_IDL_TEXT = "Select IDL";
	Label *select_idl_label = nullptr;
	Variant parent_window = nullptr;

	static void idl_selected_callback(const String &filename);

protected:
	static void _bind_methods();

public:
	AddCustomIdlDialog();
	void clear();
	static void load_idl(const String &filename);
	~AddCustomIdlDialog() = default;
};
} //namespace godot

#endif