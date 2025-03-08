#include "honeycomb/types/SendTransactionsOptions_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void SendTransactionsOptions::set_commitment(const String& val) {
this->commitment = val;
}

String SendTransactionsOptions::get_commitment() const {
return this->commitment;
}

void SendTransactionsOptions::set_skipPreflight(const bool& val) {
this->skipPreflight = val;
}

bool SendTransactionsOptions::get_skipPreflight() const {
return this->skipPreflight;
}

void SendTransactionsOptions::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_commitment"), &SendTransactionsOptions::get_commitment);
ClassDB::bind_method(D_METHOD("set_commitment", "value"), &SendTransactionsOptions::set_commitment);
ClassDB::add_property("SendTransactionsOptions", PropertyInfo(Variant::Type::STRING, "commitment"), "set_commitment", "get_commitment");
ClassDB::bind_method(D_METHOD("get_skipPreflight"), &SendTransactionsOptions::get_skipPreflight);
ClassDB::bind_method(D_METHOD("set_skipPreflight", "value"), &SendTransactionsOptions::set_skipPreflight);
ClassDB::add_property("SendTransactionsOptions", PropertyInfo(Variant::Type::BOOL, "skipPreflight"), "set_skipPreflight", "get_skipPreflight");
ClassDB::bind_method(D_METHOD("to_dict"), &SendTransactionsOptions::to_dict);
}
Dictionary SendTransactionsOptions::to_dict() {
Dictionary res;
if (!commitment.is_empty()) {
    res["commitment"] = commitment;
}
res["skipPreflight"] = skipPreflight;
return res;
}
} // namespace honeycomb_resource
} // namespace godot