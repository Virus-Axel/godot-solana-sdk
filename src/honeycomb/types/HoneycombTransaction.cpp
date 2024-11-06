#include "honeycomb/types/HoneycombTransaction.hpp"

namespace godot{
namespace honeycomb_resource{

void HoneycombTransaction::set_transaction(const PackedByteArray& val){
this->transaction = val;
}

PackedByteArray HoneycombTransaction::get_transaction(){
return this->transaction;
}

void HoneycombTransaction::set_blockhash(const String& val){
this->blockhash = val;
}

String HoneycombTransaction::get_blockhash(){
return this->blockhash;
}

void HoneycombTransaction::set_lastValidBlockHeight(const int32_t& val){
this->lastValidBlockHeight = val;
}

int32_t HoneycombTransaction::get_lastValidBlockHeight(){
return this->lastValidBlockHeight;
}

Dictionary HoneycombTransaction::to_dict(){
Dictionary res;
res["transaction"] = transaction;
res["blockhash"] = blockhash;
res["lastValidBlockHeight"] = lastValidBlockHeight;
return res;
}

void HoneycombTransaction::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_transaction"), &HoneycombTransaction::get_transaction);
ClassDB::bind_method(D_METHOD("set_transaction", "value"), &HoneycombTransaction::set_transaction);
ClassDB::add_property("HoneycombTransaction", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "transaction"), "set_transaction", "get_transaction");
ClassDB::bind_method(D_METHOD("get_blockhash"), &HoneycombTransaction::get_blockhash);
ClassDB::bind_method(D_METHOD("set_blockhash", "value"), &HoneycombTransaction::set_blockhash);
ClassDB::add_property("HoneycombTransaction", PropertyInfo(Variant::Type::STRING, "blockhash"), "set_blockhash", "get_blockhash");
ClassDB::bind_method(D_METHOD("get_lastValidBlockHeight"), &HoneycombTransaction::get_lastValidBlockHeight);
ClassDB::bind_method(D_METHOD("set_lastValidBlockHeight", "value"), &HoneycombTransaction::set_lastValidBlockHeight);
ClassDB::add_property("HoneycombTransaction", PropertyInfo(Variant::Type::INT, "lastValidBlockHeight"), "set_lastValidBlockHeight", "get_lastValidBlockHeight");
}
} // honeycomb_resource
} // godot