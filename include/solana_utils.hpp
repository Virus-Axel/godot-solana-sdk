#ifndef GODOT_SOLANA_SDK_SOLANA_UTILS_HPP
#define GODOT_SOLANA_SDK_SOLANA_UTILS_HPP

#include <godot_cpp/classes/node.hpp>
#include <hash.hpp>

namespace godot {

// Wrap the regular godot macros with lint rules disabled.

#define memnew_custom(...)                            \
	/* NOLINTBEGIN(cppcoreguidelines-owning-memory)*/ \
	memnew(__VA_ARGS__) /* NOLINTEND(cppcoreguidelines-owning-memory) */

#define ERR_FAIL_COND_V_EDMSG_CUSTOM(...)                                  \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_FAIL_COND_V_EDMSG(__VA_ARGS__)                                     \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_FAIL_COND_EDMSG_CUSTOM(...)                                    \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_FAIL_COND_EDMSG(__VA_ARGS__)                                       \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

#define ERR_FAIL_COND_CUSTOM(...)                                          \
	/* NOLINTBEGIN(llvm-else-after-return,readability-else-after-return)*/ \
	ERR_FAIL_COND(__VA_ARGS__)                                             \
	/* NOLINTEND(llvm-else-after-return,readability-else-after-return) */

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

class SolanaUtils : public Node {
	GDCLASS(SolanaUtils, Node)

private:
protected:
	static void _bind_methods();

public:
	const String MAINNET_URL = "https://api.mainnet.solana.com";
	const String DEVNET_URL = "https://api.devnet.solana.com";
	const String TESTNET_URL = "https://api.testnet.solana.com";
	const String LOCALHOST_URL = "http://127.0.0.1:8899";

	static const std::string ZERO_ENCODED_32;

	static const std::string SPL_TOKEN_ADDRESS;
	static const std::string SPL_ASSOCIATED_TOKEN_ADDRESS;

	SolanaUtils();
	static PackedByteArray bs58_decode(String str);
	static String bs58_encode(PackedByteArray input);
	static String bs64_encode(PackedByteArray bytes);
	static PackedByteArray bs64_decode(String input);

	static PackedByteArray short_u16_encode(unsigned int value);
	static unsigned int short_u16_decode(const PackedByteArray &bytes, int *cursor);

	~SolanaUtils();
};
} //namespace godot

#endif
