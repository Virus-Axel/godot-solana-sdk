#[macro_use]
extern crate lazy_static;

pub mod utils;

use crate::utils::{
    pointer_to_key,
    pointers_to_keypair,
};

use std::{sync::Mutex, ffi::c_ulonglong};
use core::ffi::{
    c_char,
    c_int,
};

use solana_sdk::{
    instruction::AccountMeta,
    signer::keypair::Keypair,
    pubkey::Pubkey, account::Account,
};

pub struct AccountManager {
    pub accounts: Vec<AccountMeta>,
    pub signers: Vec<Keypair>,
    //pub keypair: Keypair,
    //pub program_id: Pubkey,
    //pub rpc_url: String,
    //pub signatures: Vec<Signature>,
}

lazy_static! {
    static ref ACCOUNT_MANAGER: Mutex<AccountManager> = Mutex::new(AccountManager {
        accounts: vec![],
        signers: vec![],
    });
}

#[no_mangle]
pub extern "C" fn append_account_w(account_pointer: *const c_char) -> c_int {
    // Convert the key
    let pub_key = match pointer_to_key(account_pointer){
        Ok(v) => v,
        Err(e) => return e,
    };

    let mut account_manager = match ACCOUNT_MANAGER.lock(){
        Ok(v) => v,
        Err(_) => return 1,
    };

    // Append account to vector
    account_manager.accounts.push(AccountMeta::new(pub_key, false));

    0
}

#[no_mangle]
pub extern "C" fn append_signer_w(priv_key_pointer: *const c_char, pub_key_pointer: *const c_char) -> c_int{
    // Convert the keys
    let keypair = match pointers_to_keypair(priv_key_pointer, pub_key_pointer){
        Ok(v) => v,
        Err(e) => return e,
    };

    let mut account_manager = match ACCOUNT_MANAGER.lock(){
        Ok(v) => v,
        Err(_) => return 1,
    };

    // Append signer to vector
    account_manager.signers.push(keypair);

    0
}

#[no_mangle]
pub extern "C" fn create_pubkey() -> *mut Pubkey{
    Box::into_raw(Box::new(Pubkey::new_unique()))
}

#[no_mangle]
pub extern "C" fn print_pubkey(key: *const Pubkey){
    unsafe{
        println!("{}\n", (*key).to_string());
    }
}

#[no_mangle]
pub extern "C" fn free_pubkey(key: *mut Pubkey){
    unsafe{
        drop(Box::from_raw(key));
    }
}

#[no_mangle]
pub extern "C" fn create_account(lamports: c_ulonglong, space: c_ulonglong, owner: *const Pubkey) -> *mut Account{
    let pubkey_ref = unsafe{*owner};
    Box::into_raw(Box::new(Account::new(lamports, space as usize, &pubkey_ref)))
}

#[no_mangle]
pub extern "C" fn free_account(account: *mut Account){
    unsafe{
        drop(Box::from_raw(account));
    }
}

#[no_mangle]
pub extern "C" fn create_account_meta(account_key: *const Pubkey, is_signer: bool, is_writeable: bool) -> *mut AccountMeta{
    let account_ref = unsafe{*account_key};
    if is_writeable{
        Box::into_raw(Box::new(AccountMeta::new(account_ref, is_signer)))
    }
    else{
        Box::into_raw(Box::new(AccountMeta::new_readonly(account_ref, is_signer)))
    }
}

#[no_mangle]
pub extern "C" fn free_account_meta(account_meta: *mut AccountMeta){
    unsafe{
        drop(Box::from_raw(account_meta));
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        
    }
}
