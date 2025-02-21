#include "honeycomb/types/CharacterHistory.hpp"


namespace godot {
namespace honeycomb_resource {

void CharacterHistory::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray CharacterHistory::get_address() const {
return this->address;
}

void CharacterHistory::set_event(const String& val) {
this->event = val;
}

String CharacterHistory::get_event() const {
return this->event;
}

void CharacterHistory::set_event_data(const Dictionary& val) {
this->event_data = val;
}

Dictionary CharacterHistory::get_event_data() const {
return this->event_data;
}

void CharacterHistory::set_time(const String& val) {
this->time = val;
}

String CharacterHistory::get_time() const {
return this->time;
}

void CharacterHistory::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &CharacterHistory::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &CharacterHistory::set_address);
ClassDB::add_property("CharacterHistory", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_event"), &CharacterHistory::get_event);
ClassDB::bind_method(D_METHOD("set_event", "value"), &CharacterHistory::set_event);
ClassDB::add_property("CharacterHistory", PropertyInfo(Variant::Type::STRING, "event"), "set_event", "get_event");
ClassDB::bind_method(D_METHOD("get_event_data"), &CharacterHistory::get_event_data);
ClassDB::bind_method(D_METHOD("set_event_data", "value"), &CharacterHistory::set_event_data);
ClassDB::add_property("CharacterHistory", PropertyInfo(Variant::Type::DICTIONARY, "event_data"), "set_event_data", "get_event_data");
ClassDB::bind_method(D_METHOD("get_time"), &CharacterHistory::get_time);
ClassDB::bind_method(D_METHOD("set_time", "value"), &CharacterHistory::set_time);
ClassDB::add_property("CharacterHistory", PropertyInfo(Variant::Type::STRING, "time"), "set_time", "get_time");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterHistory::to_dict);
}
Dictionary CharacterHistory::to_dict() {
Dictionary res;
res["address"] = address;
if (!event.is_empty()) {
    res["event"] = event;
}
res["event_data"] = event_data;
if (!time.is_empty()) {
    res["time"] = time;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot