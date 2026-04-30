# Mobile Wallet Adapter (MWA) singleton facade — Story 2-1 T7.
#
# DD-18 autoload singleton registered by `addons/SolanaSDK/solana_sdk_plugin.gd`.
# Owns one `MobileWalletAdapter` C++ node (Component B, arch §2.2) and forwards
# 11 signals + 8 methods to GDScript consumers.
#
# Shadowed-method note (D-10 / CR-24): public facade methods use plan-spec
# names — `connect`, `disconnect`, `is_connected` — which shadow `Node.connect`,
# `Node.disconnect`, `Node.is_connected`. GDScript emits a
# `shadowed_global_identifier` warning; each method is annotated with
# `@warning_ignore("shadowed_global_identifier")` so the intentional shadowing
# doesn't pollute the editor warnings panel. Rationale: consumer ergonomics
# (`MWA.connect(...)` reads naturally as the MWA op); the C++ node uses the
# `mwa_*` rename to avoid inherited-method collisions at the GDExtension-
# binding layer (D-1). The facade carries the ergonomic names; the underlying
# node carries the collision-safe names.
#
# Signal forwarding: the 11 signals are declared here with the SAME shapes as
# the C++ node (A-12 2-param *_completed; 1-param error/lifecycle). `_ready`
# wires each child signal to this node's same-named signal's `emit` method —
# so consumers can `MWA.connect_completed.connect(my_handler)` without
# reaching through `MWA._node`.
extends Node

# 7 *_completed signals take (String request_id, Dictionary result) per A-12.
# 4 error/lifecycle signals take (Dictionary payload) per arch §3.2.
## @since v0.1.0
signal connect_completed(request_id: String, result: Dictionary)
## @since v0.1.0
signal reauthorize_completed(request_id: String, result: Dictionary)
## @since v0.1.0
signal disconnect_completed(request_id: String, result: Dictionary)
## @since v0.1.0
signal deauthorize_completed(request_id: String, result: Dictionary)
## @since v0.1.0
signal sign_messages_completed(request_id: String, result: Dictionary)
## @since v0.1.0
signal sign_transactions_completed(request_id: String, result: Dictionary)
## @since v0.1.0
signal sign_and_send_completed(request_id: String, result: Dictionary)
## @since v0.1.0
signal mwa_error(payload: Dictionary)
## @since v0.1.0
signal mwa_timeout(payload: Dictionary)
## @since v0.1.0
signal mwa_cancelled_lifecycle(payload: Dictionary)
## @since v0.1.0
signal reauth_required(payload: Dictionary)
## Story 3-3 — fired AFTER a successful `connect_completed` /
## `reauthorize_completed` if a stale sign_and_send breadcrumb survived a
## process death (AC-5). One-shot semantics per DD-3-3-D: each pending
## breadcrumb produces ONE emission and is then cleared. Payload is the
## 6-key dict `{request_id, op_type, started_at_ms, tx_count,
## tx_preview_hashes, recommendation: "check_chain_for_signatures"}` per
## DD-3-3-E. Game logic should query the chain for any of the
## `tx_preview_hashes` to determine whether the previous submission landed
## before showing UX.
## @since v0.1.0
signal pending_submission_found(payload: Dictionary)

# The underlying C++ MWA node (GDExtension-registered). Instantiated in
# `_ready` via `MobileWalletAdapter.new()`. Kept private so consumers are
# forced through the facade (ensures signal forwarding + plan-spec naming
# stay consistent).
var _node: MobileWalletAdapter = null

