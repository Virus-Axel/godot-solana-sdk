#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLESOPTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLESOPTIONS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class TransactionBundlesOptions : public Resource{
GDCLASS(TransactionBundlesOptions, Resource)
private:
int32_t bundleSize;
int32_t firstBundleSize;
int32_t lastBundleSize;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_bundleSize(const int32_t& val);
int32_t get_bundleSize();
void set_firstBundleSize(const int32_t& val);
int32_t get_firstBundleSize();
void set_lastBundleSize(const int32_t& val);
int32_t get_lastBundleSize();
};
} // honeycomb_resource
} // godot
#endif