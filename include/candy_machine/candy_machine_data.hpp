#ifndef SOLANA_SDK_MPL_CANDY_MACHINE_DATA
#define SOLANA_SDK_MPL_CANDY_MACHINE_DATA

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "solana_client.hpp"

namespace godot {

/**
 * @brief Details about Candy Machines used by Candy Machine program.
 */
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
	/**
	 * @brief Bind methods for CandyMachineData Resource.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Set the token standard property.
	 * 
	 * @param value Property value.
	 */
	void set_token_standard(const uint32_t value);

	/**
	 * @brief Get the token standard property.
	 * 
	 * @return uint32_t token standard.
	 */
	uint32_t get_token_standard();

	/**
	 * @brief Set the features byte array property.
	 * 
	 * @param value Property value.
	 */
	void set_features(const PackedByteArray &value);

	/**
	 * @brief Get the features property.
	 * 
	 * @return PackedByteArray feature property.
	 */
	PackedByteArray get_features();

	/**
	 * @brief Set the authority property.
	 * 
	 * @param value Property value.
	 */
	void set_authority(const Variant &value);

	/**
	 * @brief Get the authority property.
	 * 
	 * @return Variant authority property.
	 */
	Variant get_authority();

	/**
	 * @brief Set the mint authority property.
	 * 
	 * @param value Property value.
	 */
	void set_mint_authority(const Variant &value);

	/**
	 * @brief Get the mint authority property.
	 * 
	 * @return Variant mint authority property.
	 */
	Variant get_mint_authority();

	/**
	 * @brief Set the items available property.
	 * 
	 * @param value Property value.
	 */
	void set_items_available(const uint64_t value);

	/**
	 * @brief Get the items available property.
	 * 
	 * @return uint64_t items available property.
	 */
	uint64_t get_items_available();

	/**
	 * @brief Set the collection mint property.
	 * 
	 * @param value Property value.
	 */
	void set_collection_mint(const Variant &value);

	/**
	 * @brief Get the collection mint property.
	 * 
	 * @return Variant collection mint property.
	 */
	Variant get_collection_mint();

	/**
	 * @brief Set the items redeemed property.
	 * 
	 * @param value Property value.
	 */
	void set_items_redeemed(const uint64_t value);

	/**
	 * @brief Get the items redeemed property.
	 * 
	 * @return uint64_t items redeemed property.
	 */
	uint64_t get_items_redeemed();

	/**
	 * @brief Set the symbol property.
	 * 
	 * @param value Property value.
	 */
	void set_symbol(const String &value);

	/**
	 * @brief Get the symbol property.
	 * 
	 * @return String symbol property.
	 */
	String get_symbol();

	/**
	 * @brief Set the seller fee basis points property.
	 * 
	 * @param value Property value.
	 */
	void set_seller_fee_basis_points(const uint32_t value);

	/**
	 * @brief Get the seller fee basis point property.
	 * 
	 * @return uint32_t seller fee basis points property.
	 */
	uint32_t get_seller_fee_basis_points();

	/**
	 * @brief Set the max supply property.
	 * 
	 * @param value Property value.
	 */
	void set_max_supply(const uint64_t value);

	/**
	 * @brief Get the max supply property.
	 * 
	 * @return uint64_t max supply property.
	 */
	uint64_t get_max_supply();

	/**
	 * @brief Set the is mutable property.
	 * 
	 * @param value Property value.
	 */
	void set_is_mutable(const bool value);

	/**
	 * @brief Get the is mutable property.
	 * 
	 * @return bool is mutable property.
	 */
	bool get_is_mutable();

	/**
	 * @brief Set the creators property.
	 * 
	 * @param value Property value.
	 */
	void set_creators(const Array &value);

	/**
	 * @brief Get the creators property.
	 * 
	 * @return Array creators property.
	 */
	Array get_creators();

	/**
	 * @brief Set the consig line settings property.
	 * 
	 * @param config_line_setting Property value.
	 */
	void set_config_line_setting(const Variant &config_line_setting);

	/**
	 * @brief Get the config line setting property.
	 * 
	 * @return Variant config line setting property.
	 */
	Variant get_config_line_setting();

	/**
	 * @brief Serialize candy machine data.
	 * 
	 * @return PackedByteArray byte array.
	 */
	PackedByteArray serialize();

	/**
	 * @brief Get the space for candy property.
	 * 
	 * @return unsigned int space for candy property.
	 */
	unsigned int get_space_for_candy();
};

} //namespace godot

#endif