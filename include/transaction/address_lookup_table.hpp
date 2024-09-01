#ifndef SOLANA_SDK_ADDRESS_LOOKUP_TABLE_HPP
#define SOLANA_SDK_ADDRESS_LOOKUP_TABLE_HPP

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot{

class AddressLookupTable : public Resource{
    GDCLASS(AddressLookupTable, Resource)
private:
    Variant address;
    PackedByteArray writable_indices;
    PackedByteArray readonly_indices;
protected:
    static void _bind_methods();
public:
    AddressLookupTable();
    AddressLookupTable(const PackedByteArray& bytes, unsigned int &consumed_bytes);
    PackedByteArray serialize();
    ~AddressLookupTable();
};
}

#endif