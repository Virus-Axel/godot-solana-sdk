
package plugin.walletadapterandroid

import plugin.walletadapterandroid.connectWallet
import plugin.walletadapterandroid.myAction
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
    }
}
