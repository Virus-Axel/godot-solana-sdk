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
var myStoredTextMessage: String = ""
var myMessageSignature: ByteArray? = null
var myMessageSigningStatus: Int = 0
var myConnectedKey: ByteArray? = null
var myConnectCluster: Int = 0
var myIdentityUri: Uri = Uri.EMPTY
var myIconUri: Uri = Uri.EMPTY
var myIdentityName: String = ""

var authToken: String? = null
var myCapabilitiesResult: String = ""
var myCapabilitiesStatus: Int = 0
var mySignAndSendResult: String = ""
var mySignAndSendStatus: Int = 0  // 0=pending, 1=success, 2=failed

// MWA 2.0: SIWS (Sign In With Solana) authorize state
var mySiwsDomain: String = ""
var mySiwsStatement: String = ""
var mySiwsSignature: ByteArray? = null
var mySiwsSignedMessage: ByteArray? = null
var mySiwsPublicKey: ByteArray? = null
var mySiwsAccountLabel: String? = null
var mySiwsAccountChains: String = ""
var mySiwsAccountFeatures: String = ""
var mySiwsStatus: Int = 0   // 0=pending, 1=success, 2=failure

@Composable
fun connectWallet(sender: ActivityResultSender) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        val connectionIdentity = ConnectionIdentity(
            identityUri = myIdentityUri,
            iconUri = myIconUri,
            identityName = myIdentityName
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (myConnectCluster) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }
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
            identityUri = myIdentityUri,
            iconUri = myIconUri,
            identityName = myIdentityName
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (myConnectCluster) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }

        if(authToken != null){
            walletAdapter.authToken = authToken
        }

        val result = walletAdapter.transact(sender){
            signTransactions(arrayOf(myStoredTransaction ?: ByteArray(0)))
        }

        when (result) {
            is TransactionResult.Success -> {
                authToken = result.authResult.authToken
                val signedTxBytes = result.successPayload?.signedPayloads?.first()
                signedTxBytes?.let {
                    myMessageSignature = signedTxBytes
                    myMessageSigningStatus = 1
                }
            }
            is TransactionResult.NoWalletFound -> {
                myMessageSigningStatus = 2
            }
            is TransactionResult.Failure -> {
                myMessageSigningStatus = 2
            }
        }

        myResult = result
        activity?.finish()
    }
}

@Composable
fun signTextMessage(sender: ActivityResultSender) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        
        val connectionIdentity = ConnectionIdentity(
            identityUri = myIdentityUri,
            iconUri = myIconUri,
            identityName = myIdentityName
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (myConnectCluster) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }

        if(authToken != null){
            walletAdapter.authToken = authToken
        }

        if(myConnectedKey == null){
            myMessageSigningStatus = 2
            println("No connected key available for signing.")
            activity?.finish()
            return@LaunchedEffect
            
        }
        val result = walletAdapter.transact(sender){
            signMessagesDetached(arrayOf(myStoredTextMessage.toByteArray()), arrayOf(myConnectedKey!!))
        }

        when (result) {
            is TransactionResult.Success -> {
                authToken = result.authResult.authToken
                val signedMessageBytes = result.successPayload?.messages?.first()?.signatures?.first()
                signedMessageBytes?.let {
                    myMessageSignature = signedMessageBytes
                    myMessageSigningStatus = 1
                }
            }
            is TransactionResult.NoWalletFound -> {
                myMessageSigningStatus = 2
            }
            is TransactionResult.Failure -> {
                myMessageSigningStatus = 2
            }
        }

        myResult = result
        activity?.finish()
    }
}

@Composable
fun getWalletCapabilities(sender: ActivityResultSender) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        Log.i("godot", "[KotlinPlugin] getWalletCapabilities | START authToken_len=${authToken?.length ?: -1} cluster=$myConnectCluster identity=$myIdentityName")

        val connectionIdentity = ConnectionIdentity(
            identityUri = myIdentityUri,
            iconUri = myIconUri,
            identityName = myIdentityName
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (myConnectCluster) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }

        if (authToken != null) {
            walletAdapter.authToken = authToken
            Log.i("godot", "[KotlinPlugin] getWalletCapabilities | using cached authToken (len=${authToken?.length})")
        } else {
            Log.i("godot", "[KotlinPlugin] getWalletCapabilities | no authToken — wallet will need to authorize")
        }

        Log.i("godot", "[KotlinPlugin] getWalletCapabilities | calling walletAdapter.transact { getCapabilities() }")
        val result = walletAdapter.transact(sender) {
            getCapabilities()
        }

        Log.i("godot", "[KotlinPlugin] getWalletCapabilities | transact returned result_type=${result?.javaClass?.simpleName}")

        when (result) {
            is TransactionResult.Success -> {
                val caps = result.successPayload
                Log.i("godot", "[KotlinPlugin] getWalletCapabilities | SUCCESS payload_null=${caps == null}")
                if (caps != null) {
                    Log.i("godot", "[KotlinPlugin] getWalletCapabilities | maxTransactions=${caps.maxTransactionsPerSigningRequest} maxMessages=${caps.maxMessagesPerSigningRequest} supportsCloneAuth=${caps.supportsCloneAuthorization} supportsSignAndSend=${caps.supportsSignAndSendTransactions}")
                    Log.i("godot", "[KotlinPlugin] getWalletCapabilities | supportedVersions=${caps.supportedTransactionVersions?.joinToString(";") ?: "null"} optionalFeatures=${caps.supportedOptionalFeatures?.joinToString(";") ?: "null"}")
                    myCapabilitiesResult = "maxTransactions=${caps.maxTransactionsPerSigningRequest}" +
                        ",maxMessages=${caps.maxMessagesPerSigningRequest}" +
                        ",supportsCloneAuth=${caps.supportsCloneAuthorization}" +
                        ",supportsSignAndSend=${caps.supportsSignAndSendTransactions}" +
                        ",supportedVersions=${caps.supportedTransactionVersions?.joinToString(";") ?: ""}" +
                        ",optionalFeatures=${caps.supportedOptionalFeatures?.joinToString(";") ?: ""}"
                    myCapabilitiesStatus = 1
                    Log.i("godot", "[KotlinPlugin] getWalletCapabilities | STORED result='$myCapabilitiesResult' status=1")
                } else {
                    myCapabilitiesResult = ""
                    myCapabilitiesStatus = 2
                    Log.i("godot", "[KotlinPlugin] getWalletCapabilities | FAIL payload was null despite Success result")
                }
            }
            is TransactionResult.NoWalletFound -> {
                myCapabilitiesStatus = 2
                Log.i("godot", "[KotlinPlugin] getWalletCapabilities | FAIL NoWalletFound — no MWA compatible wallet app on device")
            }
            is TransactionResult.Failure -> {
                myCapabilitiesStatus = 2
                Log.i("godot", "[KotlinPlugin] getWalletCapabilities | FAIL error=${result.e.message} exception=${result.e.javaClass?.simpleName}")
            }
        }

        Log.i("godot", "[KotlinPlugin] getWalletCapabilities | DONE status=$myCapabilitiesStatus finishing activity")
        activity?.finish()
    }
}

@Composable
fun signAndSendTransaction(sender: ActivityResultSender) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        Log.i("godot", "[KotlinPlugin] signAndSendTransaction | START authToken_len=${authToken?.length ?: -1} storedTx_len=${myStoredTransaction?.size ?: -1}")

        val connectionIdentity = ConnectionIdentity(
            identityUri = myIdentityUri,
            iconUri = myIconUri,
            identityName = myIdentityName
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (myConnectCluster) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }

        if (authToken != null) {
            walletAdapter.authToken = authToken
            Log.i("godot", "[KotlinPlugin] signAndSendTransaction | using cached authToken (len=${authToken?.length})")
        }

        Log.i("godot", "[KotlinPlugin] signAndSendTransaction | calling walletAdapter.transact { signTransactions(...) }")
        val result = walletAdapter.transact(sender) {
            signTransactions(arrayOf(myStoredTransaction ?: ByteArray(0)))
        }

        Log.i("godot", "[KotlinPlugin] signAndSendTransaction | transact returned result_type=${result?.javaClass?.simpleName}")

        when (result) {
            is TransactionResult.Success -> {
                authToken = result.authResult.authToken
                val signedTxBytes = result.successPayload?.signedPayloads?.first()
                if (signedTxBytes != null) {
                    mySignAndSendResult = android.util.Base64.encodeToString(signedTxBytes, android.util.Base64.NO_WRAP)
                    mySignAndSendStatus = 1
                    Log.i("godot", "[KotlinPlugin] signAndSendTransaction | SIGNED base64_len=${mySignAndSendResult.length} tx_bytes=${signedTxBytes.size}")
                } else {
                    mySignAndSendStatus = 2
                    Log.i("godot", "[KotlinPlugin] signAndSendTransaction | FAIL signedPayloads was null")
                }
            }
            is TransactionResult.NoWalletFound -> {
                mySignAndSendStatus = 2
                Log.i("godot", "[KotlinPlugin] signAndSendTransaction | FAIL NoWalletFound")
            }
            is TransactionResult.Failure -> {
                mySignAndSendStatus = 2
                Log.i("godot", "[KotlinPlugin] signAndSendTransaction | FAIL error=${result.e.message}")
            }
        }

        myResult = result
        Log.i("godot", "[KotlinPlugin] signAndSendTransaction | DONE status=$mySignAndSendStatus finishing activity")
        activity?.finish()
    }
}

// MWA 2.0: SIWS authorize — one-shot connect + prove ownership via signIn().
@Composable
fun connectWalletSiws(sender: ActivityResultSender) {
    val activity = LocalContext.current as? Activity
    LaunchedEffect(Unit) {
        Log.i("godot", "[connectWalletSiws] ENTRY | domain=$mySiwsDomain statement=$mySiwsStatement cluster=$myConnectCluster")

        val connectionIdentity = ConnectionIdentity(
            identityUri = myIdentityUri,
            iconUri = myIconUri,
            identityName = myIdentityName
        )

        val walletAdapter = MobileWalletAdapter(connectionIdentity)
        when (myConnectCluster) {
            0 -> walletAdapter.blockchain = Solana.Devnet
            1 -> walletAdapter.blockchain = Solana.Mainnet
            2 -> walletAdapter.blockchain = Solana.Testnet
            else -> walletAdapter.blockchain = Solana.Devnet
        }

        val payload = SignInWithSolana.Payload(mySiwsDomain, mySiwsStatement)

        try {
            val result = walletAdapter.signIn(sender, payload)

            when (result) {
                is TransactionResult.Success -> {
                    authToken = result.authResult.authToken
                    myConnectedKey = result.authResult.publicKey

                    val signInResult = result.authResult.signInResult ?: result.successPayload
                    mySiwsSignature = signInResult?.signature
                    mySiwsSignedMessage = signInResult?.signedMessage
                    mySiwsPublicKey = signInResult?.publicKey

                    val firstAccount = result.authResult.accounts?.firstOrNull()
                    mySiwsAccountLabel = firstAccount?.accountLabel
                    mySiwsAccountChains = firstAccount?.chains?.joinToString(",") ?: ""
                    mySiwsAccountFeatures = firstAccount?.features?.joinToString(",") ?: ""

                    mySiwsStatus = 1
                    Log.i("godot", "[connectWalletSiws] SUCCESS | authToken_len=${result.authResult.authToken.length} pubkey_size=${result.authResult.publicKey?.size ?: 0} label=${firstAccount?.accountLabel}")
                }
                is TransactionResult.NoWalletFound -> {
                    Log.i("godot", "[connectWalletSiws] NO_WALLET_FOUND")
                    mySiwsStatus = 2
                }
                is TransactionResult.Failure -> {
                    Log.i("godot", "[connectWalletSiws] FAILURE | error=${result.e.message}")
                    mySiwsStatus = 2
                }
            }
            myResult = result
        } catch (e: Exception) {
            Log.i("godot", "[connectWalletSiws] EXCEPTION | error=${e.message} class=${e.javaClass.simpleName}")
            mySiwsStatus = 2
        }
        activity?.finish()
    }
}