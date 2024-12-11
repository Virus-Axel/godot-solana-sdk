#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADVANCEDTREECONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADVANCEDTREECONFIG
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class AdvancedTreeConfig : public Resource{
GDCLASS(AdvancedTreeConfig, Resource)
private:
int32_t maxDepth;
int32_t maxBufferSize;
int32_t canopyDepth;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_maxDepth(const int32_t& val);
int32_t get_maxDepth();
void set_maxBufferSize(const int32_t& val);
int32_t get_maxBufferSize();
void set_canopyDepth(const int32_t& val);
int32_t get_canopyDepth();
};
}
}
#endif
