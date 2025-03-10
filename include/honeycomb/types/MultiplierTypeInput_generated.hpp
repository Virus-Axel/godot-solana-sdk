#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class MultiplierTypeInput : public Resource {
	GDCLASS(MultiplierTypeInput, Resource)
private:
	int64_t minStakeDuration;
	int64_t minNftCount;
	String creator;
	String collection;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_minStakeDuration(const int64_t &val);
	int64_t get_minStakeDuration() const;
	void set_minNftCount(const int64_t &val);
	int64_t get_minNftCount() const;
	void set_creator(const String &val);
	String get_creator() const;
	void set_collection(const String &val);
	String get_collection() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
