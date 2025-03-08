#include "honeycomb/types/Wallets_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void Wallets::set_shadow(const Variant &val) {
	this->shadow = val;
}

Variant Wallets::get_shadow() const {
	return this->shadow;
}

void Wallets::set_wallets(const Array &val) {
	this->wallets = val;
}

Array Wallets::get_wallets() const {
	return this->wallets;
}

void Wallets::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_shadow"), &Wallets::get_shadow);
	ClassDB::bind_method(D_METHOD("set_shadow", "value"), &Wallets::set_shadow);
	ClassDB::add_property("Wallets", PropertyInfo(Variant::Type::OBJECT, "shadow"), "set_shadow", "get_shadow");
	ClassDB::bind_method(D_METHOD("get_wallets"), &Wallets::get_wallets);
	ClassDB::bind_method(D_METHOD("set_wallets", "value"), &Wallets::set_wallets);
	ClassDB::add_property("Wallets", PropertyInfo(Variant::Type::ARRAY, "wallets"), "set_wallets", "get_wallets");
	ClassDB::bind_method(D_METHOD("to_dict"), &Wallets::to_dict);
}
Dictionary Wallets::to_dict() {
	Dictionary res;
	if (shadow.has_method("to_string")) {
		res["shadow"] = shadow.call("to_string");
	} else {
		res["shadow"] = shadow;
	}
	res["wallets"] = wallets;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot