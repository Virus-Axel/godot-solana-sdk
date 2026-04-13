
package plugin.walletadapterandroid

import plugin.walletadapterandroid.connectWallet
import plugin.walletadapterandroid.disconnectWallet
import plugin.walletadapterandroid.signMessageBytes
import plugin.walletadapterandroid.signAndSendTransaction
import plugin.walletadapterandroid.getCapabilities
import plugin.walletadapterandroid.myAction
import plugin.walletadapterandroid.myResult
import plugin.walletadapterandroid.myUserCancelled
import com.solana.mobilewalletadapter.clientlib.*

import android.os.Bundle
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender
import android.content.Intent
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import android.util.Log

class ComposeWalletActivity : ComponentActivity() {
    private var hasConnectedWallet = false

    override fun onCreate(savedInstanceState: Bundle?) {
        val uri = intent?.data

        super.onCreate(savedInstanceState)

        if (myAction == 0) {
            hasConnectedWallet = true
            val sender = ActivityResultSender(this)
            setContent {
                connectWallet(sender)
            }
        }
        else if (myAction == 1) {
            hasConnectedWallet = true
            val sender = ActivityResultSender(this)
            setContent {
                signTransaction(sender)
            }
        }
        else if (myAction == 2) {
            hasConnectedWallet = true
            val sender = ActivityResultSender(this)
            setContent {
                signTextMessage(sender)
            }
        }
        else if (myAction == 3) {
            hasConnectedWallet = true
            val sender = ActivityResultSender(this)
            setContent {
                disconnectWallet(sender)
            }
        }
        else if (myAction == 5) {
            hasConnectedWallet = true
            val sender = ActivityResultSender(this)
            setContent {
                signMessageBytes(sender)
            }
        }
        else if (myAction == 6) {
            hasConnectedWallet = true
            val sender = ActivityResultSender(this)
            setContent {
                signAndSendTransaction(sender)
            }
        }
        else if (myAction == 7) {
            hasConnectedWallet = true
            val sender = ActivityResultSender(this)
            setContent {
                getCapabilities(sender)
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        // If the activity is destroyed while a wallet operation is still in progress
        // (myResult == null), the user dismissed the wallet (pressed back, swiped away, etc.).
        // Signal this back to GDScript so it can exit the polling loop immediately.
        if (myResult == null) {
            myUserCancelled = true
            Log.d("ComposeWalletActivity", "Activity destroyed before wallet responded — marking as user-cancelled")
        }
    }
}
