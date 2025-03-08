#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSNFTCOUNT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSNFTCOUNT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class MultiplierTypeParamsNFTCount : public Resource {
	GDCLASS(MultiplierTypeParamsNFTCount, Resource)
private:
	int64_t minCount;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_minCount(const int64_t &val);
	int64_t get_minCount() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
