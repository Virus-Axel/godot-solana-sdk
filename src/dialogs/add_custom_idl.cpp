#include "dialogs/add_custom_idl.hpp"

#include <cstdint>

#include "godot_cpp/classes/button.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/file_dialog.hpp"
#include "godot_cpp/classes/h_box_container.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/classes/window.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

#include "anchor/generic_anchor_node.hpp"
#include "anchor/generic_anchor_resource.hpp"
#include "solana_utils.hpp"

namespace godot {

void AddCustomIdlDialog::clear() {
	VBoxContainer *parent_ptr = memnew_custom(VBoxContainer);
	parent_window = parent_ptr;
	Label *instruction_text = memnew_custom(Label);
	instruction_text->set_text("Pick an IDL file.");
	parent_ptr->add_child(instruction_text);

	HBoxContainer *file_section = memnew_custom(HBoxContainer);
	LineEdit *file_line = memnew_custom(LineEdit);
	file_line->set_editable(false);
	file_section->add_child(file_line);
	Button *file_button = memnew_custom(Button);
	file_button->set_text("Open");
	file_section->add_child(file_button);
	parent_ptr->add_child(file_section);
	FileDialog *file_selector = memnew_custom(FileDialog);

	const Callable load_idl_callable = callable_mp_static(&AddCustomIdlDialog::idl_selected_callback);
	file_selector->connect("file_selected", load_idl_callable);
	file_selector->set_access(FileDialog::Access::ACCESS_RESOURCES);
	file_selector->set_file_mode(FileDialog::FileMode::FILE_MODE_OPEN_FILE);
	parent_ptr->add_child(file_selector);

	const int32_t IDL_WINDOW_WIDTH = 600;
	const int32_t IDL_WINDOW_HEIGHT = 400;
	const Callable open_dialog_callable = callable_mp((Window *)file_selector, &Window::popup_centered).bindv(Array::make(Vector2i(IDL_WINDOW_WIDTH, IDL_WINDOW_HEIGHT)));
	file_button->connect("pressed", open_dialog_callable);

	add_child(parent_ptr);
}

void AddCustomIdlDialog::load_idl(const String &filename) {
	const Ref<FileAccess> file = FileAccess::open(filename, FileAccess::ModeFlags::READ);
	const Dictionary content = JSON::parse_string(file->get_as_text());
	GenericAnchorNode::bind_anchor_node<GenericAnchorNode, GenericAnchorResource>(content);
}

void AddCustomIdlDialog::idl_selected_callback(const String &filename) {
	const Variant idl_locations = ProjectSettings::get_singleton()->get_setting(String(CUSTOM_IDL_LOCATION_SETTING.c_str()));
	ERR_FAIL_COND_EDMSG_CUSTOM(idl_locations.get_type() != Variant::PACKED_STRING_ARRAY, "Could not find setting for idl locations");
	PackedStringArray idl_filenames = static_cast<PackedStringArray>(idl_locations);
	idl_filenames.append(filename);
	ProjectSettings::get_singleton()->set_setting(String(CUSTOM_IDL_LOCATION_SETTING.c_str()), idl_filenames);
	ProjectSettings::get_singleton()->save();

	UtilityFunctions::print("Reloading editor is required.");
	//load_idl(filename);
}

void AddCustomIdlDialog::_bind_methods() {
}

AddCustomIdlDialog::AddCustomIdlDialog() {
	set_visible(false);
}

} //namespace godot