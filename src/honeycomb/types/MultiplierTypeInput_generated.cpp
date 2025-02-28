#include "honeycomb/types/MultiplierTypeInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void MultiplierTypeInput::set_minStakeDuration(const int64_t& val) {
this->minStakeDuration = val;
}

int64_t MultiplierTypeInput::get_minStakeDuration() const {
return this->minStakeDuration;
}

void MultiplierTypeInput::set_minNftCount(const int64_t& val) {
this->minNftCount = val;
}

int64_t MultiplierTypeInput::get_minNftCount() const {
return this->minNftCount;
}

void MultiplierTypeInput::set_creator(const String& val) {
this->creator = val;
}

String MultiplierTypeInput::get_creator() const {
return this->creator;
}

void MultiplierTypeInput::set_collection(const String& val) {
this->collection = val;
}

String MultiplierTypeInput::get_collection() const {
return this->collection;
}

void MultiplierTypeInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_minStakeDuration"), &MultiplierTypeInput::get_minStakeDuration);
ClassDB::bind_method(D_METHOD("set_minStakeDuration", "value"), &MultiplierTypeInput::set_minStakeDuration);
ClassDB::add_property("MultiplierTypeInput", PropertyInfo(Variant::Type::INT, "minStakeDuration"), "set_minStakeDuration", "get_minStakeDuration");
ClassDB::bind_method(D_METHOD("get_minNftCount"), &MultiplierTypeInput::get_minNftCount);
ClassDB::bind_method(D_METHOD("set_minNftCount", "value"), &MultiplierTypeInput::set_minNftCount);
ClassDB::add_property("MultiplierTypeInput", PropertyInfo(Variant::Type::INT, "minNftCount"), "set_minNftCount", "get_minNftCount");
ClassDB::bind_method(D_METHOD("get_creator"), &MultiplierTypeInput::get_creator);
ClassDB::bind_method(D_METHOD("set_creator", "value"), &MultiplierTypeInput::set_creator);
ClassDB::add_property("MultiplierTypeInput", PropertyInfo(Variant::Type::STRING, "creator"), "set_creator", "get_creator");
ClassDB::bind_method(D_METHOD("get_collection"), &MultiplierTypeInput::get_collection);
ClassDB::bind_method(D_METHOD("set_collection", "value"), &MultiplierTypeInput::set_collection);
ClassDB::add_property("MultiplierTypeInput", PropertyInfo(Variant::Type::STRING, "collection"), "set_collection", "get_collection");
ClassDB::bind_method(D_METHOD("to_dict"), &MultiplierTypeInput::to_dict);
}
Dictionary MultiplierTypeInput::to_dict() {
Dictionary res;
res["minStakeDuration"] = minStakeDuration;
res["minNftCount"] = minNftCount;
if (!creator.is_empty()) {
    res["creator"] = creator;
}
if (!collection.is_empty()) {
    res["collection"] = collection;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot