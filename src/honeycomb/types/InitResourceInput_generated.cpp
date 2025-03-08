#include "honeycomb/types/InitResourceInput_generated.hpp"

#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void InitResourceInput::set_name(const String &val) {
	this->name = val;
}

String InitResourceInput::get_name() const {
	return this->name;
}

void InitResourceInput::set_symbol(const String &val) {
	this->symbol = val;
}

String InitResourceInput::get_symbol() const {
	return this->symbol;
}

void InitResourceInput::set_uri(const String &val) {
	this->uri = val;
}

String InitResourceInput::get_uri() const {
	return this->uri;
}

void InitResourceInput::set_decimals(const int32_t &val) {
	this->decimals = val;
}

int32_t InitResourceInput::get_decimals() const {
	return this->decimals;
}

void InitResourceInput::set_tags(const PackedStringArray &val) {
	this->tags = val;
}

PackedStringArray InitResourceInput::get_tags() const {
	return this->tags;
}

void InitResourceInput::set_storage(const String &val) {
	this->storage = val;
}

String InitResourceInput::get_storage() const {
	return this->storage;
}

void InitResourceInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &InitResourceInput::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "value"), &InitResourceInput::set_name);
	ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
	ClassDB::bind_method(D_METHOD("get_symbol"), &InitResourceInput::get_symbol);
	ClassDB::bind_method(D_METHOD("set_symbol", "value"), &InitResourceInput::set_symbol);
	ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::STRING, "symbol"), "set_symbol", "get_symbol");
	ClassDB::bind_method(D_METHOD("get_uri"), &InitResourceInput::get_uri);
	ClassDB::bind_method(D_METHOD("set_uri", "value"), &InitResourceInput::set_uri);
	ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::STRING, "uri"), "set_uri", "get_uri");
	ClassDB::bind_method(D_METHOD("get_decimals"), &InitResourceInput::get_decimals);
	ClassDB::bind_method(D_METHOD("set_decimals", "value"), &InitResourceInput::set_decimals);
	ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::INT, "decimals"), "set_decimals", "get_decimals");
	ClassDB::bind_method(D_METHOD("get_tags"), &InitResourceInput::get_tags);
	ClassDB::bind_method(D_METHOD("set_tags", "value"), &InitResourceInput::set_tags);
	ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "tags"), "set_tags", "get_tags");
	ClassDB::bind_method(D_METHOD("get_storage"), &InitResourceInput::get_storage);
	ClassDB::bind_method(D_METHOD("set_storage", "value"), &InitResourceInput::set_storage);
	ClassDB::add_property("InitResourceInput", PropertyInfo(Variant::STRING, "storage"), "set_storage", "get_storage");
	ClassDB::bind_method(D_METHOD("to_dict"), &InitResourceInput::to_dict);
}
Dictionary InitResourceInput::to_dict() {
	Dictionary res;
	if (!name.is_empty()) {
		res["name"] = name;
	}
	if (!symbol.is_empty()) {
		res["symbol"] = symbol;
	}
	if (!uri.is_empty()) {
		res["uri"] = uri;
	}
	res["decimals"] = decimals;
	res["tags"] = tags;
	res["storage"] = this->storage;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot