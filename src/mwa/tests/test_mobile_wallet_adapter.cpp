// Story 1-5 Task 5 — MobileWalletAdapter tests (AC-1 smoke + timing, AC-2
// thread-hop, AC-3 pre-op error).
//
// Observation strategy in host-mode test binary: emit_signal has no connected
// callbacks (no scene tree, no GDScript-hooked handlers), so tests inspect
// posted-but-not-yet-drained entries via
// GodotMainDispatcher::snapshot_pending_for_testing(). drain_for_testing() is
// invoked for cleanup / state isolation between tests (not strictly required
// per-test, but good hygiene).
//
// Test-only plumbing added alongside this file:
//   - GodotMainDispatcher::snapshot_pending_for_testing() const — read-only
//     copy of pending_ under lock.
//   - MobileWalletAdapter::dispatcher_for_testing() — reference to the owned
//     dispatcher so tests can reach snapshot/drain.
// Both are #ifdef MWA_TESTING-gated, symmetric with the T3 setter pattern.

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "mwa/mobile_wallet_adapter.hpp"
#include "godot_main_dispatcher.hpp"
#include "mwa/testing/mock_mwa_android_bridge.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot_solana_sdk {
namespace {

using std::chrono::milliseconds;
using std::chrono::steady_clock;

// Returns true if `signals` contains an entry whose "name" equals `name`.
// Outputs the args TypedArray for further shape checks.
bool find_signal(const godot::TypedArray<godot::Dictionary> &signals,
		const char *name,
		godot::Array *out_args) {
	const int n = signals.size();
	const godot::String needle(name);
	for (int i = 0; i < n; ++i) {
		godot::Dictionary sig = signals[i];
		godot::String sig_name = sig["name"];
		if (sig_name == needle) {
			if (out_args != nullptr) {
				*out_args = sig["args"];
			}
			return true;
		}
	}
	return false;
}

// Returns true if `methods` contains an entry whose "name" equals `name`.
bool find_method(const godot::TypedArray<godot::Dictionary> &methods, const char *name) {
	const int n = methods.size();
	const godot::String needle(name);
	for (int i = 0; i < n; ++i) {
		godot::Dictionary m = methods[i];
		godot::String m_name = m["name"];
		if (m_name == needle) {
			return true;
		}
	}
	return false;
}

class MobileWalletAdapterTest : public ::testing::Test {
protected:
	static void SetUpTestSuite() {
		// Godot normally registers the class at extension load. In the host
		// test binary we do it manually, once per suite.
		godot::ClassDB::register_class<MobileWalletAdapter>();
	}

	void SetUp() override {
		node_ = memnew(MobileWalletAdapter);
	}

	void TearDown() override {
		// Drain any pending signals before teardown so inter-test state is clean.
		node_->dispatcher_for_testing().drain_for_testing();
		memdelete(node_);
		node_ = nullptr;
	}

