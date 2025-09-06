#ifndef GODOT_SOLANA_META_DATA_COLLECTION_HPP
#define GODOT_SOLANA_META_DATA_COLLECTION_HPP

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Contains token metadata collection information.
 */
class MetaDataCollection : public Resource {
	GDCLASS_CUSTOM(MetaDataCollection, Resource)

private:
	Variant key;
	bool verified = false;

protected:
	/**
	 * @bindmethods{MetaDataCollection, Resource}
	 */
	static void _bind_methods();

public:
	/**
	 * @setter{key, p_value}
	 */
	void set_key(const Variant &p_value);

	/**
	 * @getter{key, Variant}
	 */
	[[nodiscard]] Variant get_key() const;

	/**
	 * @setter{verified, p_value}
	 */
	void set_verified(bool p_value);

	/**
	 * @getter{verified, bool}
	 */
	[[nodiscard]] bool get_verified() const;

	/**
	 * @brief Serialize MetaDataCollection.
	 *
	 * @return PackedByteArray Serialized bytes.
	 */
	[[nodiscard]] PackedByteArray serialize() const;
};

} //namespace godot

#endif