# AC-4 support (Story 1-5) + AC-5 reconciliation (Story 5-2 DD-5-2-4): suggested
# install links when no MWA-compatible wallet is detected. Static data — no
# network call; the game prompts the user with this list when
# `mwa_error{code=NO_MWA_WALLET_INSTALLED}` fires. Three reference wallets
# (Phantom / Solflare / Backpack); each entry carries 4 keys per DD-5-2-4
# superset: `name`, `package_id` (matches the `?id=` query param of
# `play_store_url`), `play_store_url` (Android Play Store install), and
# `website_url` (non-Android fallback). DD-5-2-4 LOCKED the superset shape
# rather than the strict AC-5 3-key (`{name, package_id, play_store_url}`)
# to keep backward compatibility with existing callers reading `website_url`.
# URLs reviewed 2026-04-24 against vendor canonical homepages.
const _SUGGESTED_WALLETS: Array[Dictionary] = [
	{
		"name": "Phantom",
		"package_id": "app.phantom",
		"play_store_url": "https://play.google.com/store/apps/details?id=app.phantom",
		"website_url": "https://phantom.com",
	},
	{
		"name": "Solflare",
		"package_id": "com.solflare.mobile",
		"play_store_url": "https://play.google.com/store/apps/details?id=com.solflare.mobile",
		"website_url": "https://solflare.com",
	},
	{
		# Story 5-2 D-5-2-T4-1 Rule 1 — story spec said `app.backpack`, but the
		# `play_store_url` (reviewed against the vendor's canonical homepage on
		# 2026-04-24) carries `?id=app.backpack.mobile`. URL is the source of
		# truth; T5's `tools/test_wallet_install_links.gd` asserts against the
		# real package id. Documented at story-creation HALT.
		"name": "Backpack",
		"package_id": "app.backpack.mobile",
		"play_store_url": "https://play.google.com/store/apps/details?id=app.backpack.mobile",
		"website_url": "https://backpack.app",
	},
]


func _ready() -> void:
	# GDExtension registers `MobileWalletAdapter` at load time (verified by the
	# Story 1-5 binding-smoke inspection). Using `.new()` avoids the need for
	# a sibling .tscn and keeps the autoload self-contained.
	_node = MobileWalletAdapter.new()
	add_child(_node)
	_wire_signal_forwarding()


# Connect the child's 11 signals to this node's 11 same-named signals' `emit`
# methods. Godot 4 Callables expose `Signal.emit` as a Callable itself; this
# makes the forwarding a one-liner per signal and preserves argument shapes
# by inference.
func _wire_signal_forwarding() -> void:
	_node.connect_completed.connect(connect_completed.emit)
	_node.reauthorize_completed.connect(reauthorize_completed.emit)
	_node.disconnect_completed.connect(disconnect_completed.emit)
	_node.deauthorize_completed.connect(deauthorize_completed.emit)
	_node.sign_messages_completed.connect(sign_messages_completed.emit)
	_node.sign_transactions_completed.connect(sign_transactions_completed.emit)
	_node.sign_and_send_completed.connect(sign_and_send_completed.emit)
	_node.mwa_error.connect(mwa_error.emit)
	_node.mwa_timeout.connect(mwa_timeout.emit)
	_node.mwa_cancelled_lifecycle.connect(mwa_cancelled_lifecycle.emit)
	_node.reauth_required.connect(reauth_required.emit)
	_node.pending_submission_found.connect(pending_submission_found.emit)


# ---------------- Public facade — 8 methods per T7 spec ----------------


## Initiate a wallet connect (authorize). Returns the generated 8-lowercase-hex
## request_id the facade+C++ node will attach to the eventual terminal signal
## (connect_completed / mwa_error / mwa_timeout) so callers can correlate.
##
## `identity` shape: {"name": String, "icon_uri": String, "identity_uri": String}.
## `cluster`: "devnet" / "mainnet-beta" / "testnet" / etc.
## `opts` (optional): {"timeout_ms": int, "chain_id": String}. Omitted keys
##   fall to the C++ node's defaults (DD-23 watchdog; "solana:<cluster>" chain_id).
@warning_ignore("shadowed_global_identifier")
## @since v0.1.0
func connect(identity: Dictionary, cluster: String, opts: Dictionary = {}) -> String:
	return _node.mwa_connect(identity, cluster, opts)


## Tear down an active session. Completion arrives via `disconnect_completed`
## (Story 2-3 wires the real path; Story 2-1 emits NOT_CONNECTED-equivalent
## scaffold error).
@warning_ignore("shadowed_global_identifier")
## @since v0.1.0
func disconnect() -> void:
	_node.mwa_disconnect()


