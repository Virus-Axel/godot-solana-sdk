#include "honeycomb/types/TransactionBundleResponse.hpp"


namespace godot {
namespace honeycomb_resource {

void TransactionBundleResponse::set_bundleId(const int32_t& val) {
this->bundleId = val;
}

int32_t TransactionBundleResponse::get_bundleId() const {
return this->bundleId;
}

void TransactionBundleResponse::set_responses(const Array& val) {
this->responses = val;
}

Array TransactionBundleResponse::get_responses() const {
return this->responses;
}

void TransactionBundleResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_bundleId"), &TransactionBundleResponse::get_bundleId);
ClassDB::bind_method(D_METHOD("set_bundleId", "value"), &TransactionBundleResponse::set_bundleId);
ClassDB::add_property("TransactionBundleResponse", PropertyInfo(Variant::Type::INT, "bundleId"), "set_bundleId", "get_bundleId");
ClassDB::bind_method(D_METHOD("get_responses"), &TransactionBundleResponse::get_responses);
ClassDB::bind_method(D_METHOD("set_responses", "value"), &TransactionBundleResponse::set_responses);
ClassDB::add_property("TransactionBundleResponse", PropertyInfo(Variant::Type::ARRAY, "responses"), "set_responses", "get_responses");
ClassDB::bind_method(D_METHOD("to_dict"), &TransactionBundleResponse::to_dict);
}
Dictionary TransactionBundleResponse::to_dict() {
Dictionary res;
res["bundleId"] = bundleId;
res["responses"] = responses;
return res;
}
} // namespace honeycomb_resource
} // namespace godot