#include "honeycomb/types/MintAsParamsMplBubblegum_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void MintAsParamsMplBubblegum::set_merkleTree(const Variant& val) {
this->merkleTree = val;
}

Variant MintAsParamsMplBubblegum::get_merkleTree() const {
return this->merkleTree;
}

void MintAsParamsMplBubblegum::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_merkleTree"), &MintAsParamsMplBubblegum::get_merkleTree);
ClassDB::bind_method(D_METHOD("set_merkleTree", "value"), &MintAsParamsMplBubblegum::set_merkleTree);
ClassDB::add_property("MintAsParamsMplBubblegum", PropertyInfo(Variant::Type::OBJECT, "merkleTree"), "set_merkleTree", "get_merkleTree");
ClassDB::bind_method(D_METHOD("to_dict"), &MintAsParamsMplBubblegum::to_dict);
}
Dictionary MintAsParamsMplBubblegum::to_dict() {
Dictionary res;
if (merkleTree.has_method("to_string")) {
res["merkleTree"] = merkleTree.call("to_string");
} else {
res["merkleTree"] = merkleTree;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot