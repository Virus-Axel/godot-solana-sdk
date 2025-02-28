#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYCUSTOM
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYCUSTOM
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class UsedByCustom : public Resource{
GDCLASS(UsedByCustom, Resource)
private:
Variant user;
Dictionary data;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_user(const Variant& val);
Variant get_user() const;
void set_data(const Dictionary& val);
Dictionary get_data() const;
};
} // honeycomb_resource
} // godot
#endif
