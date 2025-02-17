#include "honeycomb/types/AssemblerConfig.hpp"

#include "pubkey.hpp"
#include "honeycomb/types/ControlledMerkleTrees.hpp"

namespace godot {
namespace honeycomb_resource {

void AssemblerConfig::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray AssemblerConfig::get_address() const {
return this->address;
}

void AssemblerConfig::set_program_id(const PackedByteArray& val) {
this->program_id = val;
}

PackedByteArray AssemblerConfig::get_program_id() const {
return this->program_id;
}

void AssemblerConfig::set_discriminator(const PackedByteArray& val) {
this->discriminator = val;
}

PackedByteArray AssemblerConfig::get_discriminator() const {
return this->discriminator;
}

void AssemblerConfig::set_ticker(const String& val) {
this->ticker = val;
}

String AssemblerConfig::get_ticker() const {
return this->ticker;
}

void AssemblerConfig::set_project(const Variant& val) {
this->project = val;
}

Variant AssemblerConfig::get_project() const {
return this->project;
}

void AssemblerConfig::set_order(const PackedStringArray& val) {
this->order = val;
}

PackedStringArray AssemblerConfig::get_order() const {
return this->order;
}

void AssemblerConfig::set_merkle_trees(const Variant& val) {
this->merkle_trees = val;
}

Variant AssemblerConfig::get_merkle_trees() const {
return this->merkle_trees;
}

void AssemblerConfig::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &AssemblerConfig::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &AssemblerConfig::set_address);
ClassDB::add_property("AssemblerConfig", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_program_id"), &AssemblerConfig::get_program_id);
ClassDB::bind_method(D_METHOD("set_program_id", "value"), &AssemblerConfig::set_program_id);
ClassDB::add_property("AssemblerConfig", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "program_id"), "set_program_id", "get_program_id");
ClassDB::bind_method(D_METHOD("get_discriminator"), &AssemblerConfig::get_discriminator);
ClassDB::bind_method(D_METHOD("set_discriminator", "value"), &AssemblerConfig::set_discriminator);
ClassDB::add_property("AssemblerConfig", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "discriminator"), "set_discriminator", "get_discriminator");
ClassDB::bind_method(D_METHOD("get_ticker"), &AssemblerConfig::get_ticker);
ClassDB::bind_method(D_METHOD("set_ticker", "value"), &AssemblerConfig::set_ticker);
ClassDB::add_property("AssemblerConfig", PropertyInfo(Variant::Type::STRING, "ticker"), "set_ticker", "get_ticker");
ClassDB::bind_method(D_METHOD("get_project"), &AssemblerConfig::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &AssemblerConfig::set_project);
ClassDB::add_property("AssemblerConfig", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_order"), &AssemblerConfig::get_order);
ClassDB::bind_method(D_METHOD("set_order", "value"), &AssemblerConfig::set_order);
ClassDB::add_property("AssemblerConfig", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "order"), "set_order", "get_order");
ClassDB::bind_method(D_METHOD("get_merkle_trees"), &AssemblerConfig::get_merkle_trees);
ClassDB::bind_method(D_METHOD("set_merkle_trees", "value"), &AssemblerConfig::set_merkle_trees);
ClassDB::add_property("AssemblerConfig", PropertyInfo(Variant::Type::OBJECT, "merkle_trees"), "set_merkle_trees", "get_merkle_trees");
ClassDB::bind_method(D_METHOD("to_dict"), &AssemblerConfig::to_dict);
}
Dictionary AssemblerConfig::to_dict() {
Dictionary res;
res["address"] = address;
res["program_id"] = program_id;
res["discriminator"] = discriminator;
if (!ticker.is_empty()) {
    res["ticker"] = ticker;
}
if (project.has_method("to_string")) {
res["project"] = project.call("to_string");
} else {
res["project"] = project;
}
res["order"] = order;

          if (merkle_trees.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ControlledMerkleTrees>(merkle_trees);
              if (ptr) {
                  res["merkle_trees"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot