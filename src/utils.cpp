#include "utils.hpp"

String encode64(PackedByteArray bytes){
	String r = ""; 
	String p = ""; 

	int64_t c = bytes.size() % 3;

	if(c > 0){
		for(int i = c; i < 3; i++){
			p += '='; 
			bytes.append(0);
		}
	}

	for(int i = 0; i < bytes.size(); i += 3){
		int n = (bytes[i] << 16) + (bytes[i + 1] << 8) + bytes[i + 2];

		r += mapping[(n >> 18) & 63];
		r += mapping[(n >> 12) & 63];
		r += mapping[(n >> 6) & 63];
		r += mapping[n & 63];
	}

	return r.substr(0, r.length() - p.length()) + p;
}