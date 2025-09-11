#include "compute_budget.hpp"

#include <cstdint>
#include <string>

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "instruction.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

const std::string ComputeBudget::PID = "ComputeBudget111111111111111111111111111111";

void ComputeBudget::_bind_methods() {
	ClassDB::bind_static_method("ComputeBudget", D_METHOD("set_compute_unit_limit", "unit_limit"), &ComputeBudget::set_compute_unit_limit);
	ClassDB::bind_static_method("ComputeBudget", D_METHOD("set_compute_unit_price", "unit_price"), &ComputeBudget::set_compute_unit_price);
}

Variant ComputeBudget::set_compute_unit_limit(const uint32_t unit_limit) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;
	data.resize(COMPUTE_UNIT_LIMIT_DATA_SIZE);

	data[0] = UNIT_LIMIT_DISCRIMINATOR;
	data.encode_u32(1, unit_limit);

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	return result;
}

Variant ComputeBudget::set_compute_unit_price(const uint32_t unit_price) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;
	data.resize(COMPUTE_UNIT_PRICE_DATA_SIZE);

	data[0] = UNIT_PRICE_DISCRIMINATOR;
	data.encode_u32(1, unit_price);

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	return result;
}

Variant ComputeBudget::get_pid() {
	return memnew_custom(Pubkey(String(PID.c_str())));
}

} //namespace godot