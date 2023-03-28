use core::ffi::{
    c_char,
    c_int,
    CStr,
};

use solana_sdk::{
    pubkey::Pubkey,
    signer::keypair::Keypair,
};

pub fn pointer_to_key(pointer: *const c_char) -> Result<Pubkey, c_int>{
    let key_string = unsafe { CStr::from_ptr(pointer).to_string_lossy().into_owned() };

    let decoded_key = match bs58::decode(&key_string).into_vec() {
        Ok(v) => v,
        Err(_) => return Err(-1),
    };

    let key = match Pubkey::try_from(decoded_key.as_slice()){
        Ok(v) => v,
        Err(_) => return Err(-1)
    };

    Ok(key)
}

pub fn pointers_to_keypair(priv_key_pointer: *const c_char, pub_key_pointer: *const c_char) -> Result<Keypair, c_int>{
    let priv_key_string = unsafe { CStr::from_ptr(priv_key_pointer).to_string_lossy().into_owned() };
    let pub_key_string = unsafe { CStr::from_ptr(pub_key_pointer).to_string_lossy().into_owned() };

    // Set private bytes
    let mut keypair_bytes = match bs58::decode(&priv_key_string).into_vec() {
        Ok(v) => v,
        Err(_) => return Err(-1),
    };

    let mut pubkey_bytes = match bs58::decode(&pub_key_string).into_vec() {
        Ok(v) => v,
        Err(_) => return Err(-1),
    };

    // Append public bytes
    keypair_bytes.append(&mut pubkey_bytes);

    let keypair = match Keypair::from_bytes(&keypair_bytes){
        Ok(v) => v,
        Err(_) => return Err(-1),
    };

    Ok(keypair)
}