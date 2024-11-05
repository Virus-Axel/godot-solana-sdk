#include "honeycomb/types/UpdateWalletInput.hpp"

namespace godot{
namespace honeycomb_resource{

void UpdateWalletInput::set_add(const PackedStringArray& val){
this->add = val;
}

PackedStringArray UpdateWalletInput::get_add(){
return this->add;
}

void UpdateWalletInput::set_remove(const PackedStringArray& val){
this->remove = val;
}

PackedStringArray UpdateWalletInput::get_remove(){
return this->remove;
}

Dictionary UpdateWalletInput::to_dict(){
Dictionary res;
res["add"] = add;
res["remove"] = remove;
return res;
}

void UpdateWalletInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_add"), &UpdateWalletInput::get_add);
ClassDB::bind_method(D_METHOD("set_add", "value"), &UpdateWalletInput::set_add);
ClassDB::add_property("UpdateWalletInput", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "add"), "set_add", "get_add");
ClassDB::bind_method(D_METHOD("get_remove"), &UpdateWalletInput::get_remove);
ClassDB::bind_method(D_METHOD("set_remove", "value"), &UpdateWalletInput::set_remove);
ClassDB::add_property("UpdateWalletInput", PropertyInfo(Variant::Type::PACKED_STRING_ARRAY, "remove"), "set_remove", "get_remove");
}
} // honeycomb_resource
} // godot