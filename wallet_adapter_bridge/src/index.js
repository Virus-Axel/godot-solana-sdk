import * as wallets from "@solana/wallet-adapter-wallets";
import { Transaction } from "@solana/web3.js";
import Backpack from "./backpack.js"

// Add all wallets from wallet-adapter and backpack.
Module.wallets = wallets;
Module.wallets['BackpackWalletAdapter'] = Backpack;

Module.Transaction = Transaction;