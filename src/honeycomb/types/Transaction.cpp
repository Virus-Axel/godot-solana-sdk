#include "Transaction.h"

namespace godot{

void Transaction::set_transaction(const PackedByteArray& val){
this->transaction = val;
}

PackedByteArray& Transaction::get_transaction(){
return this->transaction;
}

void Transaction::set_blockhash(const String& val){
this->blockhash = val;
}

String& Transaction::get_blockhash(){
return this->blockhash;
}

void Transaction::set_lastValidBlockHeight(const int32_t& val){
this->lastValidBlockHeight = val;
}

int32_t& Transaction::get_lastValidBlockHeight(){
return this->lastValidBlockHeight;
}

void Transaction::_bind_methods(){
ClassDB::bind_method(D_METHOD("get_transaction"), &Transaction::get_transaction);
ClassDB::bind_method(D_METHOD("set_transaction", "value"), &Transaction::set_transaction);
ClassDB::add_property("Transaction", PropertyInfo(Variant::PACKEDBYTEARRAY, "transaction"), "set_transaction", "get_transaction");
ClassDB::bind_method(D_METHOD("get_blockhash"), &Transaction::get_blockhash);
ClassDB::bind_method(D_METHOD("set_blockhash", "value"), &Transaction::set_blockhash);
ClassDB::add_property("Transaction", PropertyInfo(Variant::STRING, "blockhash"), "set_blockhash", "get_blockhash");
ClassDB::bind_method(D_METHOD("get_lastValidBlockHeight"), &Transaction::get_lastValidBlockHeight);
ClassDB::bind_method(D_METHOD("set_lastValidBlockHeight", "value"), &Transaction::set_lastValidBlockHeight);
ClassDB::add_property("Transaction", PropertyInfo(Variant::INT, "lastValidBlockHeight"), "set_lastValidBlockHeight", "get_lastValidBlockHeight");
}
} // godot