	MobileWalletAdapter *node_ = nullptr;
};

// ============================================================================
// AC-1 binding smoke — 11 signals + 13 methods bind with correct PropertyInfo
// shapes (per Story 1-5 Guardrails signals table + method list).
// ============================================================================
TEST_F(MobileWalletAdapterTest, BindingSmoke_SignalsAndMethodsRegistered) {
	// 11 signals in AC-1 order.
	const char *expected_signals[] = {
		"connect_completed",
		"reauthorize_completed",
		"disconnect_completed",
		"deauthorize_completed",
		"sign_messages_completed",
		"sign_transactions_completed",
		"sign_and_send_completed",
		"mwa_error",
		"mwa_timeout",
		"mwa_cancelled_lifecycle",
		"reauth_required",
	};

	godot::TypedArray<godot::Dictionary> signals =
			godot::ClassDB::class_get_signal_list("MobileWalletAdapter", /* no_inheritance = */ true);

	for (const char *name : expected_signals) {
		godot::Array args;
		bool found = find_signal(signals, name, &args);
		EXPECT_TRUE(found) << "signal not bound: " << name;
	}

	// PropertyInfo shape — Guardrails signals table:
	//   7 *_completed signals: 2 params (request_id: String, result: Dictionary)
	//   4 error/lifecycle:     1 param  (payload: Dictionary)
	// Name-only presence is insufficient (per story T5.1 spec).
	const char *two_param_signals[] = {
		"connect_completed",
		"reauthorize_completed",
		"disconnect_completed",
		"deauthorize_completed",
		"sign_messages_completed",
		"sign_transactions_completed",
		"sign_and_send_completed",
	};
	for (const char *name : two_param_signals) {
		godot::Array args;
		ASSERT_TRUE(find_signal(signals, name, &args)) << name;
		EXPECT_EQ(args.size(), 2) << "expected 2 args on " << name;
	}

	const char *one_param_signals[] = {
		"mwa_error",
		"mwa_timeout",
		"mwa_cancelled_lifecycle",
		"reauth_required",
	};
	for (const char *name : one_param_signals) {
		godot::Array args;
		ASSERT_TRUE(find_signal(signals, name, &args)) << name;
		EXPECT_EQ(args.size(), 1) << "expected 1 arg on " << name;
	}

	// 13 methods post-D-1 rename.
	const char *expected_methods[] = {
		// 7 ops
		"mwa_connect",
		"reauthorize",
		"mwa_disconnect",
		"deauthorize",
		"sign_messages",
		"sign_transactions",
		"sign_and_send",
		// 4 state getters
		"mwa_is_connected",
		"get_public_key",
		"get_cluster",
		"get_wallet_label",
		// 2 utility
		"get_diagnostics",
		"forget_all",
	};

	godot::TypedArray<godot::Dictionary> methods =
			godot::ClassDB::class_get_method_list("MobileWalletAdapter", /* no_inheritance = */ true);

	for (const char *name : expected_methods) {
		EXPECT_TRUE(find_method(methods, name)) << "method not bound: " << name;
	}
}

// ============================================================================
// AC-1 timing — each of the 7 op methods returns within ≤16 ms under the
// default NoOp bridge path (normal delegation; not the null-bridge pre-op
// branch). The null-bridge branch is exercised by AC-3's test.
// ============================================================================
TEST_F(MobileWalletAdapterTest, OpMethodsReturnWithin16ms) {
	godot::Dictionary identity;
	const godot::String cluster("devnet");
	godot::Dictionary opts;
	godot::TypedArray<godot::PackedByteArray> msgs;
	godot::TypedArray<godot::PackedByteArray> txns;

	auto within_budget = [](steady_clock::time_point t0, steady_clock::time_point t1,
									const char *op) {
		const auto elapsed = std::chrono::duration_cast<milliseconds>(t1 - t0);
		EXPECT_LE(elapsed.count(), 16) << "op " << op << " exceeded 16ms budget: "
									   << elapsed.count() << "ms";
	};

	steady_clock::time_point t0, t1;

	t0 = steady_clock::now();
	node_->mwa_connect(identity, cluster, opts);
	t1 = steady_clock::now();
	within_budget(t0, t1, "mwa_connect");

	t0 = steady_clock::now();
	node_->reauthorize(opts);
	t1 = steady_clock::now();
	within_budget(t0, t1, "reauthorize");

	t0 = steady_clock::now();
	node_->mwa_disconnect();
	t1 = steady_clock::now();
	within_budget(t0, t1, "mwa_disconnect");

	t0 = steady_clock::now();
	node_->deauthorize();
	t1 = steady_clock::now();
	within_budget(t0, t1, "deauthorize");

	t0 = steady_clock::now();
	node_->sign_messages(msgs, opts);
	t1 = steady_clock::now();
	within_budget(t0, t1, "sign_messages");

	t0 = steady_clock::now();
	node_->sign_transactions(txns, opts);
	t1 = steady_clock::now();
	within_budget(t0, t1, "sign_transactions");

	t0 = steady_clock::now();
	node_->sign_and_send(txns, opts);
	t1 = steady_clock::now();
	within_budget(t0, t1, "sign_and_send");
}

// ============================================================================
// AC-2 thread-hop — MockMwaAndroidBridge::drive_signal_from_thread spawns a
// worker thread that calls dispatcher.post() with a signal. After thread join
// + snapshot, the entry is visible with the expected signal_name and payload.
// Wall-clock budget for the full drive+join+snapshot sequence: ≤16 ms.
// ============================================================================
TEST_F(MobileWalletAdapterTest, ThreadHop_SignalEmitsOnMainFromWorkerThread) {
	mwa::GodotMainDispatcher &dispatcher = node_->dispatcher_for_testing();

	// Mock shares the node's dispatcher so driven signals land in the same queue.
	auto mock = std::unique_ptr<mwa::MockMwaAndroidBridge>(
			new mwa::MockMwaAndroidBridge(&dispatcher));
	mwa::MockMwaAndroidBridge *mock_raw = mock.get();
	node_->set_bridge_for_testing(std::move(mock));

	godot::Dictionary payload;
	payload["marker"] = "thread-hop-test";

	const steady_clock::time_point t0 = steady_clock::now();
	mock_raw->drive_signal_from_thread(
			godot::String("mwa-test-worker"),
			godot::String("mwa_error"),
			payload);
	// drive_signal_from_thread joins the worker thread before returning (per
	// MockMwaAndroidBridge contract). pending_ now carries the entry.

	godot::Array snapshot = dispatcher.snapshot_pending_for_testing();
	const steady_clock::time_point t1 = steady_clock::now();

	ASSERT_EQ(snapshot.size(), 1);
	godot::Dictionary entry = snapshot[0];
	godot::String name = entry["signal_name"];
	godot::Dictionary captured_payload = entry["payload"];

	EXPECT_TRUE(name == godot::String("mwa_error"));
	EXPECT_TRUE(godot::String(captured_payload["marker"]) == godot::String("thread-hop-test"));

	const auto elapsed = std::chrono::duration_cast<milliseconds>(t1 - t0);
	EXPECT_LE(elapsed.count(), 16) << "thread-hop sequence exceeded 16ms: "
								   << elapsed.count() << "ms";
}

// ============================================================================
// AC-3 pre-op error — with set_bridge_for_testing(nullptr), mwa_connect posts
// mwa_error{UNSUPPORTED_PLATFORM, ...} and does NOT call through to any bridge.
// ============================================================================
TEST_F(MobileWalletAdapterTest, PreOp_UnsupportedPlatformWhenKotlinNotLoaded) {
	node_->set_bridge_for_testing(nullptr);

	godot::Dictionary identity;
	godot::Dictionary opts;
	node_->mwa_connect(identity, godot::String("devnet"), opts);

	mwa::GodotMainDispatcher &dispatcher = node_->dispatcher_for_testing();
	godot::Array snapshot = dispatcher.snapshot_pending_for_testing();

	ASSERT_EQ(snapshot.size(), 1);
	godot::Dictionary entry = snapshot[0];
	godot::String name = entry["signal_name"];
	godot::Dictionary payload = entry["payload"];

	EXPECT_TRUE(name == godot::String("mwa_error"));
	EXPECT_TRUE(godot::String(payload["code"]) == godot::String("UNSUPPORTED_PLATFORM"));
	EXPECT_TRUE(godot::String(payload["developer_details"]) == godot::String("Kotlin plugin not loaded"));
	EXPECT_TRUE(godot::String(payload["source_method"]) == godot::String("connect"));
	EXPECT_EQ(static_cast<bool>(payload["recoverable"]), false);
	EXPECT_TRUE(godot::String(payload["retry_hint"]) == godot::String("none"));
	EXPECT_TRUE(godot::String(payload["layer"]) == godot::String("cpp"));
}

} // anonymous namespace
} // namespace godot_solana_sdk
