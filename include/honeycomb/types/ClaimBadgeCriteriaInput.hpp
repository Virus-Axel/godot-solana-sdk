#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CLAIMBADGECRITERIAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CLAIMBADGECRITERIAINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ClaimBadgeCriteriaInput : public Resource{
GDCLASS(ClaimBadgeCriteriaInput, Resource)
private:
Variant projectAddress;
Variant profileAddress;
Variant payer;
int32_t criteriaIndex;
int32_t proof;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_projectAddress(const Variant& val);
Variant get_projectAddress();
void set_profileAddress(const Variant& val);
Variant get_profileAddress();
void set_payer(const Variant& val);
Variant get_payer();
void set_criteriaIndex(const int32_t& val);
int32_t get_criteriaIndex();
void set_proof(const int32_t& val);
int32_t get_proof();
};
} // honeycomb_resource
} // godot
#endif