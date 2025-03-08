#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKER
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class Staker : public Resource {
	GDCLASS(Staker, Resource)
private:
	PackedByteArray address;
	int32_t bump;
	Variant stakingPool;
	Variant wallet;
	int64_t totalStaked;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const PackedByteArray &val);
	PackedByteArray get_address() const;
	void set_bump(const int32_t &val);
	int32_t get_bump() const;
	void set_stakingPool(const Variant &val);
	Variant get_stakingPool() const;
	void set_wallet(const Variant &val);
	Variant get_wallet() const;
	void set_totalStaked(const int64_t &val);
	int64_t get_totalStaked() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
