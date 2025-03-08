#include "honeycomb/types/ResourceBalance_generated.hpp"

#include "honeycomb/enums_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceBalance::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray ResourceBalance::get_address() const {
return this->address;
}

void ResourceBalance::set_amount(const String& val) {
this->amount = val;
}

String ResourceBalance::get_amount() const {
return this->amount;
}

void ResourceBalance::set_mint(const Variant& val) {
this->mint = val;
}

Variant ResourceBalance::get_mint() const {
return this->mint;
}

void ResourceBalance::set_wallet(const String& val) {
this->wallet = val;
}

String ResourceBalance::get_wallet() const {
return this->wallet;
}

void ResourceBalance::set_resourceStorage(const String& val) {
    this->resourceStorage = val;
}

String ResourceBalance::get_resourceStorage() const {
    return this->resourceStorage;
}

void ResourceBalance::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &ResourceBalance::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &ResourceBalance::set_address);
ClassDB::add_property("ResourceBalance", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_amount"), &ResourceBalance::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &ResourceBalance::set_amount);
ClassDB::add_property("ResourceBalance", PropertyInfo(Variant::Type::STRING, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("get_mint"), &ResourceBalance::get_mint);
ClassDB::bind_method(D_METHOD("set_mint", "value"), &ResourceBalance::set_mint);
ClassDB::add_property("ResourceBalance", PropertyInfo(Variant::Type::OBJECT, "mint"), "set_mint", "get_mint");
ClassDB::bind_method(D_METHOD("get_wallet"), &ResourceBalance::get_wallet);
ClassDB::bind_method(D_METHOD("set_wallet", "value"), &ResourceBalance::set_wallet);
ClassDB::add_property("ResourceBalance", PropertyInfo(Variant::Type::STRING, "wallet"), "set_wallet", "get_wallet");
ClassDB::bind_method(D_METHOD("get_resourceStorage"), &ResourceBalance::get_resourceStorage);
ClassDB::bind_method(D_METHOD("set_resourceStorage", "value"), &ResourceBalance::set_resourceStorage);
ClassDB::add_property("ResourceBalance", PropertyInfo(Variant::STRING, "resourceStorage"), "set_resourceStorage", "get_resourceStorage");
ClassDB::bind_method(D_METHOD("to_dict"), &ResourceBalance::to_dict);
}
Dictionary ResourceBalance::to_dict() {
Dictionary res;
res["address"] = address;
if (!amount.is_empty()) {
    res["amount"] = amount;
}
if (mint.has_method("to_string")) {
res["mint"] = mint.call("to_string");
} else {
res["mint"] = mint;
}
if (!wallet.is_empty()) {
    res["wallet"] = wallet;
}
res["resourceStorage"] = this->resourceStorage;
return res;
}
} // namespace honeycomb_resource
} // namespace godot