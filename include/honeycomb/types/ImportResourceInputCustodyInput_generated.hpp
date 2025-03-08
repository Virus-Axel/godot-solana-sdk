#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUTCUSTODYINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUTCUSTODYINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ImportResourceInputCustodyInput : public Resource {
	GDCLASS(ImportResourceInputCustodyInput, Resource)
private:
	int64_t supply;
	String burnerDestination;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_supply(const int64_t &val);
	int64_t get_supply() const;
	void set_burnerDestination(const String &val);
	String get_burnerDestination() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
