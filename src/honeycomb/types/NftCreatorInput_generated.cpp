#include "honeycomb/types/NftCreatorInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void NftCreatorInput::set_address(const String& val) {
this->address = val;
}

String NftCreatorInput::get_address() const {
return this->address;
}

void NftCreatorInput::set_share(const int32_t& val) {
this->share = val;
}

int32_t NftCreatorInput::get_share() const {
return this->share;
}

void NftCreatorInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &NftCreatorInput::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &NftCreatorInput::set_address);
ClassDB::add_property("NftCreatorInput", PropertyInfo(Variant::Type::STRING, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_share"), &NftCreatorInput::get_share);
ClassDB::bind_method(D_METHOD("set_share", "value"), &NftCreatorInput::set_share);
ClassDB::add_property("NftCreatorInput", PropertyInfo(Variant::Type::INT, "share"), "set_share", "get_share");
ClassDB::bind_method(D_METHOD("to_dict"), &NftCreatorInput::to_dict);
}
Dictionary NftCreatorInput::to_dict() {
Dictionary res;
if (!address.is_empty()) {
    res["address"] = address;
}
res["share"] = share;
return res;
}
} // namespace honeycomb_resource
} // namespace godot