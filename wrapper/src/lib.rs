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
    c_uchar,
    c_int,
};

use solana_sdk::{
    instruction::{AccountMeta, Instruction},
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

#[no_mangle]
pub extern "C" fn create_instruction_with_bytes(program_id: *const Pubkey, data_array: *mut c_uchar, data_size: c_int, account_meta_array: *mut *mut AccountMeta, array_size: c_int) -> *mut Instruction{
    let pointer_array = unsafe {
        Vec::from_raw_parts(account_meta_array, array_size as usize, array_size as usize)
    };
    let program_id_ref = unsafe{*program_id};
    let mut account_metas = vec![];
    for i in 0..pointer_array.len(){
        let account_meta_ref = unsafe{(*pointer_array[i]).clone()};
        account_metas.push(account_meta_ref);
    };

    let data = unsafe {
        Vec::from_raw_parts(data_array, data_size as usize, data_size as usize)
    };

    let ret = Instruction::new_with_bytes(program_id_ref, &data, account_metas);

    Box::into_raw(Box::new(ret))
}

#[no_mangle]
pub extern "C" fn free_instruction(account_meta: *mut Instruction){
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
