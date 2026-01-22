#ifndef GODOT_SOLANA_SDK_TEST_VALIDATOR_HPP
#define GODOT_SOLANA_SDK_TEST_VALIDATOR_HPP

#include <cstdint>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/thread.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_client.hpp"
#include "solana_utils.hpp"

namespace godot {

extern "C" int start_validator(int x);

class TestValidator : public Node {
	GDCLASS_CUSTOM(TestValidator, Node)

private:
    bool started = false;
    Ref<Thread> thread;
protected:
	/**
	 * @brief @bindmethods{TestValidator, Node}
	 */
	static void _bind_methods();
    void queue_start();

public:
    void _ready() override;
    void _process(double delta) override;
    void _notification(int p_what);
	TestValidator() = default;
	~TestValidator() override = default;
};

} //namespace godot

#endif // GODOT_SOLANA_SDK_TEST_VALIDATOR_HPP