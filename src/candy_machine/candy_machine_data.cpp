#include "candy_machine/candy_machine_data.hpp"

#include "candy_machine/config_line.hpp"
#include "instruction.hpp"
#include "instructions/associated_token_account.hpp"
#include "instructions/mpl_token_metadata.hpp"
#include "instructions/spl_token.hpp"
#include "instructions/system_program.hpp"
#include "solana_utils.hpp"

namespace godot {

void CandyMachineData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_token_standard", "value"), &CandyMachineData::set_token_standard);
	ClassDB::bind_method(D_METHOD("get_token_standard"), &CandyMachineData::get_token_standard);

	ClassDB::bind_method(D_METHOD("set_features", "value"), &CandyMachineData::set_features);
	ClassDB::bind_method(D_METHOD("get_features"), &CandyMachineData::get_features);

	ClassDB::bind_method(D_METHOD("set_authority", "value"), &CandyMachineData::set_authority);
	ClassDB::bind_method(D_METHOD("get_authority"), &CandyMachineData::get_authority);

	ClassDB::bind_method(D_METHOD("set_mint_authority", "value"), &CandyMachineData::set_mint_authority);
	ClassDB::bind_method(D_METHOD("get_mint_authority"), &CandyMachineData::get_mint_authority);

	ClassDB::bind_method(D_METHOD("set_collection_mint", "value"), &CandyMachineData::set_collection_mint);
	ClassDB::bind_method(D_METHOD("get_collection_mint"), &CandyMachineData::get_collection_mint);

	ClassDB::bind_method(D_METHOD("set_items_redeemed", "value"), &CandyMachineData::set_items_redeemed);
	ClassDB::bind_method(D_METHOD("get_items_redeemed"), &CandyMachineData::get_items_redeemed);

	ClassDB::bind_method(D_METHOD("set_items_available", "value"), &CandyMachineData::set_items_available);
	ClassDB::bind_method(D_METHOD("get_items_available"), &CandyMachineData::get_items_available);

	ClassDB::bind_method(D_METHOD("set_symbol", "value"), &CandyMachineData::set_symbol);
	ClassDB::bind_method(D_METHOD("get_symbol"), &CandyMachineData::get_symbol);

	ClassDB::bind_method(D_METHOD("set_seller_fee_basis_points", "value"), &CandyMachineData::set_seller_fee_basis_points);
	ClassDB::bind_method(D_METHOD("get_seller_fee_basis_points"), &CandyMachineData::get_seller_fee_basis_points);

	ClassDB::bind_method(D_METHOD("set_max_supply", "value"), &CandyMachineData::set_max_supply);
	ClassDB::bind_method(D_METHOD("get_max_supply"), &CandyMachineData::get_max_supply);

	ClassDB::bind_method(D_METHOD("set_is_mutable", "value"), &CandyMachineData::set_is_mutable);
	ClassDB::bind_method(D_METHOD("get_is_mutable"), &CandyMachineData::get_is_mutable);

	ClassDB::bind_method(D_METHOD("set_creators", "value"), &CandyMachineData::set_creators);
	ClassDB::bind_method(D_METHOD("get_creators"), &CandyMachineData::get_creators);

	ClassDB::bind_method(D_METHOD("set_config_line_setting", "value"), &CandyMachineData::set_config_line_setting);
	ClassDB::bind_method(D_METHOD("get_config_line_setting"), &CandyMachineData::get_config_line_setting);

	ClassDB::bind_method(D_METHOD("get_space_for_candy"), &CandyMachineData::get_space_for_candy);

	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "token_standard", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_READ_ONLY + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_STORAGE), "set_token_standard", "get_token_standard");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "features", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_READ_ONLY + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_STORAGE), "set_features", "get_features");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::OBJECT, "authority", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", PROPERTY_USAGE_READ_ONLY + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_STORAGE), "set_authority", "get_authority");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::OBJECT, "mint_authority", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", PROPERTY_USAGE_READ_ONLY + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_STORAGE), "set_mint_authority", "get_mint_authority");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::OBJECT, "collection_mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", PROPERTY_USAGE_READ_ONLY + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_STORAGE), "set_collection_mint", "get_collection_mint");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "items_redeemed", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_READ_ONLY + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_STORAGE), "set_items_redeemed", "get_items_redeemed");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "items_available"), "set_items_available", "get_items_available");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::STRING, "symbol"), "set_symbol", "get_symbol");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "seller_fee_basis_points"), "set_seller_fee_basis_points", "get_seller_fee_basis_points");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::INT, "max_supply"), "set_max_supply", "get_max_supply");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::BOOL, "is_mutable"), "set_is_mutable", "get_is_mutable");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::ARRAY, "creators", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("MetaDataCreator")), "set_creators", "get_creators");
	ClassDB::add_property("CandyMachineData", PropertyInfo(Variant::OBJECT, "config_line_setting", PROPERTY_HINT_RESOURCE_TYPE, "ConfigLineSetting"), "set_config_line_setting", "get_config_line_setting");
}

