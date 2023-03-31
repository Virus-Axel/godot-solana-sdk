#ifndef SOLANA_SDK_UTILS
#define SOLANA_SDK_UTILS

#include <godot_cpp/classes/node.hpp>

const char BASE_58_MAP[] = {
	'1', '2', '3', '4', '5', '6', '7', '8',
	'9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
	'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
	'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z' };

const unsigned char ALPHABET_MAP[] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255,  0,  1,  2,  3,  4,  5,  6,  7,  8, 255, 255, 255, 255, 255, 255,
	255,  9, 10, 11, 12, 13, 14, 15, 16, 255, 17, 18, 19, 20, 21, 255,
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 255, 255, 255, 255, 255,
	255, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 255, 44, 45, 46,
	47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};

using namespace godot;

String bs58_encode(PackedByteArray bytes){
	PackedByteArray encoded;
	
	// Calculate the total length of the result
	unsigned int encoded_size = (bytes.size() * 138 / 100) + 1;
	
	encoded.resize(encoded_size);
	unsigned int digit_size = 1;
	
	for(unsigned int i = 0; i < bytes.size(); i++){
		int carry = int(bytes[i]);
		
		for (unsigned int j = 0; j < digit_size; j++){
			carry = carry + int(encoded[j] << 8);
			encoded[j] = (carry % 58) % 256;
			carry /= 58;
        }
			
		while (carry){
			encoded[digit_size] = (carry % 58) % 256;
			digit_size += 1;
			carry /= 58;
        }
    }


	String result;
	
	for (unsigned int i = 0; i < bytes.size() - 1; i++){
		if (bytes[i]){
            break;
        }
		result += BASE_58_MAP[0];
    }
		
	for (unsigned int i = 0; i < digit_size; i++){
		int map_index = encoded[digit_size - 1 - i];
		result += (BASE_58_MAP[map_index]);
    }

	return result;
}

PackedByteArray bs58_decode(String str){
	PackedByteArray result;
	if (str.length() == 0){
		return result;
    }
	
	// Worst case size
	result.resize(str.to_utf8_buffer().size() * 2);
	result[0] = 0;

	int resultlen = 1;
	for (unsigned int i = 0; i < str.to_utf8_buffer().size(); i++){
		unsigned int carry = (unsigned int) ALPHABET_MAP[str.to_utf8_buffer()[i]];
		if (carry == 255){
			return PackedByteArray();
        }
		for (int j = 0; j < resultlen; j++){
			carry += (unsigned int) (result[j]) * 58;
			result[j] = (char) (carry & 0xff);
			carry >>= 8;
        }

		while (carry > 0){
			result[resultlen++] = carry & 0xff;
			carry >>= 8;
        }
    }

    for (int i = 0; i < str.to_utf8_buffer().size() && str[i] == '1'; i++)
        result[resultlen++] = 0;

	int i = resultlen - 1;
	int z = (resultlen >> 1) + (resultlen & 1);
	
	while(i >= z){
		int k = result[i];
		result[i] = result[resultlen - i - 1];
		result[resultlen - i - 1] = k;
		i -= 1;
    }

	return result.slice(0, resultlen);
}

template <typename T>
inline T* variant_to_type(Variant var){
	Object *payer_cast = var;
    T *ret = Object::cast_to<T>(payer_cast);

	return ret;
}

template <typename T>
void array_to_pointer_array(Array arr, void** destination){
	for(int i = 0; i < arr.size(); i++){
        Object *object_cast = arr[i];
        T *element = Object::cast_to<T>(object_cast);
        destination[i] = element->to_ptr();
    }
}

#endif