## Revoke the wallet's auth_token AND wipe ALL local cache records keyed under
## this caller's identity (FR-5 / AC-NFR-SEC-5). Always clears local state
## regardless of the remote outcome — the threat model is a returned/stolen
## device that must not retain a working auth_token. Completion arrives via:
##   - `deauthorize_completed(request_id, result)` on every call —
##     `result` is `{request_id, remote_revoke_succeeded, local_cache_cleared,
##     warning}` per arch.md:669 + DD-4-1-1. `local_cache_cleared` is always
##     `true`; `remote_revoke_succeeded` reflects whether the wallet's
##     remote revoke endpoint was reachable; `warning` is `"remote_unreachable"`
##     on remote failure (network off, wallet uninstalled) or `""` on success.
##   - `mwa_error{code=PROTOCOL_ERROR, source_method="deauthorize"}` ONLY
##     on catastrophic local failures (e.g. corrupted keystore wipe crash —
##     DD-4-1-3 nested defensive try/catch). Remote-revoke failure does NOT
##     route here per DD-4-1-1; it surfaces as `deauthorize_completed{
##     remote_revoke_succeeded: false, warning: "remote_unreachable"}`.
##
## Idempotent (AC-4): calling on an already-deauthorized session emits
## `deauthorize_completed{remote_revoke_succeeded: true, local_cache_cleared:
## true, warning: ""}` (vacuous success per DD-4-1-4 — nothing to revoke).
##
## No watchdog: AC-2 requires remote failure to surface as `remote_unreachable`,
## NOT `mwa_timeout`. clientlib-ktx's internal per-call timeout is the upstream
## guard.
## @since v0.1.0
func deauthorize() -> void:
	_node.deauthorize()


## Silent re-authorization using the cached auth token. No wallet UI is shown
## (NFR-5). Completion arrives via one terminal signal per request:
##   - `reauthorize_completed(request_id, result)` on success (AC-1).
##   - `mwa_error{code=NOT_CONNECTED}` if no cached token is present (AC-2) or
##     if the stored cluster/chain-id does not match the caller's args (AC-4
##     cluster-mismatch); retry hint is "connect" in both cases.
##   - `mwa_error{code=TOKEN_EXPIRED}` if the wallet rejects the cached token
##     and the token is wiped from secure storage (AC-3 graceful wipe).
##   - `mwa_timeout` on watchdog expiry (DD-2-2-3).
##
## `opts` (optional): {"timeout_ms": int} — clamped to internal default per
## DD-23. Omitted keys fall to the C++ node's defaults.
## @since v0.1.0
func reauthorize(opts: Dictionary = {}) -> void:
	_node.reauthorize(opts)


## Sign one or more byte-array messages with the connected wallet (wallet UI
## shown for confirmation per AC-2). Completion arrives via one terminal
## signal per request:
##   - `sign_messages_completed(request_id, result)` on success — `result`
##     carries `{request_id, signed_payloads: [PackedByteArray, ...]}` per
##     AC-2 / DD-3-1-5; signed payloads are caller message bytes prefixed
##     with the wallet's 64-byte ed25519 signature (decoded length ≥ input
##     length + 64).
##   - `mwa_error{code=NOT_CONNECTED}` if `is_connected()` is false at call
##     time — synchronous preflight per DD-3-1-6 / AC-3 (no scope.launch on
##     the disconnected branch).
##   - `mwa_error{code=USER_CANCELED}` if the wallet user dismisses the
##     signing prompt — AC-4, routed through the single `mwa_error` signal
##     per DD-15 (no per-op error signals across the surface).
##   - `mwa_timeout` on watchdog expiry (DD-3-1-3 effectiveWatchdog parity).
##
## Caller correlates terminal signals via the `request_id` field. The C++
## node mints the ID via `generate_request_id` (D-4 from Story 1-5); per the
## LOCKED Story 2-1 T7 convention (mobile_wallet_adapter.hpp:55-59 "the
## remaining 6 op methods stay void") the facade returns void — same shape
## as `disconnect` / `reauthorize` above.
##
## `messages`: list of message byte arrays (1+ entries).
## `opts` (optional): {"timeout_ms": int} — clamped to internal default per
## DD-3-1-3. Omitted keys fall to the C++ node's defaults.
## @since v0.1.0
func sign_messages(messages: Array[PackedByteArray], opts: Dictionary = {}) -> void:
	_node.sign_messages(messages, opts)


