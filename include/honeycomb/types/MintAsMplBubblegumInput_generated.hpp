#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASMPLBUBBLEGUMINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASMPLBUBBLEGUMINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class MintAsMplBubblegumInput : public Resource {
	GDCLASS(MintAsMplBubblegumInput, Resource)
private:
	int32_t maxDepth;
	int32_t maxBufferSize;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_maxDepth(const int32_t &val);
	int32_t get_maxDepth() const;
	void set_maxBufferSize(const int32_t &val);
	int32_t get_maxBufferSize() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
