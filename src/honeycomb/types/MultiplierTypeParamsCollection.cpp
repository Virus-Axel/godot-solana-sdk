#include "honeycomb/types/MultiplierTypeParamsCollection.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void MultiplierTypeParamsCollection::set_collection(const Variant& val) {
this->collection = val;
}

Variant MultiplierTypeParamsCollection::get_collection() const {
return this->collection;
}

void MultiplierTypeParamsCollection::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_collection"), &MultiplierTypeParamsCollection::get_collection);
ClassDB::bind_method(D_METHOD("set_collection", "value"), &MultiplierTypeParamsCollection::set_collection);
ClassDB::add_property("MultiplierTypeParamsCollection", PropertyInfo(Variant::Type::OBJECT, "collection"), "set_collection", "get_collection");
ClassDB::bind_method(D_METHOD("to_dict"), &MultiplierTypeParamsCollection::to_dict);
}
Dictionary MultiplierTypeParamsCollection::to_dict() {
Dictionary res;
if (collection.has_method("to_string")) {
res["collection"] = collection.call("to_string");
} else {
res["collection"] = collection;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot