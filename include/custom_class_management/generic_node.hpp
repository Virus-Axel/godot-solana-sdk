#ifndef CUSTOM_CLASS_MANAGEMENT_GENERIC_NODE_HPP
#define CUSTOM_CLASS_MANAGEMENT_GENERIC_NODE_HPP

#include <cstdint>
#include <string>

#include "gdextension_interface.h"
#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string_name.hpp"

#include "solana_utils.hpp"
#include "custom_class_management/generic_type.hpp"

namespace godot{

class GenericNode : public GenericType<Node> {
protected:
	void _process(double p_delta) override {}
	void _physics_process(double p_delta) override {}
	void _enter_tree() override {}
	void _exit_tree() override {}
	void _ready() override {}
	[[nodiscard]] PackedStringArray _get_configuration_warnings() const override;
	void _input(const Ref<InputEvent> &p_event) override {}
	void _shortcut_input(const Ref<InputEvent> &p_event) override {}
	void _unhandled_input(const Ref<InputEvent> &p_event) override {}
	void _unhandled_key_input(const Ref<InputEvent> &p_event) override {}

public:
	static void initialize_class();
};

} //namespace godot

#endif