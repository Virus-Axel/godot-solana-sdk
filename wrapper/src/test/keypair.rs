use crate::modules::keypair::*;

use solana_sdk::{signer::Signer, signature::Keypair};

const EMPTY_KEY: &str = "11111111111111111111111111111111";

#[test]
fn test_unique_keypair() {
    let keypair_ref = create_keypair();
    let keypair = unsafe{&(*keypair_ref)};
    assert_ne!(keypair.pubkey().to_string(), EMPTY_KEY.to_string());

    free_keypair(keypair_ref);
}

#[test]
fn test_keypair_from_bytes() {
    let mut test_bytes = vec![1; 64];

    let key_ref = create_keypair_from_bytes(test_bytes.as_mut_ptr());
    let compare_key = Keypair::from_bytes(&test_bytes).unwrap();

    let key = unsafe{&(*key_ref)};
    assert_eq!(key.pubkey().to_string(), compare_key.pubkey().to_string());

    free_keypair(key_ref);
}

