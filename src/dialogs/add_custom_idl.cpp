#include "dialogs/add_custom_idl.hpp"

namespace godot {

void AddCustomIdlDialog::clear() {
	
}

void AddCustomIdlDialog::_bind_methods() {
}

AddCustomIdlDialog::AddCustomIdlDialog() {
	set_visible(false);
	select_idl_label = memnew(Label);
	select_idl_label->set_text(SELECT_IDL_TEXT);
	add_child(select_idl_label);
}

} //namespace godot