void CandyMachineData::set_token_standard(const uint32_t value) {
	token_standard = value;
}
uint32_t CandyMachineData::get_token_standard() {
	return token_standard;
}

void CandyMachineData::set_features(const PackedByteArray &value) {
	features = value;
}
PackedByteArray CandyMachineData::get_features() {
	return features;
}

void CandyMachineData::set_authority(const Variant &value) {
	authority = value;
}
Variant CandyMachineData::get_authority() {
	return authority;
}

void CandyMachineData::set_mint_authority(const Variant &value) {
	mint_authority = value;
}
Variant CandyMachineData::get_mint_authority() {
	return mint_authority;
}

void CandyMachineData::set_collection_mint(const Variant &value) {
	collection_mint = value;
}
Variant CandyMachineData::get_collection_mint() {
	return collection_mint;
}

void CandyMachineData::set_items_redeemed(const uint64_t value) {
	items_redeemed = value;
}
uint64_t CandyMachineData::get_items_redeemed() {
	return items_redeemed;
}

void CandyMachineData::set_items_available(const uint64_t value) {
	items_available = value;
}
uint64_t CandyMachineData::get_items_available() {
	return items_available;
}

void CandyMachineData::set_symbol(const String &value) {
	symbol = value;
}
String CandyMachineData::get_symbol() {
	return symbol;
}

void CandyMachineData::set_seller_fee_basis_points(const uint32_t value) {
	seller_fee_basis_points = value;
}
uint32_t CandyMachineData::get_seller_fee_basis_points() {
	return seller_fee_basis_points;
}

void CandyMachineData::set_max_supply(const uint64_t value) {
	max_supply = value;
}
uint64_t CandyMachineData::get_max_supply() {
	return max_supply;
}

void CandyMachineData::set_is_mutable(const bool value) {
	is_mutable = value;
}
bool CandyMachineData::get_is_mutable() {
	return is_mutable;
}

void CandyMachineData::set_creators(const Array &value) {
	creators = value;
}
Array CandyMachineData::get_creators() {
	return creators;
}

void CandyMachineData::set_config_line_setting(const Variant &config_line_setting) {
	this->config_line_setting = config_line_setting;
}

Variant CandyMachineData::get_config_line_setting() {
	return config_line_setting;
}

PackedByteArray CandyMachineData::serialize() {
	const int CANDY_DATA_LENGTH = 8 + 4 + symbol.length() + 2 + 8 + 1 + 4 + 34 * creators.size(); // No config line nor hidden setting.
	PackedByteArray result;
	result.resize(CANDY_DATA_LENGTH);

	result.encode_u64(0, items_available);
	result.encode_u32(8, symbol.length());
	for (unsigned int i = 0; i < symbol.length(); i++) {
		result[12 + i] = symbol[i];
	}
	int cursor = 12 + symbol.length();
	result.encode_u16(cursor, seller_fee_basis_points);
	cursor += 2;
	result.encode_u64(cursor, max_supply);
	cursor += 8;
	result[cursor] = (int)is_mutable;
	cursor++;
	result.encode_u32(cursor, creators.size());
	cursor += 4;
	for (unsigned int i = 0; i < creators.size(); i++) {
		PackedByteArray creator_bytes = Object::cast_to<MetaDataCreator>(creators[i])->serialize();
		for (unsigned int j = 0; j < creator_bytes.size(); j++) {
			result[cursor + j] = creator_bytes[j];
		}
		cursor += creator_bytes.size();
	}

	if (config_line_setting.get_type() != Variant::Type::OBJECT) {
		result.append(0);
	} else {
		result.append(1);
		ConfigLineSetting *config_line_setting_ptr = Object::cast_to<ConfigLineSetting>(config_line_setting);
		result.append_array(config_line_setting_ptr->serialize());
	}

	// No hidden settings.
	result.append(0);

	return result;
}

unsigned int CandyMachineData::get_config_line_size() {
	if (config_line_setting.get_type() == Variant::OBJECT) {
		ERR_FAIL_COND_V_EDMSG(!((Object *)config_line_setting)->is_class("ConfigLineSetting"), 0, "Parameter is not a ConfigLine");
		return Object::cast_to<ConfigLineSetting>(config_line_setting)->get_name_length() +
				Object::cast_to<ConfigLineSetting>(config_line_setting)->get_uri_length();
	} else {
		return 0;
	}
}

unsigned int CandyMachineData::get_space_for_candy() {
	// TODO(Virax): Change when hidden settings is supported.
	const unsigned int HIDDEN_SECTION = 8 + 8 + 32 + 32 + 32 + 8 + 8 + 4 + 10 + 2 + 8 + 1 + 4 + 5 * 34 + 1 + 4 + 32 + 4 + 4 + 200 + 4 + 1 + 1 + 4 + 32 + 4 + 200 + 32;
	return HIDDEN_SECTION +
			4 +
			items_available * get_config_line_size() +
			items_available / 8 +
			1 +
			items_available * 4;
}
} //namespace godot