#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEBADGECRITERIAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEBADGECRITERIAINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class UpdateBadgeCriteriaInput : public Resource{
GDCLASS(UpdateBadgeCriteriaInput, Resource)
private:
Variant projectAddress;
Variant authority;
int32_t criteriaIndex;
Variant payer;
int32_t startTime;
int32_t endTime;
int32_t condition;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_projectAddress(const Variant& val);
Variant get_projectAddress();
void set_authority(const Variant& val);
Variant get_authority();
void set_criteriaIndex(const int32_t& val);
int32_t get_criteriaIndex();
void set_payer(const Variant& val);
Variant get_payer();
void set_startTime(const int32_t& val);
int32_t get_startTime();
void set_endTime(const int32_t& val);
int32_t get_endTime();
void set_condition(const int32_t& val);
int32_t get_condition();
};
}
}
#endif
