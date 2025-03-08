#include "honeycomb/types/Transactions_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void Transactions::set_transactions(const Array &val) {
	this->transactions = val;
}

Array Transactions::get_transactions() const {
	return this->transactions;
}

void Transactions::set_blockhash(const String &val) {
	this->blockhash = val;
}

String Transactions::get_blockhash() const {
	return this->blockhash;
}

void Transactions::set_lastValidBlockHeight(const int32_t &val) {
	this->lastValidBlockHeight = val;
}

int32_t Transactions::get_lastValidBlockHeight() const {
	return this->lastValidBlockHeight;
}

void Transactions::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_transactions"), &Transactions::get_transactions);
	ClassDB::bind_method(D_METHOD("set_transactions", "value"), &Transactions::set_transactions);
	ClassDB::add_property("Transactions", PropertyInfo(Variant::Type::ARRAY, "transactions"), "set_transactions", "get_transactions");
	ClassDB::bind_method(D_METHOD("get_blockhash"), &Transactions::get_blockhash);
	ClassDB::bind_method(D_METHOD("set_blockhash", "value"), &Transactions::set_blockhash);
	ClassDB::add_property("Transactions", PropertyInfo(Variant::Type::STRING, "blockhash"), "set_blockhash", "get_blockhash");
	ClassDB::bind_method(D_METHOD("get_lastValidBlockHeight"), &Transactions::get_lastValidBlockHeight);
	ClassDB::bind_method(D_METHOD("set_lastValidBlockHeight", "value"), &Transactions::set_lastValidBlockHeight);
	ClassDB::add_property("Transactions", PropertyInfo(Variant::Type::INT, "lastValidBlockHeight"), "set_lastValidBlockHeight", "get_lastValidBlockHeight");
	ClassDB::bind_method(D_METHOD("to_dict"), &Transactions::to_dict);
}
Dictionary Transactions::to_dict() {
	Dictionary res;
	res["transactions"] = transactions;
	if (!blockhash.is_empty()) {
		res["blockhash"] = blockhash;
	}
	res["lastValidBlockHeight"] = lastValidBlockHeight;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot