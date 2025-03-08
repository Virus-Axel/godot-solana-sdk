#include "honeycomb/types/AdvancedTreeConfig_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void AdvancedTreeConfig::set_maxDepth(const int32_t &val) {
	this->maxDepth = val;
}

int32_t AdvancedTreeConfig::get_maxDepth() const {
	return this->maxDepth;
}

void AdvancedTreeConfig::set_maxBufferSize(const int32_t &val) {
	this->maxBufferSize = val;
}

int32_t AdvancedTreeConfig::get_maxBufferSize() const {
	return this->maxBufferSize;
}

void AdvancedTreeConfig::set_canopyDepth(const int32_t &val) {
	this->canopyDepth = val;
}

int32_t AdvancedTreeConfig::get_canopyDepth() const {
	return this->canopyDepth;
}

void AdvancedTreeConfig::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_maxDepth"), &AdvancedTreeConfig::get_maxDepth);
	ClassDB::bind_method(D_METHOD("set_maxDepth", "value"), &AdvancedTreeConfig::set_maxDepth);
	ClassDB::add_property("AdvancedTreeConfig", PropertyInfo(Variant::Type::INT, "maxDepth"), "set_maxDepth", "get_maxDepth");
	ClassDB::bind_method(D_METHOD("get_maxBufferSize"), &AdvancedTreeConfig::get_maxBufferSize);
	ClassDB::bind_method(D_METHOD("set_maxBufferSize", "value"), &AdvancedTreeConfig::set_maxBufferSize);
	ClassDB::add_property("AdvancedTreeConfig", PropertyInfo(Variant::Type::INT, "maxBufferSize"), "set_maxBufferSize", "get_maxBufferSize");
	ClassDB::bind_method(D_METHOD("get_canopyDepth"), &AdvancedTreeConfig::get_canopyDepth);
	ClassDB::bind_method(D_METHOD("set_canopyDepth", "value"), &AdvancedTreeConfig::set_canopyDepth);
	ClassDB::add_property("AdvancedTreeConfig", PropertyInfo(Variant::Type::INT, "canopyDepth"), "set_canopyDepth", "get_canopyDepth");
	ClassDB::bind_method(D_METHOD("to_dict"), &AdvancedTreeConfig::to_dict);
}
Dictionary AdvancedTreeConfig::to_dict() {
	Dictionary res;
	res["maxDepth"] = maxDepth;
	res["maxBufferSize"] = maxBufferSize;
	res["canopyDepth"] = canopyDepth;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot