export function clear_state() {
    Module.wallet_status = 0;
    Module.old_serialized_message = new Uint8Array();
    Module.tampered_serialized_message = new Uint8Array();
}

export async function signMessageProcedure(buffer_message) {
    if (Module.wallet_status == -1) {
        console.warn("Wallet Adapter is busy.");
        return;
    }

    if (typeof (Module.wallet_handler) == "undefined") {
        console.error("Wallet adapter is not connected.")
        return;
    }

    async function storeMessageSignature() {
        try {
            buffer_message = new TextEncoder().encode(buffer_message);
            Module.message_signature = (await Module.wallet_handler.signMessage(buffer_message, 'utf8')).signature;
            if (Module.message_signature == null) {
                Module.wallet_status = -1;
                return;
            }
        }
        catch (err) {
            Module.wallet_status = -1;
            console.error(err);
            return;
        }
        Module.wallet_status = 1;
    }

    storeMessageSignature();
}

export async function signTransactionProcedure(signer_index) {
    if (Module.wallet_status == -1) {
        console.warn("Wallet Adapter is busy.");
        return;
    }

    if (typeof (Module.wallet_handler) == "undefined") {
        console.error("Wallet adapter is not connected.")
        return;
    }

    async function storeTransactionSignature() {
        try {
            Module.old_serialized_message = new Uint8Array(Module.serialized_message);
            var tx = Module.Transaction.from(Module.serialized_message);
            var response = (await Module.wallet_handler.signTransaction(tx));
            Module.tampered_serialized_message = response.serialize({ requireAllSignatures: false, verifySignatures: false });
            console.log(response);
            Module.message_signature = response.signatures[signer_index].signature;
            if (Module.message_signature == null) {
                Module.wallet_status = -1;
                Module.serialized_message = [];
                return;
            }
        }
        catch (err) {
            Module.wallet_status = -1;
            Module.serialized_message = [];
            console.error(err);
            return;
        }
        Module.wallet_status = 1;
    }

    storeTransactionSignature();
}

export async function connectWalletProcedure(walletName) {
    if (Module.wallet_status == -1) {
        console.warn("Wallet Adapter is busy.");
        return;
    }

    async function connectWallet() {
        try {
            Module.wallet_handler = new Module.wallets[`${walletName}WalletAdapter`];
            await Module.wallet_handler.connect();
            Module.connect_key = Module.wallet_handler.publicKey.toString();
            Module.wallet_status = 1;
        }
        catch (err) {
            Module.wallet_status = -1;
            console.error(err);
        }
    }

    connectWallet();
}

export function isWalletInstalled(walletName) {
    try {
        var adapter = new Module.wallets[`${walletName}WalletAdapter`];
        return adapter.readyState == "Installed";
    }
    catch (err) {
        console.error(err);
        return false;
    }
}