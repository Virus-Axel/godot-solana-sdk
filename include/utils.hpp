#ifndef SOLANA_SDK_UTILS
#define SOLANA_SDK_UTILS

#include <godot_cpp/classes/node.hpp>


using namespace godot;

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