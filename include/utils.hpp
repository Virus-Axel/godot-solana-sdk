#ifndef SOLANA_SDK_UTILS
#define SOLANA_SDK_UTILS

#include <godot_cpp/classes/node.hpp>

#define MAKE_RESOURCE_TYPE_HINT(m_type) vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, m_type)


using namespace godot;


const char mapping[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '=',
};

String encode64(PackedByteArray bytes);
	
/*
static func decode(str: String) -> PackedByteArray:
	var ret := PackedByteArray()
	var cutoff: int = 0
	
	# Buffer size with padding
	ret.resize(str.length() * 6 / 8)
	
	for i in range(str.length()):
		var val := int(mapping.find(str[i]))
		
		# If we find padding find how much we need to cut off
		if str[i] == "=":
			if i == str.to_utf8_buffer().size() - 1:
				cutoff = 1
			else:
				cutoff = 2
			break
		
		# Arrange bits in 8 bit chunks from 6 bit
		var index: int = ceil(float(i) * 6.0 / 8.0)
		var splash: int = val >> ((3 - (i % 4)) * 2)
		if splash != 0:
			ret[index - 1] += splash
		if index >= ret.size():
			break
			
		ret[index] += val << (2 + (i % 4) * 2)

	return ret.slice(0, ret.size() - cutoff)*/

template <typename T>
void* variant_to_type(Variant var){
	if(var.get_type() != Variant::OBJECT){
		return nullptr;
	}
	Object *payer_cast = var;
    T *ret = Object::cast_to<T>(payer_cast);

	return ret->to_ptr();
}

template <typename T>
bool array_to_pointer_array(Array arr, void** destination){
	for(int i = 0; i < arr.size(); i++){
        void *element_ptr = variant_to_type<T>(arr[i]);
		if (element_ptr == nullptr){
			return false;
		}
        destination[i] = element_ptr;
    }
	return true;
}

#endif