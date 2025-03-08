#include "honeycomb/types/MultiplierTypeParamsNFTCount_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void MultiplierTypeParamsNFTCount::set_minCount(const int64_t &val) {
	this->minCount = val;
}

int64_t MultiplierTypeParamsNFTCount::get_minCount() const {
	return this->minCount;
}

void MultiplierTypeParamsNFTCount::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_minCount"), &MultiplierTypeParamsNFTCount::get_minCount);
	ClassDB::bind_method(D_METHOD("set_minCount", "value"), &MultiplierTypeParamsNFTCount::set_minCount);
	ClassDB::add_property("MultiplierTypeParamsNFTCount", PropertyInfo(Variant::Type::INT, "minCount"), "set_minCount", "get_minCount");
	ClassDB::bind_method(D_METHOD("to_dict"), &MultiplierTypeParamsNFTCount::to_dict);
}
Dictionary MultiplierTypeParamsNFTCount::to_dict() {
	Dictionary res;
	res["minCount"] = minCount;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot