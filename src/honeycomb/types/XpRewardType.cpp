#include "honeycomb/types/XpRewardType.hpp"


namespace godot {
namespace honeycomb_resource {

void XpRewardType::set_kind(const String& val) {
this->kind = val;
}

String XpRewardType::get_kind() const {
return this->kind;
}

void XpRewardType::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &XpRewardType::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &XpRewardType::set_kind);
ClassDB::add_property("XpRewardType", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("to_dict"), &XpRewardType::to_dict);
}
Dictionary XpRewardType::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot