package plugin.walletadapterandroid

import plugin.walletadapterandroid.myResult
import plugin.walletadapterandroid.myAction
import plugin.walletadapterandroid.myStoredTransaction
import plugin.walletadapterandroid.myStoredTextMessage
import plugin.walletadapterandroid.myMessageSignature
import plugin.walletadapterandroid.myMessageSigningStatus
import plugin.walletadapterandroid.myConnectedKey
import plugin.walletadapterandroid.myConnectCluster
import plugin.walletadapterandroid.myIdentityName
import plugin.walletadapterandroid.myIdentityUri
import plugin.walletadapterandroid.myIconUri
import plugin.walletadapterandroid.myCapabilitiesResult
import plugin.walletadapterandroid.myCapabilitiesStatus
import plugin.walletadapterandroid.mySignAndSendResult
import plugin.walletadapterandroid.mySignAndSendStatus
import plugin.walletadapterandroid.mySiwsDomain
import plugin.walletadapterandroid.mySiwsStatement
import plugin.walletadapterandroid.mySiwsSignature
import plugin.walletadapterandroid.mySiwsSignedMessage
import plugin.walletadapterandroid.mySiwsPublicKey
import plugin.walletadapterandroid.mySiwsAccountLabel
import plugin.walletadapterandroid.mySiwsAccountChains
import plugin.walletadapterandroid.mySiwsAccountFeatures
import plugin.walletadapterandroid.mySiwsStatus
import plugin.walletadapterandroid.authToken
import com.solana.mobilewalletadapter.clientlib.protocol.MobileWalletAdapterClient.AuthorizationResult

import android.util.Log
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.UsedByGodot
import com.solana.mobilewalletadapter.clientlib.*

import com.solana.mobilewalletadapter.clientlib.ActivityResultSender

import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import android.os.Bundle
import android.content.Intent
import android.net.Uri

class GDExtensionAndroidPlugin(godot: Godot): GodotPlugin(godot) {

    companion object {
        val TAG = GDExtensionAndroidPlugin::class.java.simpleName

        init {
            try {
                Log.v(TAG, "Loading ${BuildConfig.GODOT_PLUGIN_NAME} library")
                System.loadLibrary(BuildConfig.GODOT_PLUGIN_NAME)
            } catch (e: UnsatisfiedLinkError) {
                Log.e(TAG, "Unable to load ${BuildConfig.GODOT_PLUGIN_NAME} shared library")
            }
        }
    }

    override fun getPluginName() = BuildConfig.GODOT_PLUGIN_NAME

    override fun getPluginGDExtensionLibrariesPaths() = setOf("res://addons/${BuildConfig.GODOT_PLUGIN_NAME}/plugin.gdextension")

    @UsedByGodot
    fun connectWallet(cluster: Int, uri: String, icon: String, name: String) {
        Log.i("godot", "[KotlinPlugin] connectWallet | START myResult=${myResult?.javaClass?.simpleName} myConnectedKey_len=${myConnectedKey?.size ?: -1} authToken_len=${authToken?.length ?: -1} cluster=$cluster name=$name")
        if (myResult is TransactionResult.Success) {
            Log.i("godot", "[KotlinPlugin] connectWallet | CACHED — myResult is Success, returning immediately WITHOUT opening OS picker")
            return
        }
        Log.i("godot", "[KotlinPlugin] connectWallet | FRESH — myResult is null/not Success, opening ComposeWalletActivity (OS picker)")
        myIdentityUri = Uri.parse(uri)
        myIconUri = Uri.parse(icon)
        myIdentityName = name
        myConnectCluster = cluster
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
            Log.i("godot", "[KotlinPlugin] connectWallet | ComposeWalletActivity started — OS picker should open")
        }
    }

    @UsedByGodot
    fun getConnectionStatus(): Int {
        val myLocalResult = myResult
        return if (myLocalResult == null) 0 else if (myLocalResult is TransactionResult.Success) 1 else 2
    }

    @UsedByGodot
    fun getSigningStatus(): Int {
        return myMessageSigningStatus
    }

    @UsedByGodot
    fun getConnectedKey(): ByteArray? {
        myAction = 0
        return myConnectedKey ?: ByteArray(0)
    }

    @UsedByGodot
    fun signTransaction(serializedTransaction: ByteArray) {
        myAction = 1
        myStoredTransaction = serializedTransaction
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun signTextMessage(textMessage: String) {
        myAction = 2
        myStoredTextMessage = textMessage
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getMessageSignature(): ByteArray {
        return myMessageSignature ?: ByteArray(0)
    }

    @UsedByGodot
    fun getLatestAction(): Int {
        return myAction
    }

    @UsedByGodot
    fun getCapabilitiesWallet() {
        myAction = 3
        myCapabilitiesStatus = 0
        myCapabilitiesResult = ""
        Log.i("godot", "[KotlinPlugin] getCapabilitiesWallet | START — opening wallet for capabilities query")
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getCapabilitiesStatus(): Int {
        return myCapabilitiesStatus
    }

    @UsedByGodot
    fun getCapabilitiesResult(): String {
        return myCapabilitiesResult
    }

    @UsedByGodot
    fun signAndSendTransaction(serializedTransaction: ByteArray) {
        myAction = 4
        myStoredTransaction = serializedTransaction
        mySignAndSendStatus = 0
        mySignAndSendResult = ""
        Log.i("godot", "[KotlinPlugin] signAndSendTransaction | START tx_len=${serializedTransaction.size} — opening wallet for sign & send")
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getSignAndSendStatus(): Int {
        return mySignAndSendStatus
    }

    @UsedByGodot
    fun getSignAndSendResult(): String {
        return mySignAndSendResult
    }

    @UsedByGodot
    fun setIdentity(cluster: Int, uri: String, icon: String, name: String) {
        myConnectCluster = cluster
        myIdentityUri = Uri.parse(uri)
        myIconUri = Uri.parse(icon)
        myIdentityName = name
        Log.i("godot", "[KotlinPlugin] setIdentity | cluster=$cluster uri=$uri icon=$icon name=$name")
    }

    @UsedByGodot
    fun clearState() {
        Log.i("godot", "[KotlinPlugin] clearState | clearing status flags — keeping myResult/myConnectedKey/authToken for connection reuse")
        myMessageSigningStatus = 0
        mySignAndSendStatus = 0
        mySignAndSendResult = ""
        mySiwsSignature = null
        mySiwsSignedMessage = null
        mySiwsPublicKey = null
        mySiwsAccountLabel = null
        mySiwsAccountChains = ""
        mySiwsAccountFeatures = ""
        mySiwsStatus = 0
    }

    @UsedByGodot
    fun clearStateFullReset() {
        Log.i("godot", "[KotlinPlugin] clearStateFullReset | clearing myResult (was ${myResult?.javaClass?.simpleName}) + all status flags — next connectWallet will open fresh OS picker")
        myResult = null
        myMessageSigningStatus = 0
        mySignAndSendStatus = 0
        mySignAndSendResult = ""
        mySiwsSignature = null
        mySiwsSignedMessage = null
        mySiwsPublicKey = null
        mySiwsAccountLabel = null
        mySiwsAccountChains = ""
        mySiwsAccountFeatures = ""
        mySiwsStatus = 0
    }

    // ─── MWA 2.0: SIWS authorize ─────────────────────────────────────────────

    @UsedByGodot
    fun connectWalletSiws(cluster: Int, uri: String, icon: String, name: String, domain: String, statement: String) {
        Log.i("godot", "[KotlinPlugin] connectWalletSiws | cluster=$cluster domain=$domain statement=$statement")
        myConnectCluster = cluster
        myIdentityUri = Uri.parse(uri)
        myIconUri = Uri.parse(icon)
        myIdentityName = name
        mySiwsDomain = domain
        mySiwsStatement = statement
        myAction = 5
        mySiwsSignature = null
        mySiwsSignedMessage = null
        mySiwsPublicKey = null
        mySiwsAccountLabel = null
        mySiwsAccountChains = ""
        mySiwsAccountFeatures = ""
        mySiwsStatus = 0
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getSiwsStatus(): Int {
        return mySiwsStatus
    }

    @UsedByGodot
    fun getSiwsSignature(): ByteArray {
        return mySiwsSignature ?: ByteArray(0)
    }

    @UsedByGodot
    fun getSiwsSignedMessage(): ByteArray {
        return mySiwsSignedMessage ?: ByteArray(0)
    }

    @UsedByGodot
    fun getSiwsPublicKey(): ByteArray {
        return mySiwsPublicKey ?: ByteArray(0)
    }

    @UsedByGodot
    fun getSiwsAccountLabel(): String {
        return mySiwsAccountLabel ?: ""
    }

    @UsedByGodot
    fun getSiwsAccountChains(): String {
        return mySiwsAccountChains
    }

    @UsedByGodot
    fun getSiwsAccountFeatures(): String {
        return mySiwsAccountFeatures
    }

    // ─── Pubkey helpers ───────────────────────────────────────────────────────

    @UsedByGodot
    fun getConnectedKeyBase58(): String {
        val key = myConnectedKey
        if (key == null || key.isEmpty()) return ""
        return base58Encode(key)
    }

    private fun base58Encode(input: ByteArray): String {
        val ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"
        if (input.isEmpty()) return ""
        var zeros = 0
        for (b in input) { if (b.toInt() == 0) zeros++ else break }
        var value = java.math.BigInteger(1, input)
        val sb = StringBuilder()
        val fifty8 = java.math.BigInteger.valueOf(58)
        while (value > java.math.BigInteger.ZERO) {
            val divrem = value.divideAndRemainder(fifty8)
            value = divrem[0]
            sb.append(ALPHABET[divrem[1].toInt()])
        }
        repeat(zeros) { sb.append('1') }
        return sb.reverse().toString()
    }

}
