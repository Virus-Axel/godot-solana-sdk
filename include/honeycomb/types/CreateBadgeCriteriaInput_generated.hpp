#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBADGECRITERIAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBADGECRITERIAINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

class CreateBadgeCriteriaInput : public Resource {
	GDCLASS(CreateBadgeCriteriaInput, Resource)
private:
	Variant projectAddress;
	Variant authority;
	Variant payer;
	int32_t startTime;
	int32_t endTime;
	int32_t badgeIndex;
	String condition;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_projectAddress(const Variant &val);
	Variant get_projectAddress() const;
	void set_authority(const Variant &val);
	Variant get_authority() const;
	void set_payer(const Variant &val);
	Variant get_payer() const;
	void set_startTime(const int32_t &val);
	int32_t get_startTime() const;
	void set_endTime(const int32_t &val);
	int32_t get_endTime() const;
	void set_badgeIndex(const int32_t &val);
	int32_t get_badgeIndex() const;
	void set_condition(const String &val);
	String get_condition() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
