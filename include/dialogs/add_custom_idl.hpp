#ifndef GODOT_SOLANA_SDK_ADD_CUSTOM_IDL_HPP
#define GODOT_SOLANA_SDK_ADD_CUSTOM_IDL_HPP
#include <string>

#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/window.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

namespace godot {

const std::string CUSTOM_IDL_LOCATION_SETTING = "solana_sdk/anchor/custom_anchor_programs";

/**
 * @brief Dialog window used to create custom IDL nodes,
 */
class AddCustomIdlDialog : public Window {
	GDCLASS_CUSTOM(AddCustomIdlDialog, Window)
private:
	const char *SELECT_IDL_TEXT = "Select IDL";
	Label *select_idl_label = nullptr;
	Variant parent_window = nullptr;

	static void idl_selected_callback(const String &filename);

protected:
	/**
	 * @brief Binds methods of AddCustomIdlDialog Window.
	 */
	static void _bind_methods();

public:
	AddCustomIdlDialog();

	/**
	 * @brief Resets the window to its default appearance.
	 */
	void clear();

	/**
	 * @brief Loads an IDL from a file and creates a generic anchor node from it.
	 *
	 * @param filename Path to IDL file.
	 */
	static void load_idl(const String &filename);
	~AddCustomIdlDialog() override = default;
};
} //namespace godot

#endif