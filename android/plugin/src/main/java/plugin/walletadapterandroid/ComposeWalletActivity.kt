package plugin.walletadapterandroid

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import com.solana.mobilewalletadapter.clientlib.ActivityResultSender

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
