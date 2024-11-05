#include "honeycomb/types/SendTransactionBundlesOptions.hpp"

namespace godot{
namespace honeycomb_resource{

void SendTransactionBundlesOptions::set_commitment(const String& val){
this->commitment = val;
}

String& SendTransactionBundlesOptions::get_commitment(){
return this->commitment;
}

void SendTransactionBundlesOptions::set_skipPreflight(const bool& val){
this->skipPreflight = val;
}

bool& SendTransactionBundlesOptions::get_skipPreflight(){
return this->skipPreflight;
}

void SendTransactionBundlesOptions::set_bundles(const Variant& val){
this->bundles = val;
}

Variant& SendTransactionBundlesOptions::get_bundles(){
return this->bundles;
}

Dictionary SendTransactionBundlesOptions::to_dict(){
Dictionary res;
res["commitment"] = commitment;
res["skipPreflight"] = skipPreflight;
res["bundles"] = Object::cast_to<godot::honeycomb_resource::TransactionBundlesOptions>(bundles)->to_dict();
return res;
}

void SendTransactionBundlesOptions::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_commitment"), &SendTransactionBundlesOptions::get_commitment);
ClassDB::bind_method(D_METHOD("set_commitment", "value"), &SendTransactionBundlesOptions::set_commitment);
ClassDB::add_property("SendTransactionBundlesOptions", PropertyInfo(Variant::Type::STRING, "commitment"), "set_commitment", "get_commitment");
ClassDB::bind_method(D_METHOD("get_skipPreflight"), &SendTransactionBundlesOptions::get_skipPreflight);
ClassDB::bind_method(D_METHOD("set_skipPreflight", "value"), &SendTransactionBundlesOptions::set_skipPreflight);
ClassDB::add_property("SendTransactionBundlesOptions", PropertyInfo(Variant::Type::BOOL, "skipPreflight"), "set_skipPreflight", "get_skipPreflight");
ClassDB::bind_method(D_METHOD("get_bundles"), &SendTransactionBundlesOptions::get_bundles);
ClassDB::bind_method(D_METHOD("set_bundles", "value"), &SendTransactionBundlesOptions::set_bundles);
ClassDB::add_property("SendTransactionBundlesOptions", PropertyInfo(Variant::Type::VARIANT, "bundles"), "set_bundles", "get_bundles");
}
} // honeycomb_resource
} // godot