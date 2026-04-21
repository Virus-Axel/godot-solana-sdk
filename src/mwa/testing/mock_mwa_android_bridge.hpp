// MockMwaAndroidBridge — host-platform (Linux/macOS) test double for MwaAndroidBridge.
// Story 1-4 Task 5 / AC-3. First consumer: Story 1-5 thread-hop unit test.
//
// THREAD-SAFETY CONTRACT (IMPORTANT):
//   MockMwaAndroidBridge is NOT thread-safe. Recording happens on the calling thread
//   (typically main thread in tests). `drive_signal_from_thread` does NOT append to
//   `recorded_calls_` — it only posts through the injected dispatcher. Tests MUST
//   NOT call `get_recorded_calls()` concurrently with in-flight mock-method
//   invocations on non-main threads. Story 1-5's test harness uses a
//   single-threaded test pattern: (1) call mock from main, (2) assert recorded,
//   (3) drive callback from worker via `drive_signal_from_thread`,
//   (4) assert dispatcher-observed emit on main.
//
// BUILD SCOPE:
//   Lives at `src/mwa/testing/` (depth 3) — NOT matched by SConstruct's
//   `Glob("src/*/*.cpp")` production source list. Story 1-5's test-target
//   alias links this file explicitly.
#ifndef GODOT_SOLANA_SDK_MWA_TESTING_MOCK_MWA_ANDROID_BRIDGE_HPP
#define GODOT_SOLANA_SDK_MWA_TESTING_MOCK_MWA_ANDROID_BRIDGE_HPP

#include "mwa_android_bridge.hpp"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot_solana_sdk::mwa {

class GodotMainDispatcher;

class MockMwaAndroidBridge : public MwaAndroidBridge {
public:
    // `dispatcher` may be nullptr — helpful for recording-only tests that
    // never exercise the thread-hop path. `drive_signal_from_thread` asserts
    // dispatcher non-null; the 9 op overrides do not call the dispatcher.
    explicit MockMwaAndroidBridge(GodotMainDispatcher* dispatcher = nullptr);
    ~MockMwaAndroidBridge() override = default;

    // --- MwaAndroidBridge overrides: record call + return ---
    void connect(const godot::String& request_id, const godot::Dictionary& identity,
                 const godot::String& cluster, const godot::Dictionary& opts) override;
    void reauthorize(const godot::String& request_id, const godot::Dictionary& opts) override;
    void disconnect(const godot::String& request_id, const godot::Dictionary& opts) override;
    void deauthorize(const godot::String& request_id, const godot::Dictionary& opts) override;
    void sign_messages(const godot::String& request_id,
                       const godot::TypedArray<godot::PackedByteArray>& messages,
                       const godot::Dictionary& opts) override;
    void sign_transactions(const godot::String& request_id,
                           const godot::TypedArray<godot::PackedByteArray>& transactions,
                           const godot::Dictionary& opts) override;
    void sign_and_send(const godot::String& request_id,
                       const godot::TypedArray<godot::PackedByteArray>& transactions,
                       const godot::Dictionary& opts) override;
    void forget_all(const godot::String& request_id) override;
    void get_diagnostics(const godot::String& request_id) override;

    // --- Test-driver API ---
    // Returns the recorded-calls buffer by value (godot::Array is COW, cheap).
    // Each element is a Dictionary {"method", "request_id", "args"}.
    godot::Array get_recorded_calls() const;
    void clear_calls();

    // Generalized over all signal types (mwa_error, connect_completed, etc.).
    // Spawns a std::thread named `thread_name` (best-effort cross-platform),
    // the thread calls `dispatcher_->post(signal_name, payload)` and exits.
    // Returns after joining the thread (synchronous for tests).
    //
    // Caveats:
    //   - Thread names longer than 15 characters are truncated or rejected on
    //     Linux (ERANGE, silently ignored). Keep names short for visibility
    //     in gdb / lldb thread listings.
    //   - `payload` is a godot::Dictionary (ref-counted handle). The lambda
    //     captures a copy of the handle, but the underlying data is shared
    //     with the caller. If the caller mutates the Dictionary between this
    //     call returning and the Godot main thread draining the deferred
    //     emission, the signal observes the mutated state. Tests typically
    //     build a fresh payload per drive call to avoid surprises.
    //   - This is the ONLY method on MockMwaAndroidBridge that spawns a
    //     thread. All 9 op overrides + record() + get_recorded_calls() +
    //     clear_calls() are test-thread-only; see THREAD-SAFETY CONTRACT
    //     at the top of this file.
    void drive_signal_from_thread(const godot::String& thread_name,
                                  const godot::String& signal_name,
                                  godot::Dictionary payload);

    MockMwaAndroidBridge(const MockMwaAndroidBridge&) = delete;
    MockMwaAndroidBridge& operator=(const MockMwaAndroidBridge&) = delete;

private:
    // Helper: append {method, request_id, args} to recorded_calls_.
    void record(const godot::String& method,
                const godot::String& request_id,
                godot::Dictionary args);

    godot::Array recorded_calls_;
    GodotMainDispatcher* dispatcher_;  // non-owning; may be nullptr.
};

}  // namespace godot_solana_sdk::mwa

#endif  // GODOT_SOLANA_SDK_MWA_TESTING_MOCK_MWA_ANDROID_BRIDGE_HPP
