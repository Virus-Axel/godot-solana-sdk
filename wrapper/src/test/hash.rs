use crate::modules::hash::*;
use bs58::encode;

const EMPTY_KEY: &str = "11111111111111111111111111111111";

#[test]
fn test_pointer() {
    let key_ref = create_unique_hash();
    let key = unsafe{*key_ref};
    assert_ne!(key.to_string(), EMPTY_KEY.to_string());

    free_hash(key_ref);
}

#[test]
fn test_creating_from_bytes() {
    let mut test_bytes = vec![1; 32];

    let key_ref = create_hash_from_array(test_bytes.as_mut_ptr());
    let encoded_sample = encode(&test_bytes).into_string();

    let key = unsafe{*key_ref};
    assert_eq!(key.to_string(), encoded_sample.to_string());

    free_hash(key_ref);
}

#[test]
fn test_pointer_free(){
    let key_ref = create_unique_hash();
    let real_key = unsafe{*key_ref};
    free_hash(key_ref);
    let fake_key = unsafe{*key_ref};

    assert_ne!(real_key.to_string(), fake_key.to_string());
}