## Sign one or more serialized Solana transactions with the connected wallet
## (wallet UI shown for confirmation per AC-1). Completion arrives via one
## terminal signal per request:
##   - `sign_transactions_completed(request_id, result)` on success — `result`
##     carries `{request_id, signed_transactions: [PackedByteArray, ...]}` per
##     AC-1 / DD-3-2-3; each signed transaction is the caller's serialized
##     transaction with the wallet's signature(s) populated. The payload-array
##     key is `signed_transactions` (renamed from the op-agnostic
##     `signed_payloads` wire format on the Kotlin side via
##     `buildSignSuccessJson(payloadKey="signed_transactions")` per
##     DD-3-2-3 + D-3-2-1).
##   - `mwa_error{code=NOT_CONNECTED}` if `is_connected()` is false at call
##     time — synchronous preflight per DD-3-1-6 (inherited via DD-3-2-5) /
##     AC-3 (no scope.launch on the disconnected branch).
##   - `mwa_error{code=WALLET_REJECTED}` if the wallet user rejects the signing
##     prompt — AC-4, routed through the single `mwa_error` signal per DD-15
##     (no per-op error signals across the surface).
##   - `mwa_timeout` on watchdog expiry (DD-3-1-3 effectiveWatchdog parity,
##     inherited).
##
## Caller correlates terminal signals via the `request_id` field. The C++
## node mints the ID via `generate_request_id` (D-4 from Story 1-5); per the
## LOCKED Story 2-1 T7 convention (mobile_wallet_adapter.hpp:55-59 "the
## remaining 6 op methods stay void") and Story 3-1 D-3-1-12, the facade
## returns void — same shape as `sign_messages` above.
##
## `transactions`: list of serialized transaction byte arrays (1+ entries).
## `opts` (optional): {"timeout_ms": int} — clamped to internal default per
## DD-3-1-3. Omitted keys fall to the C++ node's defaults.
## @since v0.1.0
func sign_transactions(transactions: Array[PackedByteArray], opts: Dictionary = {}) -> void:
	_node.sign_transactions(transactions, opts)


## Sign one or more serialized Solana transactions AND submit them to the
## connected cluster's RPC endpoint via the wallet (single round-trip).
## Completion arrives via one terminal signal per request:
##   - `sign_and_send_completed(request_id, result)` on success — `result`
##     carries the 4-key shape `{request_id, signatures: Array[String],
##     submitted_at: int, confirmation_status: "submitted"}` per AC-1 +
##     DD-3-3-E. `signatures` are base58-encoded transaction signatures
##     (NOT base64-encoded byte arrays — the Solana convention for tx
##     signatures is base58, distinct from the base64 wire format used in
##     `sign_messages` / `sign_transactions`). `submitted_at` is the
##     wall-clock ms-epoch the wallet acknowledged forwarding the txs to
##     the RPC; `confirmation_status` is the literal "submitted" today
##     (future Story 5-x may extend to "confirmed" / "finalized" if a
##     confirmation-tracking surface lands).
##   - `mwa_error{code=NOT_CONNECTED}` if `is_connected()` is false OR
##     the active session's cluster does not match a cached record's
##     cluster (cluster-bleed defensive guard per DD-27 / AC-NFR-SEC-4 /
##     AC-6) — synchronous preflight, no scope.launch on the disconnected
##     branch.
##   - `mwa_error{code=WALLET_REJECTED}` if the wallet user rejects the
##     signing prompt — AC-4, routed through the single `mwa_error` signal
##     per DD-15.
##   - `mwa_error{code=NETWORK_OFFLINE | RPC_FAILED}` if the wallet cannot
##     reach the RPC endpoint or the cluster rejects the submission.
##   - `mwa_timeout` on watchdog expiry (DD-3-1-3 effectiveWatchdog parity,
##     inherited).
##   - `reauth_required{reason: "keystore_corrupt"}` if a Tink corruption
##     event is detected during the breadcrumb-write site (DD-3-3-G
##     fail-closed wrapper).
##
## **Pending-submission breadcrumb** (AC-2 / AC-5): BEFORE the wallet
## round-trip starts, this op writes a 7-field breadcrumb to encrypted
## storage `{request_id, op_type:"sign_and_send", cluster, identity_uri,
## started_at_ms, tx_count, tx_preview_hashes:[sha256(tx)…]}` per
## DD-3-3-A/B/F. The breadcrumb is removed in ALL terminal paths
## (success/error/timeout) per DD-3-3-C. If the process is killed
## mid-op, the breadcrumb survives and the next successful
## `connect_completed` / `reauthorize_completed` will be followed by a
## one-shot `pending_submission_found` signal so game logic can query
## the chain for the preview-hashed transactions.
##
## Caller correlates terminal signals via the `request_id` field. The C++
## node mints the ID via `generate_request_id` (D-4 from Story 1-5); per
## the LOCKED Story 2-1 T7 convention and Story 3-1 D-3-1-12, the facade
## returns void — same shape as `sign_messages` / `sign_transactions`
## above.
##
## `transactions`: list of serialized transaction byte arrays (1+ entries).
## `opts` (optional): {"timeout_ms": int} — clamped to internal default
## per DD-3-1-3. Omitted keys fall to the C++ node's defaults.
## @since v0.1.0
func sign_and_send(transactions: Array[PackedByteArray], opts: Dictionary = {}) -> void:
	_node.sign_and_send(transactions, opts)


## Story 4-2 — GDPR/CCPA "Sign out everywhere" reset. Wipes EVERY cached
## wallet token across all identities, rotates the encryption MasterKey
## (Android Keystore alias), cancels any in-flight ops by emitting
## `mwa_cancelled_lifecycle{request_id, source_method, reason:
## "forget_all_invoked"}` per slot BEFORE the wipe, and best-effort
## remote-deauthorizes each cached wallet (offline wallets do not block
## local wipe per AC-4).
##
## NO completion signal per DD-4-2-8 — `forget_all` is fire-and-forget.
## Callers verify completion via state inspection or a short wait:
##   - `is_connected()` returns `false`
##   - the SecureTokenStore is empty (no observable Godot-side getter; the
##     androidTest tier asserts `store.listAllKeys().isEmpty()`)
##   - the next `connect()` triggers a fresh wallet handshake (no cache hit)
##
## DD-4-2-9 post-rotation teardown: every prior `SecureTokenStore`
## instance becomes invalid because the Keystore-backed MasterKey alias
## is deleted. The next `storeProvider(ctx)` call returns a fresh
## instance whose lazy MasterKey regenerates on first access — production
## behavior is identical to a first-run install.
##
## Mutex serialization (DD-4-2-2 + AC-3): concurrent op-method calls
## block on a plugin-instance Mutex; either complete BEFORE the wipe (and
## have their in-flight slots cancelled per DD-4-2-3) or run AFTER the
## wipe completes (observing clean state). NO partial state observable.
##
## NO `opts` arg per DD-4-2-7 — `forget_all` is a hard reset; per-call
## tunables (timeout / retry policy) would only obscure the security
## intent. Returns void per DD-4-2-8 + D-3-1-12 (no request_id correlation
## without a completion signal).
## @since v0.1.0
func forget_all() -> void:
	_node.forget_all()


## Story 5-1 — Returns true iff this build runs on Android (the only platform
## where MWA ops can succeed). Game code should gate MWA usage on this:
##   if not MWA.is_supported(): show_desktop_fallback_ui()
## Per AC-1 (Story 5-1) every MWA method on non-Android emits
## mwa_error{code=UNSUPPORTED_PLATFORM} via the NoOp bridge path per
## DD-A-4 LOCKED single platform-abstraction TU. This getter is a 1-line
## GDScript-side runtime check (DD-5-1-3 — no C++ binding ceremony for a
## value that is a pure runtime constant of the Godot platform).
## @since v0.1.0
func is_supported() -> bool:
	return OS.get_name() == "Android"


## Synchronous state read — true after a successful connect, false otherwise.
## Backed by MwaSessionState.authToken != null (arch §7.1; round-tripped via
## MwaJniContext::query_session_state).
@warning_ignore("shadowed_global_identifier")
## @since v0.1.0
func is_connected() -> bool:
	return _node.mwa_is_connected()


## Base58 public key of the connected wallet, or empty string pre-connect /
## on non-Android.
## @since v0.1.0
func get_public_key() -> String:
	return _node.get_public_key()


## Cluster name (e.g. "devnet") of the active session, or empty string
## pre-connect.
## @since v0.1.0
func get_cluster() -> String:
	return _node.get_cluster()


## Wallet-reported human-readable label, or empty string pre-connect.
## @since v0.1.0
func get_wallet_label() -> String:
	return _node.get_wallet_label()


## 8-lowercase-hex fingerprint of the current auth_token (AC-7), or empty
## string pre-connect. The fingerprint is HKDF-SHA256(auth_token, per-install
## salt) — salt rotates on `forget_all` (Story 4-2), so the fingerprint is
## stable within a single install lifecycle and changes after deauth+rotate.
## @since v0.1.0
func get_auth_token_fingerprint() -> String:
	return _node.get_auth_token_fingerprint()


## Story 5-2 AC-1 — synchronous diagnostics snapshot. Returns a 12-key
## Dictionary `{sdk_version, clientlib_ktx_version, godot_version,
## android_api_level, session_state, wallet_package, wallet_version,
## auth_token_fingerprint, cluster, last_n_correlation_trace,
## late_result_count, pending_submissions_count}` for support-engineer triage.
## Safe to attach to bug reports — AC-3 invariant: zero auth-token bytes /
## signing key material / PII; the only token-derived field is the
## 8-hex `auth_token_fingerprint` (HKDF-SHA256 truncated, NOT the token).
## On non-Android exports returns the same shape with empty values per
## DD-5-2-3 — callers do NOT need to branch on `is_supported()`.
## @since v0.1.0
func get_diagnostics() -> Dictionary:
	return _node.get_diagnostics()


## Story 5-2 AC-4 — device posture snapshot. Returns a 4-key Dictionary
## `{rooted, debuggable, developer_options_on, adb_enabled}` (all `bool`).
## Sourced from `Build.TAGS`, `ApplicationInfo.FLAG_DEBUGGABLE`,
## `Settings.Secure.DEVELOPMENT_SETTINGS_ENABLED`, and
## `Settings.Global.ADB_ENABLED`.
##
## NON-AUTHORITATIVE — these signals are inexpensive heuristics, NOT
## cryptographic attestation. A determined attacker can mask each signal at
## the OS level. Do NOT use as a security gate; surface as informational
## metadata only (e.g., for support-team triage). On non-Android exports
## returns 4-key all-false per DD-5-2-3.
## @since v0.1.0
func get_device_posture() -> Dictionary:
	return _node.get_device_posture()


## Suggested wallet install links for AC-4 UX (display when `mwa_error` fires
## with code=NO_MWA_WALLET_INSTALLED). Each entry has 4 keys: `name`,
## `package_id` (Story 5-2 DD-5-2-4 superset add — matches the play_store_url
## `?id=` query param), `play_store_url`, and `website_url`. Three reference
## wallets: Phantom / Solflare / Backpack. Static data — no network call.
## @since v0.1.0
func get_suggested_wallet_install_links() -> Array[Dictionary]:
	return _SUGGESTED_WALLETS.duplicate(true)
