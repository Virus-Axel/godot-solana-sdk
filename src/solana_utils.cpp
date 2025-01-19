#include "solana_utils.hpp"

#include <algorithm>

#include "godot_cpp/core/class_db.hpp"

#include "utils.hpp"

namespace godot {

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
// clang-format off
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

const std::vector<char> BASE_64_MAP = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '=',
};
// clang-format on

using internal::gdextension_interface_print_warning;

const std::string SolanaUtils::ZERO_ENCODED_32 = "11111111111111111111111111111111";
const std::string SolanaUtils::SPL_TOKEN_ADDRESS = "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA";
const std::string SolanaUtils::SPL_ASSOCIATED_TOKEN_ADDRESS = "ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL";

void SolanaUtils::_bind_methods() {
	ClassDB::bind_static_method("SolanaUtils", D_METHOD("bs58_encode", "input"), &SolanaUtils::bs58_encode);
	ClassDB::bind_static_method("SolanaUtils", D_METHOD("bs58_decode", "input"), &SolanaUtils::bs58_decode);
	ClassDB::bind_static_method("SolanaUtils", D_METHOD("bs64_encode", "input"), &SolanaUtils::bs64_encode);
	ClassDB::bind_static_method("SolanaUtils", D_METHOD("bs64_decode", "input"), &SolanaUtils::bs64_decode);
}

SolanaUtils::SolanaUtils() {
}

PackedByteArray SolanaUtils::bs58_decode(String str) {
	PackedByteArray result;
	if (str.length() == 0) {
		return result;
	}

	// Worst case size
	result.resize(str.to_utf8_buffer().size() * 2);
	result[0] = 0;

	int resultlen = 1;
	for (unsigned int i = 0; i < str.length(); i++) {
		unsigned int carry = (unsigned int)ALPHABET_MAP[str.to_utf8_buffer()[i]];
		if (carry == 255) {
			return PackedByteArray();
		}
		for (int j = 0; j < resultlen; j++) {
			carry += (unsigned int)(result[j]) * 58;
			result[j] = (char)(carry & 0xff);
			carry >>= 8;
		}

		while (carry > 0) {
			result[resultlen++] = carry & 0xff;
			carry >>= 8;
		}
	}

	int i = 0;
	for (; (i < str.length()) && (str[i] == '1'); i++) {
		result[resultlen++] = 0;
	}

	// Special case for all 1's.
	if (i == str.length()) {
		resultlen--;
	}

	i = resultlen - 1;
	int z = (resultlen >> 1) + (resultlen & 1);

	while (i >= z) {
		int k = result[i];
		result[i] = result[resultlen - i - 1];
		result[resultlen - i - 1] = k;
		i -= 1;
	}

	return result.slice(0, resultlen);
}

String SolanaUtils::bs58_encode(PackedByteArray input) {
	PackedByteArray encoded;

	// Calculate the total length of the result
	unsigned int encoded_size = (input.size() * 138 / 100) + 1;

	encoded.resize(encoded_size);
	unsigned int digit_size = 1;

	for (unsigned int i = 0; i < input.size(); i++) {
		int carry = int(input[i]);

		for (unsigned int j = 0; j < digit_size; j++) {
			carry = carry + int(encoded[j] << 8);
			encoded[j] = (carry % 58) % 256;
			carry /= 58;
		}

		while (carry) {
			encoded[digit_size] = (carry % 58) % 256;
			digit_size += 1;
			carry /= 58;
		}
	}

	String result;

	for (unsigned int i = 0; i < input.size() - 1; i++) {
		if (input[i]) {
			break;
		}
		result += BASE_58_MAP[0];
	}

	for (unsigned int i = 0; i < digit_size; i++) {
		int map_index = encoded[digit_size - 1 - i];
		result += (BASE_58_MAP[map_index]);
	}

	return result;
}

String SolanaUtils::bs64_encode(PackedByteArray bytes) {
	String r = "";
	String p = "";

	int64_t c = bytes.size() % 3;

	if (c > 0) {
		for (int i = c; i < 3; i++) {
			p += '=';
			bytes.append(0);
		}
	}

	for (int i = 0; i < bytes.size(); i += 3) {
		int n = (bytes[i] << 16) + (bytes[i + 1] << 8) + bytes[i + 2];

		r += mapping[(n >> 18) & 63];
		r += mapping[(n >> 12) & 63];
		r += mapping[(n >> 6) & 63];
		r += mapping[n & 63];
	}

	return r.substr(0, r.length() - p.length()) + p;
}

PackedByteArray SolanaUtils::bs64_decode(String input) {
	PackedByteArray result;
	int cutoff = 0;

	// Buffer size with padding
	result.resize(input.length() * 6 / 8); // Maybe should be ceiled...
	for (unsigned int i = 0; i < input.length(); i++) {
		int val = int(std::find(BASE_64_MAP.begin(), BASE_64_MAP.end(), input[i]) - BASE_64_MAP.begin());

		if (input[i] == '=') {
			if (i == input.to_utf8_buffer().size() - 1) {
				cutoff = 1;
			} else {
				cutoff = 2;
			}
			break;
		}
		int index = ceil(float(i) * 6.0 / 8.0);
		int splash = val >> ((3 - (i % 4)) * 2);
		if (splash != 0) {
			result[index - 1] += splash;
		}
		if (index >= result.size()) {
			break;
		}

		result[index] += val << (2 + (i % 4) * 2);
	}
	return result.slice(0, result.size() - cutoff);
}

PackedByteArray SolanaUtils::short_u16_encode(const unsigned int value) {
	PackedByteArray result;
	unsigned int remaining = value;
	for (int i = 0; i < 3; ++i) {
		uint8_t byte = remaining & 0x7f;
		remaining >>= 7;
		if (remaining == 0) {
			result.append(byte);
			break;
		} else {
			byte |= 0x80;
			result.append(byte);
		}
	}
	return result;
}

unsigned int SolanaUtils::short_u16_decode(const PackedByteArray &bytes, int *cursor) {
	unsigned int value = 0;
	int initial_cursor = *cursor;
	for (int i = 0; i < 3 && (*cursor) < bytes.size(); ++i) {
		uint8_t byte = bytes[*cursor];
		value |= static_cast<unsigned int>(byte & 0x7f) << (7 * i);
		(*cursor)++;
		if ((byte & 0x80) == 0) {
			break;
		}
	}
	return value;
}

} //namespace godot
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
