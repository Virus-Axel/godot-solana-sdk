#include "honeycomb/types/CharacterConfigAssembled.hpp"

#include "pubkey.hpp"
#include "honeycomb/types/MintAs.hpp"

namespace godot {
namespace honeycomb_resource {

void CharacterConfigAssembled::set_kind(const String& val) {
this->kind = val;
}

String CharacterConfigAssembled::get_kind() const {
return this->kind;
}

void CharacterConfigAssembled::set_assemblerConfig(const Variant& val) {
this->assemblerConfig = val;
}

Variant CharacterConfigAssembled::get_assemblerConfig() const {
return this->assemblerConfig;
}

void CharacterConfigAssembled::set_name(const String& val) {
this->name = val;
}

String CharacterConfigAssembled::get_name() const {
return this->name;
}

void CharacterConfigAssembled::set_symbol(const String& val) {
this->symbol = val;
}

String CharacterConfigAssembled::get_symbol() const {
return this->symbol;
}

void CharacterConfigAssembled::set_description(const String& val) {
this->description = val;
}

String CharacterConfigAssembled::get_description() const {
return this->description;
}

void CharacterConfigAssembled::set_creators(const Array& val) {
this->creators = val;
}

Array CharacterConfigAssembled::get_creators() const {
return this->creators;
}

void CharacterConfigAssembled::set_sellerFeeBasisPoints(const int32_t& val) {
this->sellerFeeBasisPoints = val;
}

int32_t CharacterConfigAssembled::get_sellerFeeBasisPoints() const {
return this->sellerFeeBasisPoints;
}

void CharacterConfigAssembled::set_collectionName(const String& val) {
this->collectionName = val;
}

String CharacterConfigAssembled::get_collectionName() const {
return this->collectionName;
}

void CharacterConfigAssembled::set_mintAs(const Variant& val) {
this->mintAs = val;
}

Variant CharacterConfigAssembled::get_mintAs() const {
return this->mintAs;
}

void CharacterConfigAssembled::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &CharacterConfigAssembled::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &CharacterConfigAssembled::set_kind);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_assemblerConfig"), &CharacterConfigAssembled::get_assemblerConfig);
ClassDB::bind_method(D_METHOD("set_assemblerConfig", "value"), &CharacterConfigAssembled::set_assemblerConfig);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::OBJECT, "assemblerConfig"), "set_assemblerConfig", "get_assemblerConfig");
ClassDB::bind_method(D_METHOD("get_name"), &CharacterConfigAssembled::get_name);
ClassDB::bind_method(D_METHOD("set_name", "value"), &CharacterConfigAssembled::set_name);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
ClassDB::bind_method(D_METHOD("get_symbol"), &CharacterConfigAssembled::get_symbol);
ClassDB::bind_method(D_METHOD("set_symbol", "value"), &CharacterConfigAssembled::set_symbol);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::STRING, "symbol"), "set_symbol", "get_symbol");
ClassDB::bind_method(D_METHOD("get_description"), &CharacterConfigAssembled::get_description);
ClassDB::bind_method(D_METHOD("set_description", "value"), &CharacterConfigAssembled::set_description);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::STRING, "description"), "set_description", "get_description");
ClassDB::bind_method(D_METHOD("get_creators"), &CharacterConfigAssembled::get_creators);
ClassDB::bind_method(D_METHOD("set_creators", "value"), &CharacterConfigAssembled::set_creators);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::ARRAY, "creators"), "set_creators", "get_creators");
ClassDB::bind_method(D_METHOD("get_sellerFeeBasisPoints"), &CharacterConfigAssembled::get_sellerFeeBasisPoints);
ClassDB::bind_method(D_METHOD("set_sellerFeeBasisPoints", "value"), &CharacterConfigAssembled::set_sellerFeeBasisPoints);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::INT, "sellerFeeBasisPoints"), "set_sellerFeeBasisPoints", "get_sellerFeeBasisPoints");
ClassDB::bind_method(D_METHOD("get_collectionName"), &CharacterConfigAssembled::get_collectionName);
ClassDB::bind_method(D_METHOD("set_collectionName", "value"), &CharacterConfigAssembled::set_collectionName);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::STRING, "collectionName"), "set_collectionName", "get_collectionName");
ClassDB::bind_method(D_METHOD("get_mintAs"), &CharacterConfigAssembled::get_mintAs);
ClassDB::bind_method(D_METHOD("set_mintAs", "value"), &CharacterConfigAssembled::set_mintAs);
ClassDB::add_property("CharacterConfigAssembled", PropertyInfo(Variant::Type::OBJECT, "mintAs"), "set_mintAs", "get_mintAs");
ClassDB::bind_method(D_METHOD("to_dict"), &CharacterConfigAssembled::to_dict);
}
Dictionary CharacterConfigAssembled::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
if (assemblerConfig.has_method("to_string")) {
res["assemblerConfig"] = assemblerConfig.call("to_string");
} else {
res["assemblerConfig"] = assemblerConfig;
}
if (!name.is_empty()) {
    res["name"] = name;
}
if (!symbol.is_empty()) {
    res["symbol"] = symbol;
}
if (!description.is_empty()) {
    res["description"] = description;
}
res["creators"] = creators;
res["sellerFeeBasisPoints"] = sellerFeeBasisPoints;
if (!collectionName.is_empty()) {
    res["collectionName"] = collectionName;
}

          if (mintAs.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::MintAs>(mintAs);
              if (ptr) {
                  res["mintAs"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot