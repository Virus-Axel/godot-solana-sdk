# Signing

Once a wallet session is established, you can request the user to sign messages, transactions, or sign-and-broadcast transactions. All signing methods open the wallet app via an Android Intent and return once the user approves or rejects.

---

## `signMessages()`

Ask the wallet to sign arbitrary raw byte payloads (off-chain messages). The result contains one signature per message.

```gdscript
# Sign a single UTF-8 message
var msg: PackedByteArray = "Hello from Godot!".to_utf8_buffer()
var result: Dictionary = await mwa.signMessages([msg])

if result.has("signed_messages"):
    var sigs: Array    = result["signed_messages"]   # Array of PackedByteArray
    var addrs: Array   = result["addresses"]          # Signing addresses
    print("Signed by: ", addrs[0])
    print("Signature bytes: ", sigs[0].size())
elif result.has("error") and result["error"] == "user_cancelled":
    print("User cancelled.")
```

> **Signal emitted:** `messages_signed(signed_messages: Array, addresses: Array)` on success, `signing_failed(error)` on failure.

---

## `signTransactions()`

Ask the wallet to sign one or more serialized Solana transactions **without** broadcasting them. Returns the signed transaction bytes so you can inspect or broadcast them yourself later.

```gdscript
# Build a transaction
var sender: Pubkey  = Pubkey.new_from_string("YourWalletAddress...")
var receiver: Pubkey = Keypair.new_random().to_pubkey()

var ix  = SystemProgram.transfer(sender, receiver, 1000)
var tx  = Transaction.new()
tx.add_instruction(ix)
tx.set_payer(sender)
await SolanaService.transaction_manager.update_blockhash(tx)

# Serialize and sign
var payload: PackedByteArray = tx.serialize()
var result: Array = await mwa.signTransactions([payload])

if result.size() > 0 and result[0] is PackedByteArray:
    var signed_bytes: PackedByteArray = result[0]
    print("Signed tx size: ", signed_bytes.size(), " bytes")
else:
    print("Signing cancelled or failed.")
```

> **Signal emitted:** `transactions_signed(signed_transactions: Array)` on success.

---

## `signAndSendTransactions()`

Ask the wallet to sign a transaction AND broadcast it to the configured Solana cluster. This is the most common action for a Solana game — transfer tokens, mint NFTs, call a smart contract.

```gdscript
var sender: Pubkey  = _get_my_wallet_pubkey()
var receiver: Pubkey = Keypair.new_random().to_pubkey()

var ix = SystemProgram.transfer(sender, receiver, 1_000_000)  # 0.001 SOL
var tx = Transaction.new()
tx.add_instruction(ix)
tx.set_payer(sender)
tx.set_unit_limit(800_000)   # optional: compute budget
tx.set_unit_price(8_000)     # optional: priority fee in microlamports
await SolanaService.transaction_manager.update_blockhash(tx)

var payload: PackedByteArray = tx.serialize()
var result: Dictionary = await mwa.signAndSendTransactions([payload])

if result.has("transaction_signatures"):
    var sigs: Array = result["transaction_signatures"]
    print("Broadcast confirmed! Sig: ", sigs[0])
elif result.has("error"):
    print("Failed: ", result["error"])
```

> **Note on Rent Exemption:** Sending to a **brand new** account requires at least **890,880 lamports** to make it rent-exempt. Sending less may result in `InsufficientFundsForRent` during simulation. The example above sends 1,000,000 lamports (0.001 SOL) to be safe.

> **Signal emitted:** The signed transaction signature is returned in the result dictionary's `transaction_signatures` array.

---

## Error Handling

All signing methods may return `{ "error": "user_cancelled" }` if the user denied the request in the wallet. Always check for this:

```gdscript
var result = await mwa.signAndSendTransactions([payload])
if result.has("error") and result["error"] == "user_cancelled":
    print("User declined the transaction.")
    return
```
