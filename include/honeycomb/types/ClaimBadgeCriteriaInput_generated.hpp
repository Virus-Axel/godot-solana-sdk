#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CLAIMBADGECRITERIAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CLAIMBADGECRITERIAINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

class ClaimBadgeCriteriaInput : public Resource {
	GDCLASS(ClaimBadgeCriteriaInput, Resource)
private:
	Variant projectAddress;
	Variant profileAddress;
	Variant payer;
	int32_t criteriaIndex;
	String proof;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_projectAddress(const Variant &val);
	Variant get_projectAddress() const;
	void set_profileAddress(const Variant &val);
	Variant get_profileAddress() const;
	void set_payer(const Variant &val);
	Variant get_payer() const;
	void set_criteriaIndex(const int32_t &val);
	int32_t get_criteriaIndex() const;
	void set_proof(const String &val);
	String get_proof() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
