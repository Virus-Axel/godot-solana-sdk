#include "honeycomb/types/UpdateMissionInput.hpp"

#include "honeycomb/types/NewMissionCost.hpp"
namespace godot{
namespace honeycomb_resource{

void UpdateMissionInput::set_cost(const Variant& val){
this->cost = val;
}

Variant UpdateMissionInput::get_cost(){
return this->cost;
}

void UpdateMissionInput::set_minXp(const int64_t& val){
this->minXp = val;
}

int64_t UpdateMissionInput::get_minXp(){
return this->minXp;
}

void UpdateMissionInput::set_duration(const int64_t& val){
this->duration = val;
}

int64_t UpdateMissionInput::get_duration(){
return this->duration;
}

void UpdateMissionInput::set_removeRewards(const PackedInt32Array& val){
this->removeRewards = val;
}

PackedInt32Array UpdateMissionInput::get_removeRewards(){
return this->removeRewards;
}

void UpdateMissionInput::set_newRewards(const Array& val){
this->newRewards = val;
}

Array UpdateMissionInput::get_newRewards(){
return this->newRewards;
}

void UpdateMissionInput::set_updateRewards(const Array& val){
this->updateRewards = val;
}

Array UpdateMissionInput::get_updateRewards(){
return this->updateRewards;
}

Dictionary UpdateMissionInput::to_dict(){
Dictionary res;
res["cost"] = Object::cast_to<godot::honeycomb_resource::NewMissionCost>(cost)->to_dict();
res["minXp"] = minXp;
res["duration"] = duration;
res["removeRewards"] = removeRewards;
res["newRewards"] = newRewards;
res["updateRewards"] = updateRewards;
return res;
}

void UpdateMissionInput::from_dict(const Dictionary& dict){
Object::cast_to<godot::honeycomb_resource::NewMissionCost>(cost)->from_dict(dict["cost"]);
minXp = dict["minXp"];
duration = dict["duration"];
removeRewards = dict["removeRewards"];
newRewards = dict["newRewards"];
updateRewards = dict["updateRewards"];
}

void UpdateMissionInput::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_cost"), &UpdateMissionInput::get_cost);
ClassDB::bind_method(D_METHOD("set_cost", "value"), &UpdateMissionInput::set_cost);
ClassDB::add_property("UpdateMissionInput", PropertyInfo(Variant::Type::OBJECT, "cost"), "set_cost", "get_cost");
ClassDB::bind_method(D_METHOD("get_minXp"), &UpdateMissionInput::get_minXp);
ClassDB::bind_method(D_METHOD("set_minXp", "value"), &UpdateMissionInput::set_minXp);
ClassDB::add_property("UpdateMissionInput", PropertyInfo(Variant::Type::INT, "minXp"), "set_minXp", "get_minXp");
ClassDB::bind_method(D_METHOD("get_duration"), &UpdateMissionInput::get_duration);
ClassDB::bind_method(D_METHOD("set_duration", "value"), &UpdateMissionInput::set_duration);
ClassDB::add_property("UpdateMissionInput", PropertyInfo(Variant::Type::INT, "duration"), "set_duration", "get_duration");
ClassDB::bind_method(D_METHOD("get_removeRewards"), &UpdateMissionInput::get_removeRewards);
ClassDB::bind_method(D_METHOD("set_removeRewards", "value"), &UpdateMissionInput::set_removeRewards);
ClassDB::add_property("UpdateMissionInput", PropertyInfo(Variant::Type::PACKED_INT32_ARRAY, "removeRewards"), "set_removeRewards", "get_removeRewards");
ClassDB::bind_method(D_METHOD("get_newRewards"), &UpdateMissionInput::get_newRewards);
ClassDB::bind_method(D_METHOD("set_newRewards", "value"), &UpdateMissionInput::set_newRewards);
ClassDB::add_property("UpdateMissionInput", PropertyInfo(Variant::Type::ARRAY, "newRewards"), "set_newRewards", "get_newRewards");
ClassDB::bind_method(D_METHOD("get_updateRewards"), &UpdateMissionInput::get_updateRewards);
ClassDB::bind_method(D_METHOD("set_updateRewards", "value"), &UpdateMissionInput::set_updateRewards);
ClassDB::add_property("UpdateMissionInput", PropertyInfo(Variant::Type::ARRAY, "updateRewards"), "set_updateRewards", "get_updateRewards");
ClassDB::bind_method(D_METHOD("to_dict"), &UpdateMissionInput::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &UpdateMissionInput::from_dict);
}
} // honeycomb_resource
} // godot