package plugin.walletadapterandroid

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

import com.godotengine.godot_solana_sdk.mwa.session.MwaSessionState

class GDExtensionAndroidPlugin(godot: Godot) : GodotPlugin(godot) {

    companion object {
        val TAG = GDExtensionAndroidPlugin::class.java.simpleName

        // Single session-state instance shared between the plugin (Godot-facing API) and
        // ComposeWalletActivity (which reads it to dispatch the right @Composable). A formal DI
        // solution is out of scope for Story 1-2; Story 2-1 may introduce one.
        val sessionState: MwaSessionState = MwaSessionState()

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
        if (sessionState.getLastResult() is TransactionResult.Success<*>) {
            return
        }
        sessionState.setIdentity(uri, icon, name)
        sessionState.setCluster(cluster)
        sessionState.setAction(0)
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getConnectionStatus(): Int {
        val last = sessionState.getLastResult()
        if (last == null) {
            return 0
        } else if (last is TransactionResult.Success<*>) {
            return 1
        } else {
            return 2
        }
    }

    @UsedByGodot
    fun getSigningStatus(): Int = sessionState.getSigningStatus()

    @UsedByGodot
    fun getConnectedKey(): ByteArray? {
        sessionState.setAction(0)
        return sessionState.getConnectedKey() ?: ByteArray(0)
    }

    @UsedByGodot
    fun signTransaction(serializedTransaction: ByteArray) {
        sessionState.setAction(1)
        sessionState.setTransaction(serializedTransaction)
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun signTextMessage(textMessage: String) {
        sessionState.setAction(2)
        sessionState.setTextMessage(textMessage)
        godot.getActivity()?.let {
            val intent = Intent(it, ComposeWalletActivity::class.java)
            it.startActivity(intent)
        }
    }

    @UsedByGodot
    fun getMessageSignature(): ByteArray = sessionState.getLastSignature() ?: ByteArray(0)

    @UsedByGodot
    fun getLatestAction(): Int = sessionState.getPendingAction()

    @UsedByGodot
    fun clearState() {
        sessionState.clearStatus()
    }
}
