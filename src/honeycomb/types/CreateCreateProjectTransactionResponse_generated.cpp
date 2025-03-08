#include "honeycomb/types/CreateCreateProjectTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateCreateProjectTransactionResponse::set_tx(const Variant &val) {
	this->tx = val;
}

Variant CreateCreateProjectTransactionResponse::get_tx() const {
	return this->tx;
}

void CreateCreateProjectTransactionResponse::set_project(const String &val) {
	this->project = val;
}

String CreateCreateProjectTransactionResponse::get_project() const {
	return this->project;
}

void CreateCreateProjectTransactionResponse::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_tx"), &CreateCreateProjectTransactionResponse::get_tx);
	ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateCreateProjectTransactionResponse::set_tx);
	ClassDB::add_property("CreateCreateProjectTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
	ClassDB::bind_method(D_METHOD("get_project"), &CreateCreateProjectTransactionResponse::get_project);
	ClassDB::bind_method(D_METHOD("set_project", "value"), &CreateCreateProjectTransactionResponse::set_project);
	ClassDB::add_property("CreateCreateProjectTransactionResponse", PropertyInfo(Variant::Type::STRING, "project"), "set_project", "get_project");
	ClassDB::bind_method(D_METHOD("to_dict"), &CreateCreateProjectTransactionResponse::to_dict);
}
Dictionary CreateCreateProjectTransactionResponse::to_dict() {
	Dictionary res;

	if (tx.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
		if (ptr) {
			res["tx"] = ptr->to_dict();
		}
	}
	if (!project.is_empty()) {
		res["project"] = project;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot