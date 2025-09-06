#ifndef SOLANA_SDK_MPL_CANDY_MACHINE_DATA
#define SOLANA_SDK_MPL_CANDY_MACHINE_DATA

#include <cstdint>

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Details about Candy Machines used by Candy Machine program.
 */
class CandyMachineData : public Resource {
	GDCLASS_CUSTOM(CandyMachineData, Resource)
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
	void set_token_standard(uint32_t value);

	/**
	 * @brief Get the token standard property.
	 *
	 * @return uint32_t token standard.
	 */
	[[nodiscard]] uint32_t get_token_standard() const;

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
	[[nodiscard]] PackedByteArray get_features() const;

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
	[[nodiscard]] Variant get_authority() const;

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
	[[nodiscard]] Variant get_mint_authority() const;

	/**
	 * @brief Set the items available property.
	 *
	 * @param value Property value.
	 */
	void set_items_available(uint64_t value);

	/**
	 * @brief Get the items available property.
	 *
	 * @return uint64_t items available property.
	 */
	[[nodiscard]] uint64_t get_items_available() const;

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
	[[nodiscard]] Variant get_collection_mint() const;

	/**
	 * @brief Set the items redeemed property.
	 *
	 * @param value Property value.
	 */
	void set_items_redeemed(uint64_t value);

	/**
	 * @brief Get the items redeemed property.
	 *
	 * @return uint64_t items redeemed property.
	 */
	[[nodiscard]] uint64_t get_items_redeemed() const;

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
	[[nodiscard]] String get_symbol() const;

	/**
	 * @brief Set the seller fee basis points property.
	 *
	 * @param value Property value.
	 */
	void set_seller_fee_basis_points(uint32_t value);

	/**
	 * @brief Get the seller fee basis point property.
	 *
	 * @return uint32_t seller fee basis points property.
	 */
	[[nodiscard]] uint32_t get_seller_fee_basis_points() const;

	/**
	 * @brief Set the max supply property.
	 *
	 * @param value Property value.
	 */
	void set_max_supply(uint64_t value);

	/**
	 * @brief Get the max supply property.
	 *
	 * @return uint64_t max supply property.
	 */
	[[nodiscard]] uint64_t get_max_supply() const;

	/**
	 * @brief Set the is mutable property.
	 *
	 * @param value Property value.
	 */
	void set_is_mutable(bool value);

	/**
	 * @brief Get the is mutable property.
	 *
	 * @return bool is mutable property.
	 */
	[[nodiscard]] bool get_is_mutable() const;

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
	[[nodiscard]] Array get_creators() const;

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
	[[nodiscard]] Variant get_config_line_setting() const;

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