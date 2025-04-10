#ifndef SOLANA_SDK_ADDRESS_LOOKUP_TABLE_HPP
#define SOLANA_SDK_ADDRESS_LOOKUP_TABLE_HPP

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {

/**
 * @brief Address lookup table used in versioned transactions.
 */
class AddressLookupTable : public Resource {
	GDCLASS(AddressLookupTable, Resource)
private:
	Variant address;
	PackedByteArray writable_indices;
	PackedByteArray readonly_indices;

protected:
	/**
	 * @brief Binds methods of AddressLookupTable Resource.
	 */
	static void _bind_methods();

public:
	AddressLookupTable() = default;

	/**
	 * @brief Construct a new Address Lookup Table object from bytes
	 * 
	 * @param bytes Byte serialization of an address lookup table.
	 * @param consumed_bytes Total amount of bytes read will be written to this variable.
	 */
	AddressLookupTable(const PackedByteArray &bytes, unsigned int &consumed_bytes);

	/**
	 * @brief Serializes the address lookup table.
	 * 
	 * @return PackedByteArray byte serialization.
	 */
	PackedByteArray serialize();
	~AddressLookupTable() = default;
};
} //namespace godot

#endif