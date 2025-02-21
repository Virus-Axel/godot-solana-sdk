#include "honeycomb/types/ImportResourceInputCustodyInput.hpp"


namespace godot {
namespace honeycomb_resource {

void ImportResourceInputCustodyInput::set_supply(const int64_t& val) {
this->supply = val;
}

int64_t ImportResourceInputCustodyInput::get_supply() const {
return this->supply;
}

void ImportResourceInputCustodyInput::set_burnerDestination(const String& val) {
this->burnerDestination = val;
}

String ImportResourceInputCustodyInput::get_burnerDestination() const {
return this->burnerDestination;
}

void ImportResourceInputCustodyInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_supply"), &ImportResourceInputCustodyInput::get_supply);
ClassDB::bind_method(D_METHOD("set_supply", "value"), &ImportResourceInputCustodyInput::set_supply);
ClassDB::add_property("ImportResourceInputCustodyInput", PropertyInfo(Variant::Type::INT, "supply"), "set_supply", "get_supply");
ClassDB::bind_method(D_METHOD("get_burnerDestination"), &ImportResourceInputCustodyInput::get_burnerDestination);
ClassDB::bind_method(D_METHOD("set_burnerDestination", "value"), &ImportResourceInputCustodyInput::set_burnerDestination);
ClassDB::add_property("ImportResourceInputCustodyInput", PropertyInfo(Variant::Type::STRING, "burnerDestination"), "set_burnerDestination", "get_burnerDestination");
ClassDB::bind_method(D_METHOD("to_dict"), &ImportResourceInputCustodyInput::to_dict);
}
Dictionary ImportResourceInputCustodyInput::to_dict() {
Dictionary res;
res["supply"] = supply;
if (!burnerDestination.is_empty()) {
    res["burnerDestination"] = burnerDestination;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot