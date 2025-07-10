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

import android.util.Log

import android.net.Uri

var myResult: TransactionResult<*>? = null
var myAction: Int = 0
var myStoredTransaction: ByteArray? = null
var myMessageSignature: ByteArray? = null
var myMessageSigningStatus: Int = 0
var myConnectedKey: ByteArray? = null

var authToken: String? = null

@Composable
fun connectWallet(sender: ActivityResultSender) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        
        val connectionIdentity = ConnectionIdentity(
            identityUri = Uri.parse("https://solana.com"),
            iconUri = Uri.parse("favicon.ico"),
            identityName = "Solana"
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        val result = walletAdapter.connect(sender)

        when (result) {
            is TransactionResult.Success -> {
                // On success, an `AuthorizationResult` type is returned.
                authToken = result.authResult.authToken
                myConnectedKey = result.authResult.publicKey
            }
            is TransactionResult.NoWalletFound -> {
                println("No MWA compatible wallet app found on device.")
            }
            is TransactionResult.Failure -> {
                println("Error connecting to wallet: " + result.e.message)
            }
        }

        myResult = result
        activity?.finish()
    }
}

@Composable
fun signTransaction(sender: ActivityResultSender) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        
        val connectionIdentity = ConnectionIdentity(
            identityUri = Uri.parse("https://solana.com"),
            iconUri = Uri.parse("favicon.ico"),
            identityName = "Solana"
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)

        if(authToken != null){
            walletAdapter.authToken = authToken
        }

        val result = walletAdapter.transact(sender){
            signTransactions(arrayOf(myStoredTransaction ?: ByteArray(0)))
        }

        when (result) {
            is TransactionResult.Success -> {
                val signedTxBytes = result.successPayload?.signedPayloads?.first()
                signedTxBytes?.let {
                    myMessageSignature = signedTxBytes
                    myMessageSigningStatus = 1
                    println("Signed memo transaction:")
                }
            }
            is TransactionResult.NoWalletFound -> {
                myMessageSigningStatus = 2
                println("No MWA compatible wallet app found on device.")
            }
            is TransactionResult.Failure -> {
                myMessageSigningStatus = 2
                println("Error during transaction signing: " + result.e.message)
            }
        }

        myResult = result
        activity?.finish()
    }
}