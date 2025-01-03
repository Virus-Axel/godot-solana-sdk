#include "compute_budget.hpp"

#include "solana_utils.hpp"

namespace godot {

PackedByteArray encode_u32(uint32_t input) {
	PackedByteArray result;
	result.resize(4);
	result[3] = (uint8_t)(input >> 24);
	result[2] = (uint8_t)(input >> 16);
	result[1] = (uint8_t)(input >> 8);
	result[0] = (uint8_t)input;
	return result;
}

const std::string ComputeBudget::ID = "ComputeBudget111111111111111111111111111111";

void ComputeBudget::_bind_methods() {
	ClassDB::bind_static_method("ComputeBudget", D_METHOD("set_compute_unit_limit", "unit_limit"), &ComputeBudget::set_compute_unit_limit);
	ClassDB::bind_static_method("ComputeBudget", D_METHOD("set_compute_unit_price", "unit_price"), &ComputeBudget::set_compute_unit_price);
}

Variant ComputeBudget::set_compute_unit_limit(const uint32_t unit_limit) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;
	data.resize(COMPUTE_UNIT_LIMIT_DATA_SIZE);

	data[0] = UNIT_LIMIT_DISCRIMINATOR;
	const PackedByteArray encoded_int = encode_u32(unit_limit);
	for (int i = 0; i < 4; i++) {
		data[1 + i] = encoded_int[i];
	}

	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	return result;
}

Variant ComputeBudget::set_compute_unit_price(const uint32_t unit_price) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;
	data.resize(COMPUTE_UNIT_PRICE_DATA_SIZE);

	data[0] = UNIT_PRICE_DISCRIMINATOR;
	// TODO(Virax): Use built in function instead.
	const PackedByteArray encoded_int = encode_u32(unit_price);
	for (int i = 0; i < 4; i++) {
		data[1 + i] = encoded_int[i];
	}

	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	return result;
}

Variant ComputeBudget::get_pid() {
	return memnew_custom(Pubkey(String(ID.c_str())));
}

} //namespace godot