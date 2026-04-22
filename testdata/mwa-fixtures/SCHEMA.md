# MWA Fixture Schema v1

Test-fixture format consumed by `FakeMwaClient` (Kotlin) and the Fake Wallet APK
contract tier (Story 3/4 T4). Source of truth for both fakes per DD-28.

## Filename convention

```
{op}_{scenario}.json
```

`op` is one of the 7 MWA ops; `scenario` is the situational variant
(`success`, `user_canceled`, `token_expired`, …).

### Filename parser (LOCKED)

The op enum contains two names that share a prefix (`sign_messages`,
`sign_transactions`, `sign_and_send`). A naive `split("_")` is ambiguous —
the parser MUST use **longest-prefix match against the op enum**, trying
candidates in descending length order:

```
sign_and_send       (13 chars)
sign_transactions   (17 chars)
sign_messages       (13 chars)
reauthorize         (11 chars)
deauthorize         (11 chars)
disconnect          (10 chars)
authorize           (9 chars)
```

Algorithm:
1. Strip the `.json` suffix.
2. For each op (in the order above), check if the filename starts with `{op}_`.
3. The **first** matching op wins; the remainder (after `{op}_`) is the scenario.
4. If no op matches → reject the filename.

### Self-consistency

After parsing, the filename-derived `op` and `scenario` MUST equal the JSON
document's `op` and `scenario` fields exactly. A mismatch is a validator failure
(PR-blocking via `tools/validate_fixtures.py`).

## JSON Schema v1

```jsonc
{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "type": "object",
  "required": ["schema_version", "op", "scenario", "expected_input", "response"],
  "additionalProperties": false,
  "properties": {
    "schema_version": { "const": 1 },
    "op": {
      "enum": [
        "authorize", "reauthorize", "deauthorize", "disconnect",
        "sign_messages", "sign_transactions", "sign_and_send"
      ]
    },
    "scenario": {
      "type": "string",
      "pattern": "^[a-z0-9][a-z0-9_]*$"
    },
    "expected_input": {
      "type": "object",
      "description": "Shape of args the test expects the op to be called with. Validator checks structural presence; tests assert concrete values separately."
    },
    "response": {
      "oneOf": [
        {
          "required": ["type", "payload"],
          "additionalProperties": false,
          "properties": {
            "type": { "const": "success" },
            "payload": { "type": "object" }
          }
        },
        {
          "required": ["type", "code"],
          "additionalProperties": false,
          "properties": {
            "type": { "const": "error" },
            "code": {
              "enum": [
                "USER_CANCELED", "WALLET_REJECTED", "NO_MWA_WALLET_INSTALLED",
                "WALLET_UNINSTALLED", "TOKEN_EXPIRED", "RPC_FAILED",
                "NETWORK_OFFLINE", "TIMEOUT", "NOT_CONNECTED",
                "UNSUPPORTED_PLATFORM", "PROTOCOL_ERROR", "STORAGE_CORRUPT",
                "REAUTH_REQUIRED"
              ]
            }
          }
        }
      ]
    }
  }
}
```

`additionalProperties: false` is set at both the top level AND on each
`response` oneOf branch. Stray fields — including the previously-considered
`developer_details` string on the error branch, which was dropped because
`MwaError` singletons have no field to thread it through — are rejected by
`tools/validate_fixtures.py` rather than silently accepted.

### Per-op success-payload required keys

The top-level schema intentionally types `response.payload` as `object` with
no required keys so the schema stays open to future fields. The
per-op shape contract `FakeMwaClient` relies on is enforced as a second-stage
check in `tools/validate_fixtures.py` via `REQUIRED_PAYLOAD_KEYS`:

| op | Required keys on `response.payload` (success branch) |
|---|---|
| `authorize` / `reauthorize` | `auth_token`, `public_key`, `cluster`, `chain_id` |
| `deauthorize` / `disconnect` | (none — empty `payload: {}` is valid) |
| `sign_messages` / `sign_transactions` | `signed_payloads` |
| `sign_and_send` | `signatures` |

A fixture with `response.type = "success"` and a `payload` missing any of the
above keys is rejected at CI time rather than blowing up at
`FakeMwaClient` parse time.

## Encoding conventions

All base64 fields use **RFC 4648 §4 standard alphabet with `=` padding** —
the default form produced by `java.util.Base64.getEncoder()` / decoded by
`java.util.Base64.getDecoder()`. URL-safe base64 (§5, `-`/`_` alphabet) is NOT
accepted. Fixture authors must pad to a 4-char boundary.

- `auth_token` — **opaque string**. Treated as an opaque blob by both MWA impls;
  although wallets in practice emit base64 characters, the fixture consumer
  (`FakeMwaClient`) stores the string's UTF-8 bytes in `SecretString` without
  decoding, and `MwaClientImpl` pushes it back to the adapter via an identity
  transform (`adapter.authToken = String(reveal(), UTF_8)`). See `AuthResult`
  KDoc for the opaque-invariant rationale.
- `public_key` — **base64** (not base58). 32-byte ed25519 public key; base64 is
  44 chars with `=` padding. `FakeMwaClient` decodes via
  `java.util.Base64.getDecoder().decode(...)`.
- `messages` / `addresses` / `transactions` / `signed_payloads` (in
  `expected_input` or `payload`) — base64 strings.
- `signatures` (in `sign_and_send` success payload) — base58 strings, per Solana
  RPC convention. `FakeMwaClient` returns them as `List<String>` without
  decoding; callers decode if they need raw bytes.

## Scenario name vs `response.code`

The scenario name describes the **situational cause**; `response.code` is the
protocol-level error the wallet actually returns. These may differ intentionally.
Example: `reauthorize_cluster_mismatch.json` returns `code: "NOT_CONNECTED"` —
the cache-key mismatch (per DD-27 and Story 2-2 AC-4) forces a fresh authorize,
and the wallet-facing error is `NOT_CONNECTED` even though the developer reading
the fixture name sees the underlying cause as a cluster mismatch.

## Retry hints

Fixtures do not specify `retry_hint` directly. When `FakeMwaClient` returns
`MwaResult.Failure(error)`, the `retry_hint` is inherited from the matched
`MwaError` singleton (generated by Story 1-1's `tools/gen_error_codes.py` from
the error-codes manifest). E.g., `NO_MWA_WALLET_INSTALLED` →
`retryHint="install_wallet"` per Amendment A-1.

## Adding a new fixture

1. Name the file `{op}_{scenario}.json` where `op` is an enum member and
   `scenario` matches `^[a-z0-9][a-z0-9_]*$`.
2. Fill in `schema_version: 1`, `op`, `scenario`, `expected_input`, and one of
   the two `response` shapes.
3. Run `python tools/validate_fixtures.py testdata/mwa-fixtures/` locally
   before pushing — CI will block the PR on any violation.
