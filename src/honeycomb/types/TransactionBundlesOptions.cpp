#include "honeycomb/types/TransactionBundlesOptions.hpp"

namespace godot{
namespace honeycomb_resource{

void TransactionBundlesOptions::set_bundleSize(const int32_t& val){
this->bundleSize = val;
}

int32_t TransactionBundlesOptions::get_bundleSize(){
return this->bundleSize;
}

void TransactionBundlesOptions::set_firstBundleSize(const int32_t& val){
this->firstBundleSize = val;
}

int32_t TransactionBundlesOptions::get_firstBundleSize(){
return this->firstBundleSize;
}

void TransactionBundlesOptions::set_lastBundleSize(const int32_t& val){
this->lastBundleSize = val;
}

int32_t TransactionBundlesOptions::get_lastBundleSize(){
return this->lastBundleSize;
}

Dictionary TransactionBundlesOptions::to_dict(){
Dictionary res;
res["bundleSize"] = bundleSize;
res["firstBundleSize"] = firstBundleSize;
res["lastBundleSize"] = lastBundleSize;
return res;
}

void TransactionBundlesOptions::from_dict(const Dictionary& dict){
bundleSize = dict["bundleSize"];
firstBundleSize = dict["firstBundleSize"];
lastBundleSize = dict["lastBundleSize"];
}

void TransactionBundlesOptions::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_bundleSize"), &TransactionBundlesOptions::get_bundleSize);
ClassDB::bind_method(D_METHOD("set_bundleSize", "value"), &TransactionBundlesOptions::set_bundleSize);
ClassDB::add_property("TransactionBundlesOptions", PropertyInfo(Variant::Type::INT, "bundleSize"), "set_bundleSize", "get_bundleSize");
ClassDB::bind_method(D_METHOD("get_firstBundleSize"), &TransactionBundlesOptions::get_firstBundleSize);
ClassDB::bind_method(D_METHOD("set_firstBundleSize", "value"), &TransactionBundlesOptions::set_firstBundleSize);
ClassDB::add_property("TransactionBundlesOptions", PropertyInfo(Variant::Type::INT, "firstBundleSize"), "set_firstBundleSize", "get_firstBundleSize");
ClassDB::bind_method(D_METHOD("get_lastBundleSize"), &TransactionBundlesOptions::get_lastBundleSize);
ClassDB::bind_method(D_METHOD("set_lastBundleSize", "value"), &TransactionBundlesOptions::set_lastBundleSize);
ClassDB::add_property("TransactionBundlesOptions", PropertyInfo(Variant::Type::INT, "lastBundleSize"), "set_lastBundleSize", "get_lastBundleSize");
ClassDB::bind_method(D_METHOD("to_dict"), &TransactionBundlesOptions::to_dict);
ClassDB::bind_method(D_METHOD("from_dict", "dict"), &TransactionBundlesOptions::from_dict);
}
} // honeycomb_resource
} // godot