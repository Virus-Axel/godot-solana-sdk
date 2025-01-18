#ifndef SOLANA_SDK_MPL_CANDY_MACHINE_DATA
#define SOLANA_SDK_MPL_CANDY_MACHINE_DATA

#include <account_meta.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <solana_client.hpp>

namespace godot {

class CandyMachineData : public Resource {
	GDCLASS(CandyMachineData, Resource)
private:
	uint32_t token_standard = 0;
	PackedByteArray features;
	Variant authority;
	Variant mint_authority;
	Variant collection_mint;
	uint64_t items_redeemed = 0;

	uint64_t items_available = 0;
	String symbol;
	uint32_t seller_fee_basis_points = 0;
	uint64_t max_supply = 0;
	bool is_mutable = false;
	Array creators;

	Variant config_line_setting = nullptr;
	Variant hidden_settings = nullptr;

	unsigned int get_config_line_size();

protected:
	static void _bind_methods();

public:
	void set_token_standard(const uint32_t value);
	uint32_t get_token_standard();

	void set_features(const PackedByteArray &value);
	PackedByteArray get_features();

	void set_authority(const Variant &value);
	Variant get_authority();

	void set_mint_authority(const Variant &value);
	Variant get_mint_authority();

	void set_items_available(const uint64_t value);
	uint64_t get_items_available();

	void set_collection_mint(const Variant &value);
	Variant get_collection_mint();

	void set_items_redeemed(const uint64_t value);
	uint64_t get_items_redeemed();

	void set_symbol(const String &value);
	String get_symbol();

	void set_seller_fee_basis_points(const uint32_t value);
	uint32_t get_seller_fee_basis_points();

	void set_max_supply(const uint64_t value);
	uint64_t get_max_supply();

	void set_is_mutable(const bool value);
	bool get_is_mutable();

	void set_creators(const Array &value);
	Array get_creators();

	void set_config_line_setting(const Variant &config_line_setting);
	Variant get_config_line_setting();

	PackedByteArray serialize();
	unsigned int get_space_for_candy();
};

} //namespace godot

#endif