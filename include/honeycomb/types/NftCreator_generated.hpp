#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NFTCREATOR
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NFTCREATOR
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class NftCreator : public Resource {
	GDCLASS(NftCreator, Resource)
private:
	Variant address;
	int32_t share;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const Variant &val);
	Variant get_address() const;
	void set_share(const int32_t &val);
	int32_t get_share() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
