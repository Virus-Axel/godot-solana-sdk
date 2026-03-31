package plugin.walletadapterandroid

import plugin.walletadapterandroid.myResult
import plugin.walletadapterandroid.myAction
import plugin.walletadapterandroid.myStoredTransaction
import plugin.walletadapterandroid.myStoredTextMessage
import plugin.walletadapterandroid.myStoredDetachedMessageBytes
import plugin.walletadapterandroid.myMessageSignature
import plugin.walletadapterandroid.myMessageSigningStatus
import plugin.walletadapterandroid.myConnectedKey
import plugin.walletadapterandroid.myCapabilitiesResultJson
import plugin.walletadapterandroid.myWalletUriBase
import plugin.walletadapterandroid.myConnectCluster
import plugin.walletadapterandroid.myIdentityName
import plugin.walletadapterandroid.myIdentityUri
import plugin.walletadapterandroid.myIconUri
import plugin.walletadapterandroid.authToken
import plugin.walletadapterandroid.myUserCancelled

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

    override fun getPluginMethods(): List<String> {
        return listOf(
            "connectWallet",
            "setAuthToken",
            "clearAuthToken",
            "getAuthToken",
            "deauthorizeWallet",
            "getCapabilitiesWallet",
            "getCapabilitiesResultJson",
            "getWalletUriBase",
            "getConnectionStatus",
            "getSigningStatus",
            "getConnectedKey",
            "signTransaction",
            "signAndSendTransaction",
            "signTextMessage",
            "signMessageBytes",
            "getMessageSignature",
            "getLatestAction",
            "clearState",
            "setConnectedKey"
        )
    }

    @UsedByGodot
    fun connectWallet(cluster: Int, uri: String, icon: String, name: String) {
        if (!authToken.isNullOrEmpty()) {
            myResult = TransactionResult.Success(true)
            return
        }

        // Always start a fresh connect operation; prior results must not short-circuit new sessions.
        myAction = 0
        myResult = null
        myConnectedKey = null
        myMessageSigningStatus = 0

        myIdentityUri = Uri.parse(uri);
        myIconUri = Uri.parse(icon);
        myIdentityName = name;
        myConnectCluster = cluster
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun setAuthToken(token: String) {
        authToken = if (token.isNotEmpty()) token else null
    }

    @UsedByGodot
    fun clearAuthToken() {
        authToken = null
    }

    @UsedByGodot
    fun getAuthToken(): String {
        return authToken ?: ""
    }

    @UsedByGodot
    fun deauthorizeWallet(token: String) {
        // Attempt to revoke this auth token. If it's empty, revoke whatever token the wallet holds.
        myAction = 3
        myResult = null
        myConnectedKey = null
        authToken = if (token.isNotEmpty()) token else null

        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getCapabilitiesWallet() {
        // Use the transact flow without relying on wallet authorization to populate payload.
        myAction = 7
        myResult = null
        myConnectedKey = null
        myMessageSigningStatus = 0
        myCapabilitiesResultJson = ""

        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getCapabilitiesResultJson(): String {
        return myCapabilitiesResultJson
    }

    @UsedByGodot
    fun getWalletUriBase(): String {
        return myWalletUriBase
    }

    @UsedByGodot
    fun getConnectionStatus(): Int{
        // Check for user-cancelled first: activity was destroyed before wallet responded.
        if (myUserCancelled) {
            return 3
        }
        val myLocalResult = myResult
        if (myLocalResult == null) {
            return 0
        }
        else if(myLocalResult is TransactionResult.Success) {
            return 1
        }
        else{
            return 2
        }
    }

    @UsedByGodot
    fun getSigningStatus(): Int{
        return myMessageSigningStatus
    }

    @UsedByGodot
    fun getConnectedKey(): ByteArray?{
        return myConnectedKey?: ByteArray(0)
    }

    @UsedByGodot
    fun setConnectedKey(key: ByteArray) {
        myConnectedKey = key
    }

    @UsedByGodot
    fun signTransaction(serializedTransaction: ByteArray){
        myAction = 1
        myMessageSigningStatus = 0
        myMessageSignature = null
        myResult = null
        myUserCancelled = false
        myStoredTransaction = serializedTransaction
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun signAndSendTransaction(serializedTransaction: ByteArray){
        myAction = 6
        myMessageSigningStatus = 0
        myMessageSignature = null
        myResult = null
        myUserCancelled = false
        myStoredTransaction = serializedTransaction
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }
    
    @UsedByGodot
    fun signTextMessage(textMessage: String){
        myAction = 2
        myMessageSigningStatus = 0
        myMessageSignature = null
        myResult = null
        myUserCancelled = false
        myStoredTextMessage = textMessage
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun signMessageBytes(messageBytes: ByteArray) {
        myAction = 5
        myMessageSigningStatus = 0
        myMessageSignature = null
        myResult = null
        myUserCancelled = false
        myStoredDetachedMessageBytes = messageBytes
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getMessageSignature(): ByteArray {
        return myMessageSignature?: ByteArray(0)
    }

    @UsedByGodot
    fun getLatestAction(): Int {
        return myAction
    }

    @UsedByGodot
    fun clearState() {
        myMessageSigningStatus = 0
        myUserCancelled = false
        myResult = null
    }
}
