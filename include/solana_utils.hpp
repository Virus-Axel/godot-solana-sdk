#ifndef GODOT_SOLANA_SDK_SOLANA_UTILS_HPP
#define GODOT_SOLANA_SDK_SOLANA_UTILS_HPP

#include <string>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {

// Wrap the regular godot macros with lint rules disabled.

#define GDCLASS_CUSTOM(...)                                                                                                                                                                                   \
	/* NOLINTBEGIN(hicpp-use-auto,modernize-use-auto,llvm-else-after-return,readability-else-after-return,cppcoreguidelines-pro-type-reinterpret-cast,cert-oop54-cpp,cppcoreguidelines-pro-type-const-cast)*/ \
	GDCLASS(__VA_ARGS__) /* NOLINTEND(hicpp-use-auto,modernize-use-auto,llvm-else-after-return,readability-else-after-return,cppcoreguidelines-pro-type-reinterpret-cast,cert-oop54-cpp,cppcoreguidelines-pro-type-const-cast) */

#define memnew_custom(...)                            \
	/* NOLINTBEGIN(cppcoreguidelines-owning-memory)*/ \
	memnew(__VA_ARGS__) /* NOLINTEND(cppcoreguidelines-owning-memory) */

#define ERR_PRINT_ED_CUSTOM(m_msg)                                         \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_PRINT_ED(m_msg)                                                    \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_PRINT_ONCE_ED_CUSTOM(m_msg)                                    \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_PRINT_ONCE_ED(m_msg)                                               \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_FAIL_COND_V_CUSTOM(m_cond, m_retval)                           \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_FAIL_COND_V(m_cond, m_retval)                                      \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_FAIL_V_EDMSG_CUSTOM(m_retval, m_msg)                           \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_FAIL_V_EDMSG(m_retval, m_msg)                                      \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_FAIL_COND_V_EDMSG_CUSTOM(m_cond, m_retval, m_msg)              \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_FAIL_COND_V_EDMSG(m_cond, m_retval, m_msg)                         \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_FAIL_COND_EDMSG_CUSTOM(m_cond, m_msg)                          \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return, readability-simplify-boolean-expr)*/ \
	ERR_FAIL_COND_EDMSG(m_cond, m_msg)                                     \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return, readability-simplify-boolean-expr) */

#define ERR_FAIL_COND_CUSTOM(m_cond)                                                                         \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return,readability-simplify-boolean-expr)*/ \
	ERR_FAIL_COND(m_cond)                                                                                    \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return,readability-simplify-boolean-expr) */

#define ERR_FAIL_EDMSG_CUSTOM(m_msg)                                       \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_FAIL_EDMSG(m_msg)                                                  \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_FAIL_COND_V_MSG_CUSTOM(m_cond, m_retval, m_msg)                \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_FAIL_COND_V_MSG(m_cond, m_retval, m_msg)                           \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define WARN_PRINT_ED_CUSTOM(m_msg)                                        \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	WARN_PRINT_ED(m_msg)                                                   \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define WARN_PRINT_ONCE_ED_CUSTOM(m_msg)                                   \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	WARN_PRINT_ONCE_ED(m_msg)                                              \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_FAIL_COND_EDMSG_LAMBDA(m_cond, m_msg)                                                                        \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/                                               \
	if (unlikely(m_cond)) {                                                                                              \
		::godot::_err_print_error("lambda", __FILE__, __LINE__, "Condition \"" _STR(m_cond) "\" is true.", m_msg, true); \
		return;                                                                                                          \
	} else                                                                                                               \
		((void)0)                                                                                                        \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define MAKE_RESOURCE_TYPE_HINT(m_type) vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, m_type)

template <typename T>
inline void append_array(Array &arr, T arg) {
	arr.push_back(arg);
}

template <typename T, typename... Args>
inline void append_array(Array &arr, T arg1, Args... args) {
	arr.push_back(arg1);
	append_array(arr, args...);
}

template <typename T, typename... Args>
inline Array build_array(T arg1, Args... args) {
	Array arr;
	append_array(arr, arg1, args...);
	return arr;
}

const char mapping[] = {
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	'a',
	'b',
	'c',
	'd',
	'e',
	'f',
	'g',
	'h',
	'i',
	'j',
	'k',
	'l',
	'm',
	'n',
	'o',
	'p',
	'q',
	'r',
	's',
	't',
	'u',
	'v',
	'w',
	'x',
	'y',
	'z',
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'+',
	'/',
	'=',
};

