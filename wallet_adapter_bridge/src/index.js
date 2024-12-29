import * as wallets from "@solana/wallet-adapter-wallets";
import { Transaction } from "@solana/web3.js";

Module.wallets = wallets;
Module.Transaction = Transaction;