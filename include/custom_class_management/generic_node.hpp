#ifndef CUSTOM_CLASS_MANAGEMENT_GENERIC_NODE_HPP
#define CUSTOM_CLASS_MANAGEMENT_GENERIC_NODE_HPP

#include "godot_cpp/classes/input_event.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string_name.hpp"

#include "custom_class_management/generic_type.hpp"

namespace godot{

/**
 * @brief Extensible node that can be extended and modified.
 */
class GenericNode : public GenericType<Node> {
protected:
	void _process(double p_delta) override {} ///< Intermediate _process method.
	void _physics_process(double p_delta) override {} ///< Intermediate _physics_process method.
	void _enter_tree() override {} ///< Intermediate _enter_tree method.
	void _exit_tree() override {} ///< Intermediate _exit_tree method.
	void _ready() override {} ///< Intermediate _ready method.
	[[nodiscard]] PackedStringArray _get_configuration_warnings() const override; ///< Intermediate _get_configuration_warnings method.
	void _input(const Ref<InputEvent> &p_event) override {} ///< Intermediate _input method.
	void _shortcut_input(const Ref<InputEvent> &p_event) override {} ///< Intermediate _shortcut_input method.
	void _unhandled_input(const Ref<InputEvent> &p_event) override {} ///< Intermediate _unhandled_input method.
	void _unhandled_key_input(const Ref<InputEvent> &p_event) override {} ///< Intermediate _unhandled_key_input method.

public:
	/**
	 * @brief Initializes class and virtual methods.
	 */
	static void initialize_class();
};

} //namespace godot

#endif