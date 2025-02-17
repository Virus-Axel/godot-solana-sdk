#include "honeycomb/types/HoneycombAccount.hpp"


namespace godot {
namespace honeycomb_resource {

void HoneycombAccount::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray HoneycombAccount::get_address() const {
return this->address;
}

void HoneycombAccount::set_program_id(const PackedByteArray& val) {
this->program_id = val;
}

PackedByteArray HoneycombAccount::get_program_id() const {
return this->program_id;
}

void HoneycombAccount::set_discriminator(const PackedByteArray& val) {
this->discriminator = val;
}

PackedByteArray HoneycombAccount::get_discriminator() const {
return this->discriminator;
}

void HoneycombAccount::set_parsed_data(const Dictionary& val) {
this->parsed_data = val;
}

Dictionary HoneycombAccount::get_parsed_data() const {
return this->parsed_data;
}

void HoneycombAccount::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &HoneycombAccount::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &HoneycombAccount::set_address);
ClassDB::add_property("HoneycombAccount", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_program_id"), &HoneycombAccount::get_program_id);
ClassDB::bind_method(D_METHOD("set_program_id", "value"), &HoneycombAccount::set_program_id);
ClassDB::add_property("HoneycombAccount", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "program_id"), "set_program_id", "get_program_id");
ClassDB::bind_method(D_METHOD("get_discriminator"), &HoneycombAccount::get_discriminator);
ClassDB::bind_method(D_METHOD("set_discriminator", "value"), &HoneycombAccount::set_discriminator);
ClassDB::add_property("HoneycombAccount", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "discriminator"), "set_discriminator", "get_discriminator");
ClassDB::bind_method(D_METHOD("get_parsed_data"), &HoneycombAccount::get_parsed_data);
ClassDB::bind_method(D_METHOD("set_parsed_data", "value"), &HoneycombAccount::set_parsed_data);
ClassDB::add_property("HoneycombAccount", PropertyInfo(Variant::Type::DICTIONARY, "parsed_data"), "set_parsed_data", "get_parsed_data");
ClassDB::bind_method(D_METHOD("to_dict"), &HoneycombAccount::to_dict);
}
Dictionary HoneycombAccount::to_dict() {
Dictionary res;
res["address"] = address;
res["program_id"] = program_id;
res["discriminator"] = discriminator;
res["parsed_data"] = parsed_data;
return res;
}
} // namespace honeycomb_resource
} // namespace godot