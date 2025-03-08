#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCOLLECTION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCOLLECTION
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class MultiplierTypeParamsCollection : public Resource{
GDCLASS(MultiplierTypeParamsCollection, Resource)
private:
Variant collection;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_collection(const Variant& val);
Variant get_collection() const;
};
} // honeycomb_resource
} // godot
#endif
