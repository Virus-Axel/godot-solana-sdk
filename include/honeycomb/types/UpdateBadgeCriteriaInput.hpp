#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEBADGECRITERIAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEBADGECRITERIAINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums.hpp"

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
String condition;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_projectAddress(const Variant& val);
Variant get_projectAddress() const;
void set_authority(const Variant& val);
Variant get_authority() const;
void set_criteriaIndex(const int32_t& val);
int32_t get_criteriaIndex() const;
void set_payer(const Variant& val);
Variant get_payer() const;
void set_startTime(const int32_t& val);
int32_t get_startTime() const;
void set_endTime(const int32_t& val);
int32_t get_endTime() const;
void set_condition(const String& val);
String get_condition() const;
};
} // honeycomb_resource
} // godot
#endif
