package plugin.walletadapterandroid

import androidx.compose.runtime.Composable
import com.solana.mobilewalletadapter.clientlib.*
import androidx.compose.runtime.LaunchedEffect

import kotlinx.coroutines.*
import kotlin.coroutines.CoroutineContext
import androidx.activity.ComponentActivity
import com.solana.mobilewalletadapter.common.signin.SignInWithSolana
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import androidx.compose.ui.platform.LocalContext

import kotlinx.coroutines.runBlocking
import android.app.Activity
import android.content.Intent

import android.net.Uri

import com.godotengine.godot_solana_sdk.mwa.session.MwaSessionState
import com.godotengine.godot_solana_sdk.mwa.util.SdkLog

private const val TAG = "MWA"
private const val CORR_SCAFFOLD = "scaffold"

@Composable
fun connectWallet(sender: ActivityResultSender, session: MwaSessionState) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        val connectionIdentity = ConnectionIdentity(
            identityUri = Uri.parse(session.getIdentityUri()),
            iconUri = Uri.parse(session.getIconUri()),
            identityName = session.getIdentityName()
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
                session.setAuthToken(result.authResult.authToken)
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
fun signTransaction(sender: ActivityResultSender, session: MwaSessionState) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {

        val connectionIdentity = ConnectionIdentity(
            identityUri = Uri.parse(session.getIdentityUri()),
            iconUri = Uri.parse(session.getIconUri()),
            identityName = session.getIdentityName()
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
            walletAdapter.authToken = token
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
fun signTextMessage(sender: ActivityResultSender, session: MwaSessionState) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {

        val connectionIdentity = ConnectionIdentity(
            identityUri = Uri.parse(session.getIdentityUri()),
            iconUri = Uri.parse(session.getIconUri()),
            identityName = session.getIdentityName()
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
            walletAdapter.authToken = token
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
