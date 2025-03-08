#include "honeycomb/types/AssemblerConfigInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void AssemblerConfigInput::set_assemblerConfig(const String &val) {
	this->assemblerConfig = val;
}

String AssemblerConfigInput::get_assemblerConfig() const {
	return this->assemblerConfig;
}

void AssemblerConfigInput::set_name(const String &val) {
	this->name = val;
}

String AssemblerConfigInput::get_name() const {
	return this->name;
}

void AssemblerConfigInput::set_symbol(const String &val) {
	this->symbol = val;
}

String AssemblerConfigInput::get_symbol() const {
	return this->symbol;
}

void AssemblerConfigInput::set_description(const String &val) {
	this->description = val;
}

String AssemblerConfigInput::get_description() const {
	return this->description;
}

void AssemblerConfigInput::set_creators(const Array &val) {
	this->creators = val;
}

Array AssemblerConfigInput::get_creators() const {
	return this->creators;
}

void AssemblerConfigInput::set_sellerFeeBasisPoints(const int32_t &val) {
	this->sellerFeeBasisPoints = val;
}

int32_t AssemblerConfigInput::get_sellerFeeBasisPoints() const {
	return this->sellerFeeBasisPoints;
}

void AssemblerConfigInput::set_collectionName(const String &val) {
	this->collectionName = val;
}

String AssemblerConfigInput::get_collectionName() const {
	return this->collectionName;
}

void AssemblerConfigInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_assemblerConfig"), &AssemblerConfigInput::get_assemblerConfig);
	ClassDB::bind_method(D_METHOD("set_assemblerConfig", "value"), &AssemblerConfigInput::set_assemblerConfig);
	ClassDB::add_property("AssemblerConfigInput", PropertyInfo(Variant::Type::STRING, "assemblerConfig"), "set_assemblerConfig", "get_assemblerConfig");
	ClassDB::bind_method(D_METHOD("get_name"), &AssemblerConfigInput::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "value"), &AssemblerConfigInput::set_name);
	ClassDB::add_property("AssemblerConfigInput", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
	ClassDB::bind_method(D_METHOD("get_symbol"), &AssemblerConfigInput::get_symbol);
	ClassDB::bind_method(D_METHOD("set_symbol", "value"), &AssemblerConfigInput::set_symbol);
	ClassDB::add_property("AssemblerConfigInput", PropertyInfo(Variant::Type::STRING, "symbol"), "set_symbol", "get_symbol");
	ClassDB::bind_method(D_METHOD("get_description"), &AssemblerConfigInput::get_description);
	ClassDB::bind_method(D_METHOD("set_description", "value"), &AssemblerConfigInput::set_description);
	ClassDB::add_property("AssemblerConfigInput", PropertyInfo(Variant::Type::STRING, "description"), "set_description", "get_description");
	ClassDB::bind_method(D_METHOD("get_creators"), &AssemblerConfigInput::get_creators);
	ClassDB::bind_method(D_METHOD("set_creators", "value"), &AssemblerConfigInput::set_creators);
	ClassDB::add_property("AssemblerConfigInput", PropertyInfo(Variant::Type::ARRAY, "creators"), "set_creators", "get_creators");
	ClassDB::bind_method(D_METHOD("get_sellerFeeBasisPoints"), &AssemblerConfigInput::get_sellerFeeBasisPoints);
	ClassDB::bind_method(D_METHOD("set_sellerFeeBasisPoints", "value"), &AssemblerConfigInput::set_sellerFeeBasisPoints);
	ClassDB::add_property("AssemblerConfigInput", PropertyInfo(Variant::Type::INT, "sellerFeeBasisPoints"), "set_sellerFeeBasisPoints", "get_sellerFeeBasisPoints");
	ClassDB::bind_method(D_METHOD("get_collectionName"), &AssemblerConfigInput::get_collectionName);
	ClassDB::bind_method(D_METHOD("set_collectionName", "value"), &AssemblerConfigInput::set_collectionName);
	ClassDB::add_property("AssemblerConfigInput", PropertyInfo(Variant::Type::STRING, "collectionName"), "set_collectionName", "get_collectionName");
	ClassDB::bind_method(D_METHOD("to_dict"), &AssemblerConfigInput::to_dict);
}
Dictionary AssemblerConfigInput::to_dict() {
	Dictionary res;
	if (!assemblerConfig.is_empty()) {
		res["assemblerConfig"] = assemblerConfig;
	}
	if (!name.is_empty()) {
		res["name"] = name;
	}
	if (!symbol.is_empty()) {
		res["symbol"] = symbol;
	}
	if (!description.is_empty()) {
		res["description"] = description;
	}
	res["creators"] = creators;
	res["sellerFeeBasisPoints"] = sellerFeeBasisPoints;
	if (!collectionName.is_empty()) {
		res["collectionName"] = collectionName;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot