#include "utils.hpp"


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
