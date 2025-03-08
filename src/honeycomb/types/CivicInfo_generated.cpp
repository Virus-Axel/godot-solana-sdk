#include "honeycomb/types/CivicInfo_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CivicInfo::set_gatekeeperNetwork(const String& val) {
    this->gatekeeperNetwork = val;
}

String CivicInfo::get_gatekeeperNetwork() const {
    return this->gatekeeperNetwork;
}

void CivicInfo::set_expiry(const String& val) {
this->expiry = val;
}

String CivicInfo::get_expiry() const {
return this->expiry;
}

void CivicInfo::set_walletIndex(const int32_t& val) {
this->walletIndex = val;
}

int32_t CivicInfo::get_walletIndex() const {
return this->walletIndex;
}

void CivicInfo::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_gatekeeperNetwork"), &CivicInfo::get_gatekeeperNetwork);
ClassDB::bind_method(D_METHOD("set_gatekeeperNetwork", "value"), &CivicInfo::set_gatekeeperNetwork);
ClassDB::add_property("CivicInfo", PropertyInfo(Variant::STRING, "gatekeeperNetwork"), "set_gatekeeperNetwork", "get_gatekeeperNetwork");
ClassDB::bind_method(D_METHOD("get_expiry"), &CivicInfo::get_expiry);
ClassDB::bind_method(D_METHOD("set_expiry", "value"), &CivicInfo::set_expiry);
ClassDB::add_property("CivicInfo", PropertyInfo(Variant::Type::STRING, "expiry"), "set_expiry", "get_expiry");
ClassDB::bind_method(D_METHOD("get_walletIndex"), &CivicInfo::get_walletIndex);
ClassDB::bind_method(D_METHOD("set_walletIndex", "value"), &CivicInfo::set_walletIndex);
ClassDB::add_property("CivicInfo", PropertyInfo(Variant::Type::INT, "walletIndex"), "set_walletIndex", "get_walletIndex");
ClassDB::bind_method(D_METHOD("to_dict"), &CivicInfo::to_dict);
}
Dictionary CivicInfo::to_dict() {
Dictionary res;
res["gatekeeperNetwork"] = this->gatekeeperNetwork;
if (!expiry.is_empty()) {
    res["expiry"] = expiry;
}
res["walletIndex"] = walletIndex;
return res;
}
} // namespace honeycomb_resource
} // namespace godot