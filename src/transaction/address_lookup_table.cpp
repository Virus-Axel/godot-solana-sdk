#include "address_lookup_table.hpp"

#include "godot_cpp/variant/packed_byte_array.hpp"

#include "pubkey.hpp"

namespace godot {

AddressLookupTable::AddressLookupTable(const PackedByteArray &bytes, unsigned int &consumed_bytes) {
	unsigned int cursor = 0;
	address = Pubkey::new_from_bytes(bytes.slice(0, PUBKEY_LENGTH));
	cursor += PUBKEY_LENGTH;
	const unsigned int writable_indices_size = bytes[cursor++];
	writable_indices = bytes.slice(cursor, cursor + writable_indices_size);
	cursor += writable_indices_size;
	const unsigned int readonly_indices_size = bytes[cursor++];
	readonly_indices = bytes.slice(cursor, cursor + readonly_indices_size);
	cursor += readonly_indices_size;

	consumed_bytes = cursor;
}

void AddressLookupTable::_bind_methods() {
}

PackedByteArray AddressLookupTable::serialize() {
	PackedByteArray result;
	result.append_array(Pubkey::bytes_from_variant(address));
	result.append(static_cast<unsigned char>(writable_indices.size()));
	result.append_array(writable_indices);
	result.append(static_cast<unsigned char>(readonly_indices.size()));
	result.append_array(readonly_indices);

	return result;
}

} //namespace godot