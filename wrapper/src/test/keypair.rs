use crate::modules::keypair::*;

use solana_sdk::signer::Signer;


const EMPTY_KEY: &str = "11111111111111111111111111111111";

#[test]
fn test_unique_keypair() {
    let keypair_ref = create_keypair();
    let keypair = unsafe{&(*keypair_ref)};
    assert_ne!(keypair.pubkey().to_string(), EMPTY_KEY.to_string());

    free_keypair(keypair_ref);
}

