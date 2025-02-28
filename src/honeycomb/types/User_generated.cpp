#include "honeycomb/types/User_generated.hpp"

#include "honeycomb/types/Proof_generated.hpp"
#include "honeycomb/types/HoneycombUserInfo_generated.hpp"
#include "honeycomb/types/SocialInfo_generated.hpp"
#include "honeycomb/types/Wallets_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void User::set_id(const int32_t& val) {
this->id = val;
}

int32_t User::get_id() const {
return this->id;
}

void User::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray User::get_address() const {
return this->address;
}

void User::set_tree_id(const PackedByteArray& val) {
this->tree_id = val;
}

PackedByteArray User::get_tree_id() const {
return this->tree_id;
}

void User::set_leaf_idx(const int64_t& val) {
this->leaf_idx = val;
}

int64_t User::get_leaf_idx() const {
return this->leaf_idx;
}

void User::set_proof(const Variant& val) {
this->proof = val;
}

Variant User::get_proof() const {
return this->proof;
}

void User::set_info(const Variant& val) {
this->info = val;
}

Variant User::get_info() const {
return this->info;
}

void User::set_socialInfo(const Variant& val) {
this->socialInfo = val;
}

Variant User::get_socialInfo() const {
return this->socialInfo;
}

void User::set_wallets(const Variant& val) {
this->wallets = val;
}

Variant User::get_wallets() const {
return this->wallets;
}

void User::set_profiles(const Array& val) {
this->profiles = val;
}

Array User::get_profiles() const {
return this->profiles;
}

void User::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_id"), &User::get_id);
ClassDB::bind_method(D_METHOD("set_id", "value"), &User::set_id);
ClassDB::add_property("User", PropertyInfo(Variant::Type::INT, "id"), "set_id", "get_id");
ClassDB::bind_method(D_METHOD("get_address"), &User::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &User::set_address);
ClassDB::add_property("User", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_tree_id"), &User::get_tree_id);
ClassDB::bind_method(D_METHOD("set_tree_id", "value"), &User::set_tree_id);
ClassDB::add_property("User", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree_id"), "set_tree_id", "get_tree_id");
ClassDB::bind_method(D_METHOD("get_leaf_idx"), &User::get_leaf_idx);
ClassDB::bind_method(D_METHOD("set_leaf_idx", "value"), &User::set_leaf_idx);
ClassDB::add_property("User", PropertyInfo(Variant::Type::INT, "leaf_idx"), "set_leaf_idx", "get_leaf_idx");
ClassDB::bind_method(D_METHOD("get_proof"), &User::get_proof);
ClassDB::bind_method(D_METHOD("set_proof", "value"), &User::set_proof);
ClassDB::add_property("User", PropertyInfo(Variant::Type::OBJECT, "proof"), "set_proof", "get_proof");
ClassDB::bind_method(D_METHOD("get_info"), &User::get_info);
ClassDB::bind_method(D_METHOD("set_info", "value"), &User::set_info);
ClassDB::add_property("User", PropertyInfo(Variant::Type::OBJECT, "info"), "set_info", "get_info");
ClassDB::bind_method(D_METHOD("get_socialInfo"), &User::get_socialInfo);
ClassDB::bind_method(D_METHOD("set_socialInfo", "value"), &User::set_socialInfo);
ClassDB::add_property("User", PropertyInfo(Variant::Type::OBJECT, "socialInfo"), "set_socialInfo", "get_socialInfo");
ClassDB::bind_method(D_METHOD("get_wallets"), &User::get_wallets);
ClassDB::bind_method(D_METHOD("set_wallets", "value"), &User::set_wallets);
ClassDB::add_property("User", PropertyInfo(Variant::Type::OBJECT, "wallets"), "set_wallets", "get_wallets");
ClassDB::bind_method(D_METHOD("get_profiles"), &User::get_profiles);
ClassDB::bind_method(D_METHOD("set_profiles", "value"), &User::set_profiles);
ClassDB::add_property("User", PropertyInfo(Variant::Type::ARRAY, "profiles"), "set_profiles", "get_profiles");
ClassDB::bind_method(D_METHOD("to_dict"), &User::to_dict);
}
Dictionary User::to_dict() {
Dictionary res;
res["id"] = id;
res["address"] = address;
res["tree_id"] = tree_id;
res["leaf_idx"] = leaf_idx;

          if (proof.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::Proof>(proof);
              if (ptr) {
                  res["proof"] = ptr->to_dict();
              }
          }

          if (info.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombUserInfo>(info);
              if (ptr) {
                  res["info"] = ptr->to_dict();
              }
          }

          if (socialInfo.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::SocialInfo>(socialInfo);
              if (ptr) {
                  res["socialInfo"] = ptr->to_dict();
              }
          }

          if (wallets.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::Wallets>(wallets);
              if (ptr) {
                  res["wallets"] = ptr->to_dict();
              }
          }
res["profiles"] = profiles;
return res;
}
} // namespace honeycomb_resource
} // namespace godot