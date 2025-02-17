#include "honeycomb/types/CivicInfoInput.hpp"

#include "honeycomb/enums.hpp"

namespace godot {
namespace honeycomb_resource {

void CivicInfoInput::set_gatekeeperNetwork(const String& val) {
    this->gatekeeperNetwork = val;
}

String CivicInfoInput::get_gatekeeperNetwork() const {
    return this->gatekeeperNetwork;
}

void CivicInfoInput::set_wallet(const String& val) {
this->wallet = val;
}

String CivicInfoInput::get_wallet() const {
return this->wallet;
}

void CivicInfoInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_gatekeeperNetwork"), &CivicInfoInput::get_gatekeeperNetwork);
ClassDB::bind_method(D_METHOD("set_gatekeeperNetwork", "value"), &CivicInfoInput::set_gatekeeperNetwork);
ClassDB::add_property("CivicInfoInput", PropertyInfo(Variant::STRING, "gatekeeperNetwork"), "set_gatekeeperNetwork", "get_gatekeeperNetwork");
ClassDB::bind_method(D_METHOD("get_wallet"), &CivicInfoInput::get_wallet);
ClassDB::bind_method(D_METHOD("set_wallet", "value"), &CivicInfoInput::set_wallet);
ClassDB::add_property("CivicInfoInput", PropertyInfo(Variant::Type::STRING, "wallet"), "set_wallet", "get_wallet");
ClassDB::bind_method(D_METHOD("to_dict"), &CivicInfoInput::to_dict);
}
Dictionary CivicInfoInput::to_dict() {
Dictionary res;
res["gatekeeperNetwork"] = this->gatekeeperNetwork;
if (!wallet.is_empty()) {
    res["wallet"] = wallet;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot