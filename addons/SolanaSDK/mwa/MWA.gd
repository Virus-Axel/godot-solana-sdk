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
signal connect_completed(request_id: String, result: Dictionary)
signal reauthorize_completed(request_id: String, result: Dictionary)
signal disconnect_completed(request_id: String, result: Dictionary)
signal deauthorize_completed(request_id: String, result: Dictionary)
signal sign_messages_completed(request_id: String, result: Dictionary)
signal sign_transactions_completed(request_id: String, result: Dictionary)
signal sign_and_send_completed(request_id: String, result: Dictionary)
signal mwa_error(payload: Dictionary)
signal mwa_timeout(payload: Dictionary)
signal mwa_cancelled_lifecycle(payload: Dictionary)
signal reauth_required(payload: Dictionary)

# The underlying C++ MWA node (GDExtension-registered). Instantiated in
# `_ready` via `MobileWalletAdapter.new()`. Kept private so consumers are
# forced through the facade (ensures signal forwarding + plan-spec naming
# stay consistent).
var _node: MobileWalletAdapter = null

# AC-4 support: suggested install links when no MWA-compatible wallet is
# detected. Static data — no network call; the game prompts the user with
# this list when `mwa_error{code=NO_MWA_WALLET_INSTALLED}` fires. Three
# reference wallets (Phantom / Solflare / Backpack); each entry carries a
# playstore URL for in-Android-store install + a website URL for non-Android
# fallback. URLs reviewed 2026-04-24 against vendor canonical homepages.
const _SUGGESTED_WALLETS: Array[Dictionary] = [
	{
		"name": "Phantom",
		"play_store_url": "https://play.google.com/store/apps/details?id=app.phantom",
		"website_url": "https://phantom.com",
	},
	{
		"name": "Solflare",
		"play_store_url": "https://play.google.com/store/apps/details?id=com.solflare.mobile",
		"website_url": "https://solflare.com",
	},
	{
		"name": "Backpack",
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
func connect(identity: Dictionary, cluster: String, opts: Dictionary = {}) -> String:
	return _node.mwa_connect(identity, cluster, opts)


## Tear down an active session. Completion arrives via `disconnect_completed`
## (Story 2-3 wires the real path; Story 2-1 emits NOT_CONNECTED-equivalent
## scaffold error).
@warning_ignore("shadowed_global_identifier")
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
func sign_transactions(transactions: Array[PackedByteArray], opts: Dictionary = {}) -> void:
	_node.sign_transactions(transactions, opts)


## Synchronous state read — true after a successful connect, false otherwise.
## Backed by MwaSessionState.authToken != null (arch §7.1; round-tripped via
## MwaJniContext::query_session_state).
@warning_ignore("shadowed_global_identifier")
func is_connected() -> bool:
	return _node.mwa_is_connected()


## Base58 public key of the connected wallet, or empty string pre-connect /
## on non-Android.
func get_public_key() -> String:
	return _node.get_public_key()


## Cluster name (e.g. "devnet") of the active session, or empty string
## pre-connect.
func get_cluster() -> String:
	return _node.get_cluster()


## Wallet-reported human-readable label, or empty string pre-connect.
func get_wallet_label() -> String:
	return _node.get_wallet_label()


## 8-lowercase-hex fingerprint of the current auth_token (AC-7), or empty
## string pre-connect. The fingerprint is HKDF-SHA256(auth_token, per-install
## salt) — salt rotates on `forget_all` (Story 4-2), so the fingerprint is
## stable within a single install lifecycle and changes after deauth+rotate.
func get_auth_token_fingerprint() -> String:
	return _node.get_auth_token_fingerprint()


## Suggested wallet install links for AC-4 UX (display when `mwa_error` fires
## with code=NO_MWA_WALLET_INSTALLED). Each entry has `name`, `play_store_url`,
## and `website_url` keys. Three reference wallets: Phantom / Solflare /
## Backpack. Static data — no network call.
func get_suggested_wallet_install_links() -> Array[Dictionary]:
	return _SUGGESTED_WALLETS.duplicate(true)
