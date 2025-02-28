#include "honeycomb/types/MintAs_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void MintAs::set_kind(const String& val) {
    this->kind = val;
}

String MintAs::get_kind() const {
    return this->kind;
}

void MintAs::set_params(const Dictionary& val) {
this->params = val;
}

Dictionary MintAs::get_params() const {
return this->params;
}

void MintAs::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &MintAs::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &MintAs::set_kind);
ClassDB::add_property("MintAs", PropertyInfo(Variant::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &MintAs::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &MintAs::set_params);
ClassDB::add_property("MintAs", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &MintAs::to_dict);
}
Dictionary MintAs::to_dict() {
Dictionary res;
res["kind"] = this->kind;
res["params"] = params;
return res;
}
} // namespace honeycomb_resource
} // namespace godot