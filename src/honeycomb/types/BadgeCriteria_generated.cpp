#include "honeycomb/types/BadgeCriteria_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void BadgeCriteria::set_startTime(const int64_t& val) {
this->startTime = val;
}

int64_t BadgeCriteria::get_startTime() const {
return this->startTime;
}

void BadgeCriteria::set_endTime(const int64_t& val) {
this->endTime = val;
}

int64_t BadgeCriteria::get_endTime() const {
return this->endTime;
}

void BadgeCriteria::set_index(const int32_t& val) {
this->index = val;
}

int32_t BadgeCriteria::get_index() const {
return this->index;
}

void BadgeCriteria::set_condition(const String& val) {
    this->condition = val;
}

String BadgeCriteria::get_condition() const {
    return this->condition;
}

void BadgeCriteria::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_startTime"), &BadgeCriteria::get_startTime);
ClassDB::bind_method(D_METHOD("set_startTime", "value"), &BadgeCriteria::set_startTime);
ClassDB::add_property("BadgeCriteria", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
ClassDB::bind_method(D_METHOD("get_endTime"), &BadgeCriteria::get_endTime);
ClassDB::bind_method(D_METHOD("set_endTime", "value"), &BadgeCriteria::set_endTime);
ClassDB::add_property("BadgeCriteria", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
ClassDB::bind_method(D_METHOD("get_index"), &BadgeCriteria::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &BadgeCriteria::set_index);
ClassDB::add_property("BadgeCriteria", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("get_condition"), &BadgeCriteria::get_condition);
ClassDB::bind_method(D_METHOD("set_condition", "value"), &BadgeCriteria::set_condition);
ClassDB::add_property("BadgeCriteria", PropertyInfo(Variant::STRING, "condition"), "set_condition", "get_condition");
ClassDB::bind_method(D_METHOD("to_dict"), &BadgeCriteria::to_dict);
}
Dictionary BadgeCriteria::to_dict() {
Dictionary res;
res["startTime"] = startTime;
res["endTime"] = endTime;
res["index"] = index;
res["condition"] = this->condition;
return res;
}
} // namespace honeycomb_resource
} // namespace godot