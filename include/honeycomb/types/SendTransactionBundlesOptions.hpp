#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONBUNDLESOPTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONBUNDLESOPTIONS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class SendTransactionBundlesOptions : public Resource{
GDCLASS(SendTransactionBundlesOptions, Resource)
private:
String commitment;
bool skipPreflight;
Variant bundles;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_commitment(const String& val);
String get_commitment();
void set_skipPreflight(const bool& val);
bool get_skipPreflight();
void set_bundles(const Variant& val);
Variant get_bundles();
};
} // honeycomb_resource
} // godot
#endif