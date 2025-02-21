#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WALLETS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WALLETS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Wallets : public Resource{
GDCLASS(Wallets, Resource)
private:
Variant shadow;
Array wallets;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_shadow(const Variant& val);
Variant get_shadow() const;
void set_wallets(const Array& val);
Array get_wallets() const;
};
} // honeycomb_resource
} // godot
#endif
