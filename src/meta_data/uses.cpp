#include "meta_data/uses.hpp"


namespace godot{

void MetaDataUses::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_use_method"), &MetaDataUses::get_use_method);
    ClassDB::bind_method(D_METHOD("set_use_method", "p_value"), &MetaDataUses::set_use_method);
    ClassDB::bind_method(D_METHOD("get_total"), &MetaDataUses::get_total);
    ClassDB::bind_method(D_METHOD("set_total", "p_value"), &MetaDataUses::set_total);
    ClassDB::bind_method(D_METHOD("get_remaining"), &MetaDataUses::get_remaining);
    ClassDB::bind_method(D_METHOD("set_remaining", "p_value"), &MetaDataUses::set_remaining);

    ClassDB::add_property("MetaDataUses", PropertyInfo(Variant::INT, "use_method", PROPERTY_HINT_ENUM, "Burn,Multiple,Single", PROPERTY_USAGE_DEFAULT), "set_use_method", "get_use_method");
    ClassDB::add_property("MetaDataUses", PropertyInfo(Variant::INT, "total", PROPERTY_HINT_NONE), "set_total", "get_total");
    ClassDB::add_property("MetaDataUses", PropertyInfo(Variant::INT, "remaining", PROPERTY_HINT_NONE), "set_remaining", "get_remaining");
}

void MetaDataUses::set_use_method(const uint32_t p_value){
    use_method = (UseMethod)p_value;
}
uint32_t MetaDataUses::get_use_method(){
    return use_method;
}

void MetaDataUses::set_total(const uint64_t p_value){
    total = p_value;
}
uint64_t MetaDataUses::get_total(){
    return total;
}

void MetaDataUses::set_remaining(const uint64_t p_value){
    remaining = p_value;
}
uint64_t MetaDataUses::get_remaining(){
    return remaining;
}

PackedByteArray MetaDataUses::serialize() const{
    PackedByteArray res;
    res.resize(17);
    res[0] = use_method;
    res.encode_u64(1, remaining);
    res.encode_u64(9, total);

    return res;
}

}
