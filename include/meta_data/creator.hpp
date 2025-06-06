#ifndef GODOT_SOLANA_META_DATA_CREATOR_HPP
#define GODOT_SOLANA_META_DATA_CREATOR_HPP

#include <cstdint>

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {

/**
 * @brief Contains token metadata creator information.
 */
class MetaDataCreator : public Resource {
	GDCLASS(MetaDataCreator, Resource)
private:
	Variant address;
	bool verified = false;
	uint16_t share = 0;

protected:
	/**
	 * @bindmethods{MetaDataCreator, Resource}
	 */
	static void _bind_methods();

public:
	/**
	 * @setter{address, p_value}
	 */
	void set_address(const Variant &p_value);

	/**
	 * @getter{address, Variant}
	 */
	[[nodiscard]] Variant get_address() const;

	/**
	 * @setter{verified, p_value}
	 */
	void set_verified(bool p_value);

	/**
	 * @getter{verified, bool}
	 */
	[[nodiscard]] bool get_verified() const;

	/**
	 * @setter{share, p_value}
	 */
	void set_share(uint32_t p_value);

	/**
	 * @getter{share, uint32_t}
	 */
	[[nodiscard]] uint32_t get_share() const;

	/**
	 * @brief Serialize MetaDataCreator.
	 *
	 * @return PackedByteArray Serialized bytes.
	 */
	[[nodiscard]] PackedByteArray serialize() const;
};

} //namespace godot

#endif