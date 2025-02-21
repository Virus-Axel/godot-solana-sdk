#include "honeycomb/types/MintAsMplBubblegumInput.hpp"


namespace godot {
namespace honeycomb_resource {

void MintAsMplBubblegumInput::set_maxDepth(const int32_t& val) {
this->maxDepth = val;
}

int32_t MintAsMplBubblegumInput::get_maxDepth() const {
return this->maxDepth;
}

void MintAsMplBubblegumInput::set_maxBufferSize(const int32_t& val) {
this->maxBufferSize = val;
}

int32_t MintAsMplBubblegumInput::get_maxBufferSize() const {
return this->maxBufferSize;
}

void MintAsMplBubblegumInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_maxDepth"), &MintAsMplBubblegumInput::get_maxDepth);
ClassDB::bind_method(D_METHOD("set_maxDepth", "value"), &MintAsMplBubblegumInput::set_maxDepth);
ClassDB::add_property("MintAsMplBubblegumInput", PropertyInfo(Variant::Type::INT, "maxDepth"), "set_maxDepth", "get_maxDepth");
ClassDB::bind_method(D_METHOD("get_maxBufferSize"), &MintAsMplBubblegumInput::get_maxBufferSize);
ClassDB::bind_method(D_METHOD("set_maxBufferSize", "value"), &MintAsMplBubblegumInput::set_maxBufferSize);
ClassDB::add_property("MintAsMplBubblegumInput", PropertyInfo(Variant::Type::INT, "maxBufferSize"), "set_maxBufferSize", "get_maxBufferSize");
ClassDB::bind_method(D_METHOD("to_dict"), &MintAsMplBubblegumInput::to_dict);
}
Dictionary MintAsMplBubblegumInput::to_dict() {
Dictionary res;
res["maxDepth"] = maxDepth;
res["maxBufferSize"] = maxBufferSize;
return res;
}
} // namespace honeycomb_resource
} // namespace godot