/**
 * @class SolanaUtils
 * @brief Utility class for various purposes like encoding and decoding.
 */
class SolanaUtils : public Node {
	GDCLASS(SolanaUtils, Node)

private:
protected:
	/**
	 * @bindmethods{SolanaUtils, Node}
	 */
	static void _bind_methods();

public:
	const String MAINNET_URL = "https://api.mainnet.solana.com"; ///< Mainnet RPC Endpoint URL.
	const String DEVNET_URL = "https://api.devnet.solana.com"; ///< Devnet RPC Endpoint URL.
	const String TESTNET_URL = "https://api.testnet.solana.com"; ///< Testnet RPC Endpoint URL.
	const String LOCALHOST_URL = "http://127.0.0.1:8899"; ///< Localnet RPC Endpoint URL.

	static const std::string ZERO_ENCODED_32; ///< Base58 encoded 32 lengthed byte array of 0's.

	static const std::string SPL_TOKEN_ADDRESS; ///< SPL token address.
	static const std::string SPL_ASSOCIATED_TOKEN_ADDRESS; ///< SPL associated token address.

	SolanaUtils();
	/**
	 * @brief Decode base58 encoded string into bytes.
	 *
	 * @param str Base58 encoded string to decode.
	 * @return PackedByteArray Decoded bytes.
	 */
	static PackedByteArray bs58_decode(String str);

	/**
	 * @brief Encode a byte array into a base58 encoded string.
	 *
	 * @param input Byte array to encode.
	 * @return String Base58 encoded String.
	 */
	static String bs58_encode(PackedByteArray input);

	/**
	 * @brief Encode a byte array int a base64 encoded string.
	 *
	 * @param bytes Byte array to encode.
	 * @return String Base64 encoded String.
	 */
	static String bs64_encode(PackedByteArray bytes);

	/**
	 * @brief Decode a base64 encoded string into bytes.
	 *
	 * @param input Base64 encoded string to decode.
	 * @return PackedByteArray Decoded bytes.
	 */
	static PackedByteArray bs64_decode(String input);

	/**
	 * @brief Short vec encode a 16 bit integer.
	 *
	 * @param value integer to encode.
	 * @return PackedByteArray Short vec encoded byte array.
	 */
	static PackedByteArray short_u16_encode(unsigned int value);

	/**
	 * @brief Decode short vec bytes int an integer.
	 *
	 * @param bytes Short vec encoded bytes.
	 * @param cursor Pointer to an integer that saves abount of bytes required.
	 * @return unsigned int Decoded integer.
	 */
	static unsigned int short_u16_decode(const PackedByteArray &bytes, int *cursor);

	/**
	 * @brief Check if RPC response has returned a value.
	 *
	 * @param rpc_response Response to check in.
	 * @return true if value is in response Dictionary.
	 * @return false if value is not in response Dictionary.
	 */
	static bool rpc_response_has_value(const Dictionary &rpc_response);

	/**
	 * @brief Get the RPC response value.
	 *
	 * @note If there is not result/value in the response, a nullptr will be returned.
	 * This can be mixed up with a real null value. Use rpc_response_has_value to distinguish the
	 * scenarios.
	 *
	 * @param rpc_response RPC response to get value from.
	 * @return Dictionary RPC response value.
	 */
	static Variant get_rpc_response_value(const Dictionary &rpc_response);

	/**
	 * @brief Produce a hash from an array of String array as contents.
	 *
	 * @param contents String array to feed to the hash algorithm.
	 * @return PackedByteArray SHA256 Hash as byte array.
	 */
	static PackedByteArray sha256_hash_array(const PackedStringArray &contents);

	/**
	 * @brief Get an element from nested dictionaries.
	 * 
	 * @param dict Dictionary containing sub-dictionaries.
	 * @param path Path pointing to the value in the form of "path/to/value".
	 * @return Variant Value at the path.
	 */
	static Variant get_nested_value(const Dictionary& dict, const String& path);

	/**
	 * @brief Check if path of keys exists within nested dictionaries.
	 * 
	 * @param dict Dictionary containing sub-dictionaries.
	 * @param path Path pointing to the value in the form of "path/to/value".
	 * @return true if path exist.
	 * @return false if path does not exist.
	 */
	static bool nested_dict_has_keys(const Dictionary& dict, const String& path);

	~SolanaUtils() override = default;
};
} //namespace godot

#endif
