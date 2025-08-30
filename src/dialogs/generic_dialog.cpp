#include "dialogs/generic_dialog.hpp"

#include "godot_cpp/classes/button.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/file_dialog.hpp"
#include "godot_cpp/classes/h_box_container.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/line_edit.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/script.hpp"
#include "godot_cpp/classes/scene_state.hpp"

#include "anchor/generic_anchor_node.hpp"
#include "solana_utils.hpp"

namespace godot {

void GenericDialog::set_scene_path(const String &path) {
    scene_path = path;
}

void GenericDialog::clear() {
    Ref<PackedScene> scene = ResourceLoader::get_singleton()->load(scene_path, "PackedScene");
    if (scene.is_valid()) {
        auto node = scene->instantiate();
        add_child(node);
    }
    else{
        UtilityFunctions::print("Failed to load scene: ", scene_path);
    }
}

void GenericDialog::load_idl(const String &filename) {
	Ref<FileAccess> file = FileAccess::open(filename, FileAccess::ModeFlags::READ);
	const Dictionary content = JSON::parse_string(file->get_as_text());
	GenericAnchorNode::bind_anchor_node(content);
}

void GenericDialog::idl_selected_callback(const String &filename){

}

void GenericDialog::_bind_methods() {
}

GenericDialog::GenericDialog() {
	set_visible(false);
}

} //namespace godot