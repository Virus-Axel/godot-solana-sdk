#include "honeycomb/types/Service_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void Service::set_kind(const String& val) {
this->kind = val;
}

String Service::get_kind() const {
return this->kind;
}

void Service::set_params(const Dictionary& val) {
this->params = val;
}

Dictionary Service::get_params() const {
return this->params;
}

void Service::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &Service::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &Service::set_kind);
ClassDB::add_property("Service", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &Service::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &Service::set_params);
ClassDB::add_property("Service", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &Service::to_dict);
}
Dictionary Service::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
res["params"] = params;
return res;
}
} // namespace honeycomb_resource
} // namespace godot