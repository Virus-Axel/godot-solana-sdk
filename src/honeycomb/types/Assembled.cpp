#include "honeycomb/types/Assembled.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Assembled::set_hash(const Variant& val) {
this->hash = val;
}

Variant Assembled::get_hash() const {
return this->hash;
}

void Assembled::set_mint(const Variant& val) {
this->mint = val;
}

Variant Assembled::get_mint() const {
return this->mint;
}

void Assembled::set_uri(const String& val) {
this->uri = val;
}

String Assembled::get_uri() const {
return this->uri;
}

void Assembled::set_attributes(const Array& val) {
this->attributes = val;
}

Array Assembled::get_attributes() const {
return this->attributes;
}

void Assembled::set_assemblerConfig(const Variant& val) {
this->assemblerConfig = val;
}

Variant Assembled::get_assemblerConfig() const {
return this->assemblerConfig;
}

void Assembled::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_hash"), &Assembled::get_hash);
ClassDB::bind_method(D_METHOD("set_hash", "value"), &Assembled::set_hash);
ClassDB::add_property("Assembled", PropertyInfo(Variant::Type::OBJECT, "hash"), "set_hash", "get_hash");
ClassDB::bind_method(D_METHOD("get_mint"), &Assembled::get_mint);
ClassDB::bind_method(D_METHOD("set_mint", "value"), &Assembled::set_mint);
ClassDB::add_property("Assembled", PropertyInfo(Variant::Type::OBJECT, "mint"), "set_mint", "get_mint");
ClassDB::bind_method(D_METHOD("get_uri"), &Assembled::get_uri);
ClassDB::bind_method(D_METHOD("set_uri", "value"), &Assembled::set_uri);
ClassDB::add_property("Assembled", PropertyInfo(Variant::Type::STRING, "uri"), "set_uri", "get_uri");
ClassDB::bind_method(D_METHOD("get_attributes"), &Assembled::get_attributes);
ClassDB::bind_method(D_METHOD("set_attributes", "value"), &Assembled::set_attributes);
ClassDB::add_property("Assembled", PropertyInfo(Variant::Type::ARRAY, "attributes"), "set_attributes", "get_attributes");
ClassDB::bind_method(D_METHOD("get_assemblerConfig"), &Assembled::get_assemblerConfig);
ClassDB::bind_method(D_METHOD("set_assemblerConfig", "value"), &Assembled::set_assemblerConfig);
ClassDB::add_property("Assembled", PropertyInfo(Variant::Type::OBJECT, "assemblerConfig"), "set_assemblerConfig", "get_assemblerConfig");
ClassDB::bind_method(D_METHOD("to_dict"), &Assembled::to_dict);
}
Dictionary Assembled::to_dict() {
Dictionary res;
if (hash.has_method("to_string")) {
res["hash"] = hash.call("to_string");
} else {
res["hash"] = hash;
}
if (mint.has_method("to_string")) {
res["mint"] = mint.call("to_string");
} else {
res["mint"] = mint;
}
if (!uri.is_empty()) {
    res["uri"] = uri;
}
res["attributes"] = attributes;
if (assemblerConfig.has_method("to_string")) {
res["assemblerConfig"] = assemblerConfig.call("to_string");
} else {
res["assemblerConfig"] = assemblerConfig;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot