use gag::Gag;
use once_cell::sync::Lazy;
use solana_fee_calculator::FeeRateGovernor;
use solana_sdk::instruction::Instruction;
use solana_sdk::pubkey::Pubkey;
use solana_sdk::rent::Rent;
use solana_sdk::signature::{Keypair, Signer};
use solana_sdk::transaction::Transaction;
use solana_test_validator::{TestValidator, TestValidatorGenesis};
use std::sync::Mutex;

use solana_faucet::faucet::run_local_faucet_with_unique_port_for_tests;
use solana_streamer::socket::SocketAddrSpace;

static VALIDATOR: Lazy<Mutex<Option<TestValidator>>> = Lazy::new(|| Mutex::new(None));

#[unsafe(no_mangle)]
pub extern "C" fn start_validator(x: i32) -> i32 {
    let print_gag = Gag::stdout().unwrap();
    let mint_keypair = Keypair::new();
    let mint_pubkey = mint_keypair.pubkey();
    let faucet_addr = run_local_faucet_with_unique_port_for_tests(mint_keypair);
    let program_id = Pubkey::new_unique();
    let test_validator = TestValidatorGenesis::default()
        .rpc_port(8899)
        .fee_rate_governor(FeeRateGovernor::new(0, 0))
        .rent(Rent::with_lamports_per_byte(1))
        .faucet_addr(Some(faucet_addr))
        .start_with_mint_address_and_geyser_plugin_rpc(
            mint_pubkey,
            SocketAddrSpace::Unspecified,
            None,
            false,
        )
        .expect("validator start failed");

    *VALIDATOR.lock().unwrap() = Some(test_validator);

    x + 1
}
