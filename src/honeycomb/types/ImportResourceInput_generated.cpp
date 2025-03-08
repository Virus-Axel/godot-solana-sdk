#include "honeycomb/types/ImportResourceInput_generated.hpp"

#include "honeycomb/enums_generated.hpp"
#include "honeycomb/types/ImportResourceInputCustodyInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ImportResourceInput::set_decimals(const int32_t& val) {
this->decimals = val;
}

int32_t ImportResourceInput::get_decimals() const {
return this->decimals;
}

void ImportResourceInput::set_project(const String& val) {
this->project = val;
}

String ImportResourceInput::get_project() const {
return this->project;
}

void ImportResourceInput::set_mint(const String& val) {
this->mint = val;
}

String ImportResourceInput::get_mint() const {
return this->mint;
}

void ImportResourceInput::set_authority(const String& val) {
this->authority = val;
}

String ImportResourceInput::get_authority() const {
return this->authority;
}

void ImportResourceInput::set_storage(const String& val) {
    this->storage = val;
}

String ImportResourceInput::get_storage() const {
    return this->storage;
}

void ImportResourceInput::set_tags(const PackedStringArray& val) {
this->tags = val;
}

PackedStringArray ImportResourceInput::get_tags() const {
return this->tags;
}

void ImportResourceInput::set_custody(const Variant& val) {
this->custody = val;
}

Variant ImportResourceInput::get_custody() const {
return this->custody;
}

void ImportResourceInput::set_payer(const String& val) {
this->payer = val;
}

String ImportResourceInput::get_payer() const {
return this->payer;
}

void ImportResourceInput::set_delegateAuthority(const String& val) {
this->delegateAuthority = val;
}

String ImportResourceInput::get_delegateAuthority() const {
return this->delegateAuthority;
}

void ImportResourceInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_decimals"), &ImportResourceInput::get_decimals);
ClassDB::bind_method(D_METHOD("set_decimals", "value"), &ImportResourceInput::set_decimals);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::Type::INT, "decimals"), "set_decimals", "get_decimals");
ClassDB::bind_method(D_METHOD("get_project"), &ImportResourceInput::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &ImportResourceInput::set_project);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::Type::STRING, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_mint"), &ImportResourceInput::get_mint);
ClassDB::bind_method(D_METHOD("set_mint", "value"), &ImportResourceInput::set_mint);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::Type::STRING, "mint"), "set_mint", "get_mint");
ClassDB::bind_method(D_METHOD("get_authority"), &ImportResourceInput::get_authority);
ClassDB::bind_method(D_METHOD("set_authority", "value"), &ImportResourceInput::set_authority);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::Type::STRING, "authority"), "set_authority", "get_authority");
ClassDB::bind_method(D_METHOD("get_storage"), &ImportResourceInput::get_storage);
ClassDB::bind_method(D_METHOD("set_storage", "value"), &ImportResourceInput::set_storage);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::STRING, "storage"), "set_storage", "get_storage");
ClassDB::bind_method(D_METHOD("get_tags"), &ImportResourceInput::get_tags);
ClassDB::bind_method(D_METHOD("set_tags", "value"), &ImportResourceInput::set_tags);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "tags"), "set_tags", "get_tags");
ClassDB::bind_method(D_METHOD("get_custody"), &ImportResourceInput::get_custody);
ClassDB::bind_method(D_METHOD("set_custody", "value"), &ImportResourceInput::set_custody);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::Type::OBJECT, "custody"), "set_custody", "get_custody");
ClassDB::bind_method(D_METHOD("get_payer"), &ImportResourceInput::get_payer);
ClassDB::bind_method(D_METHOD("set_payer", "value"), &ImportResourceInput::set_payer);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::Type::STRING, "payer"), "set_payer", "get_payer");
ClassDB::bind_method(D_METHOD("get_delegateAuthority"), &ImportResourceInput::get_delegateAuthority);
ClassDB::bind_method(D_METHOD("set_delegateAuthority", "value"), &ImportResourceInput::set_delegateAuthority);
ClassDB::add_property("ImportResourceInput", PropertyInfo(Variant::Type::STRING, "delegateAuthority"), "set_delegateAuthority", "get_delegateAuthority");
ClassDB::bind_method(D_METHOD("to_dict"), &ImportResourceInput::to_dict);
}
Dictionary ImportResourceInput::to_dict() {
Dictionary res;
res["decimals"] = decimals;
if (!project.is_empty()) {
    res["project"] = project;
}
if (!mint.is_empty()) {
    res["mint"] = mint;
}
if (!authority.is_empty()) {
    res["authority"] = authority;
}
res["storage"] = this->storage;
res["tags"] = tags;

          if (custody.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ImportResourceInputCustodyInput>(custody);
              if (ptr) {
                  res["custody"] = ptr->to_dict();
              }
          }
if (!payer.is_empty()) {
    res["payer"] = payer;
}
if (!delegateAuthority.is_empty()) {
    res["delegateAuthority"] = delegateAuthority;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot