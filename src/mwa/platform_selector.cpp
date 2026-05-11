// Platform selector — the sole #ifdef __ANDROID__ translation unit in the
// entire C++ source tree (AC-5). `MwaAndroidBridge::create`
// is defined ONLY here; every other TU is platform-agnostic and compiles
// against the pure-virtual ABC. CI grep (ci/grep_bans.sh pattern 5,
// Task 6) enforces this constraint for all __ANDROID__ spelling variants
// outside src/mwa/platform_selector.cpp.
#include "mwa_android_bridge.hpp"

#ifdef __ANDROID__
#include "mwa_android_bridge_jni.hpp"
#else
#include "no_op_mwa_android_bridge.hpp"
#endif

namespace godot_solana_sdk::mwa {

MwaAndroidBridge *MwaAndroidBridge::create(GodotMainDispatcher *dispatcher) {
#ifdef __ANDROID__
	// NOLINTNEXTLINE(cppcoreguidelines-owning-memory) — factory deliberately returns
	// a raw pointer; the caller takes ownership (typically stored in unique_ptr at
	// the call site). Header documents the ownership contract.
	return new MwaAndroidBridgeJni(dispatcher);
#else
	// NOLINTNEXTLINE(cppcoreguidelines-owning-memory) — see ownership note above.
	return new NoOpMwaAndroidBridge(dispatcher);
#endif
}

} // namespace godot_solana_sdk::mwa
