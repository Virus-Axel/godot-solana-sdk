
package plugin.walletadapterandroid

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
        super.onCreate(savedInstanceState)

        val session = GDExtensionAndroidPlugin.sessionState
        when (session.getPendingAction()) {
            0 -> {
                hasConnectedWallet = true
                val sender = ActivityResultSender(this)
                setContent {
                    connectWallet(sender, session)
                }
            }
            1 -> {
                hasConnectedWallet = true
                val sender = ActivityResultSender(this)
                setContent {
                    signTransaction(sender, session)
                }
            }
            2 -> {
                hasConnectedWallet = true
                val sender = ActivityResultSender(this)
                setContent {
                    signTextMessage(sender, session)
                }
            }
        }
    }
}
