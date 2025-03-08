#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLERESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLERESPONSE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class TransactionBundleResponse : public Resource{
GDCLASS(TransactionBundleResponse, Resource)
private:
int32_t bundleId;
Array responses;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_bundleId(const int32_t& val);
int32_t get_bundleId() const;
void set_responses(const Array& val);
Array get_responses() const;
};
} // honeycomb_resource
} // godot
#endif
