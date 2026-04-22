// Story 1-5 Task 4 Phase D — port Story 1-2 §Task 2 deferred SecretString
// GoogleTest body. Resolves CR-8 when `scons tests && ./bin/mwa_tests` reports
// GREEN on both TEST(SecretString, ...) cases.
//
// Story 1-2 §Task 2 specified two assertions verbatim; the current SecretString
// public API at src/mwa/include/secret_string.hpp is unchanged from its 1-2
// landing (ctor taking godot::PackedByteArray, reveal_bytes() returning a COW
// copy, to_string() returning the literal "<redacted>", plus a godot::String
// operator+ convenience). No semantic reconciliation needed; compiles
// mechanically from the 1-2 spec.

#include <gtest/gtest.h>

#include "secret_string.hpp"

#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot_solana_sdk::mwa {
namespace {

TEST(SecretString, ToStringReturnsRedacted) {
	godot::PackedByteArray bytes;
	bytes.push_back(1);
	bytes.push_back(2);
	bytes.push_back(3);
	SecretString s(bytes);
	EXPECT_TRUE(s.to_string() == godot::String("<redacted>"));
}

TEST(SecretString, RevealBytesReturnsOriginal) {
	godot::PackedByteArray bytes;
	bytes.push_back(1);
	bytes.push_back(2);
	bytes.push_back(3);
	SecretString s(bytes);
	godot::PackedByteArray revealed = s.reveal_bytes();
	ASSERT_EQ(revealed.size(), 3);
	EXPECT_EQ(revealed[0], 1);
	EXPECT_EQ(revealed[1], 2);
	EXPECT_EQ(revealed[2], 3);
}

}  // anonymous namespace
}  // namespace godot_solana_sdk::mwa
