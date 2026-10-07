package plugin.walletadapterandroid

import android.app.Activity
import android.net.Uri
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.platform.LocalContext
import com.godotengine.godot_solana_sdk.mwa.session.MwaSessionState
import com.godotengine.godot_solana_sdk.mwa.util.SdkLog
import com.godotengine.godot_solana_sdk.mwa.util.SecretString
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import com.solana.mobilewalletadapter.clientlib.ConnectionIdentity
import com.solana.mobilewalletadapter.clientlib.MobileWalletAdapter
import com.solana.mobilewalletadapter.clientlib.Solana
import com.solana.mobilewalletadapter.clientlib.TransactionResult
import com.solana.mobilewalletadapter.clientlib.successPayload

private const val TAG = "MWA"
private const val CORR_SCAFFOLD = "scaffold"

@Composable
internal fun connectWallet(sender: ActivityResultSender, session: MwaSessionState) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        val connectionIdentity = ConnectionIdentity(
            identityUri = Uri.parse(session.getIdentityUri()),
            iconUri = Uri.parse(session.getIconUri()),
            identityName = session.getIdentityName(),
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (session.getCluster()) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }
        val result = walletAdapter.connect(sender)

        when (result) {
            is TransactionResult.Success -> {
                // MwaSessionState.authToken migrated to SecretString?.
                // clientlib hands us a raw String; wrap at the seam to keep the field
                // redacted in toString / logs. Unwrap on the read side below.
                session.setAuthToken(SecretString(result.authResult.authToken.toByteArray(Charsets.UTF_8)))
                session.setKey(result.authResult.publicKey)
            }
            is TransactionResult.NoWalletFound -> {
                SdkLog.d(TAG, CORR_SCAFFOLD) { "No MWA compatible wallet app found on device." }
            }
            is TransactionResult.Failure -> {
                SdkLog.d(TAG, CORR_SCAFFOLD) { "Error connecting to wallet: " + result.e.message }
            }
        }

        session.setResult(result)
        activity?.finish()
    }
}

@Composable
internal fun signTransaction(sender: ActivityResultSender, session: MwaSessionState) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        val connectionIdentity = ConnectionIdentity(
            identityUri = Uri.parse(session.getIdentityUri()),
            iconUri = Uri.parse(session.getIconUri()),
            identityName = session.getIdentityName(),
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (session.getCluster()) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }

        val token = session.getAuthToken()
        if (token != null) {
            // Unwrap SecretString at the clientlib seam (the adapter takes a raw String).
            walletAdapter.authToken = String(token.reveal(), Charsets.UTF_8)
        }

        val result = walletAdapter.transact(sender) {
            signTransactions(arrayOf(session.getPendingTransaction() ?: ByteArray(0)))
        }

        when (result) {
            is TransactionResult.Success -> {
                val signedTxBytes = result.successPayload?.signedPayloads?.first()
                signedTxBytes?.let {
                    session.setSignature(signedTxBytes)
                    session.setStatus(1)
                    SdkLog.d(TAG, CORR_SCAFFOLD) { "Signed memo transaction:" }
                }
            }
            is TransactionResult.NoWalletFound -> {
                session.setStatus(2)
                SdkLog.d(TAG, CORR_SCAFFOLD) { "No MWA compatible wallet app found on device." }
            }
            is TransactionResult.Failure -> {
                session.setStatus(2)
                SdkLog.d(TAG, CORR_SCAFFOLD) { "Error during transaction signing: " + result.e.message }
            }
        }

        session.setResult(result)
        activity?.finish()
    }
}

@Composable
internal fun signTextMessage(sender: ActivityResultSender, session: MwaSessionState) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        val connectionIdentity = ConnectionIdentity(
            identityUri = Uri.parse(session.getIdentityUri()),
            iconUri = Uri.parse(session.getIconUri()),
            identityName = session.getIdentityName(),
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (session.getCluster()) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }

        val token = session.getAuthToken()
        if (token != null) {
            // Unwrap SecretString at the clientlib seam (the adapter takes a raw String).
            walletAdapter.authToken = String(token.reveal(), Charsets.UTF_8)
        }

        val connectedKey = session.getConnectedKey()
        if (connectedKey == null) {
            session.setStatus(2)
            SdkLog.d(TAG, CORR_SCAFFOLD) { "No connected key available for signing." }
            activity?.finish()
            return@LaunchedEffect
        }
        val result = walletAdapter.transact(sender) {
            signMessagesDetached(arrayOf(session.getPendingTextMessage().toByteArray()), arrayOf(connectedKey))
        }

        when (result) {
            is TransactionResult.Success -> {
                val signedMessageBytes = result.successPayload?.messages?.first()?.signatures?.first()
                signedMessageBytes?.let {
                    session.setSignature(signedMessageBytes)
                    session.setStatus(1)
                }
            }
            is TransactionResult.NoWalletFound -> {
                session.setStatus(2)
                SdkLog.d(TAG, CORR_SCAFFOLD) { "No MWA compatible wallet app found on device." }
            }
            is TransactionResult.Failure -> {
                session.setStatus(2)
                SdkLog.d(TAG, CORR_SCAFFOLD) { "Error during message signing: " + result.e.message }
            }
        }

        session.setResult(result)
        activity?.finish()
    }
}
