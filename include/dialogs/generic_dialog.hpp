#ifndef GENERIC_DIALOG_HPP
#define GENERIC_DIALOG_HPP


#include "godot_cpp/classes/file_dialog.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/window.hpp"

namespace godot {

/**
 * @brief Dialog window used to manage mints and tokens.
 */
class GenericDialog : public Window {
	GDCLASS(GenericDialog, Window)
private:
	const char *SELECT_IDL_TEXT = "Select IDL";
	Label *select_idl_label = nullptr;
	Variant parent_window = nullptr;
    String scene_path = "";

	static void idl_selected_callback(const String &filename);

protected:
	/**
	 * @brief @bindmethods{MintManagerDialog, Window}
	 */
	static void _bind_methods();

public:
	GenericDialog();

	/**
	 * @brief Resets the window to its default appearance.
	 */
	void clear();

    /**
     * @setter{scene_path, path}
     */
    void set_scene_path(const String &path);

	/**
	 * @brief Loads an IDL from a file and creates a generic anchor node from it.
	 * 
	 * @param filename Path to IDL file.
	 */
	static void load_idl(const String &filename);
	~GenericDialog() = default;
};
} //namespace godot 

#endif // MINT_MANAGER_HPP