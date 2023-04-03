#[macro_use]
extern crate lazy_static;

pub mod utils;

use crate::utils::{
    pointer_to_key,
    pointers_to_keypair,
};

use std::{sync::Mutex, ffi::c_ulonglong, ptr,};
use core::ffi::{
    c_char,
    c_uchar,
    c_int,
};

use solana_sdk::{
    instruction::{AccountMeta, Instruction},
    signer::keypair::Keypair,
    pubkey::Pubkey, account::Account, transaction::Transaction, hash::Hash,
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
pub extern "C" fn create_unique_pubkey() -> *mut Pubkey{
    Box::into_raw(Box::new(Pubkey::new_unique()))
}

#[no_mangle]
pub extern "C" fn create_pubkey_from_array(bytes: *mut u8) -> *mut Pubkey{
    let arr = unsafe{Vec::from_raw_parts(bytes, 32, 32)};
    Box::into_raw(Box::new(Pubkey::new_from_array(arr.try_into().unwrap())))
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
pub extern "C" fn create_unique_hash() -> *mut Hash{
    Box::into_raw(Box::new(Hash::new_unique()))
}

#[no_mangle]
pub extern "C" fn create_hash_from_array(bytes: *mut u8) -> *mut Hash{
    let arr = unsafe{Vec::from_raw_parts(bytes, 32, 32)};
    Box::into_raw(Box::new(Hash::new_from_array(arr.try_into().unwrap())))
}

#[no_mangle]
pub extern "C" fn free_hash(key: *mut Hash){
    unsafe{
        drop(Box::from_raw(key));
    }
}

#[no_mangle]
pub extern "C" fn create_keypair() -> *mut Keypair{
    Box::into_raw(Box::new(Keypair::new()))
}

#[no_mangle]
pub extern "C" fn free_keypair(key: *mut Keypair){
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

#[no_mangle]
pub extern "C" fn create_transaction_signed_with_payer(instruction_array: *mut *mut Instruction, array_size: c_int, payer: *const Pubkey, signers_array: *mut *mut Keypair, signers_array_size: c_int, latest_blockhash: *const Pubkey) -> *const Transaction{
    let instruction_pointer_array = unsafe {
        Vec::from_raw_parts(instruction_array, array_size as usize, array_size as usize)
    };
    let signer_pointer_array = unsafe {
        Vec::from_raw_parts(signers_array, signers_array_size as usize, signers_array_size as usize)
    };

    let payer_ref = unsafe{*payer};
    let latest_blockhash_ref = unsafe{*latest_blockhash};

    let mut instructions = vec![];
    for i in 0..instruction_pointer_array.len(){
        let instruction_ref = unsafe{(*instruction_pointer_array[i]).clone()};
        instructions.push(instruction_ref);
    };

    let mut signers = vec![];
    for i in 0..signer_pointer_array.len(){
        let signer_ref = unsafe{&(*signer_pointer_array[i])};
        signers.push(signer_ref);
    };

    let hash = Hash::new_from_array(latest_blockhash_ref.to_bytes());
    let mut ret = Transaction::new_with_payer(&instructions, Some(&payer_ref));
    match ret.try_sign(&signers, hash){
        Ok(_) => Box::into_raw(Box::new(ret)),
        Err(_) => std::ptr::null(),
    }
}

#[no_mangle]
pub extern "C" fn create_transaction_unsigned_with_payer(instruction_array: *mut *mut Instruction, array_size: c_int, payer: *const Pubkey) -> *const Transaction{
    let instruction_pointer_array = unsafe {
        Vec::from_raw_parts(instruction_array, array_size as usize, array_size as usize)
    };

    let payer_ref = unsafe{*payer};

    let mut instructions = vec![];
    for i in 0..instruction_pointer_array.len(){
        let instruction_ref = unsafe{(*instruction_pointer_array[i]).clone()};
        instructions.push(instruction_ref);
    };

    let ret = Transaction::new_with_payer(&instructions, Some(&payer_ref));
    Box::into_raw(Box::new(ret))
}

#[no_mangle]
pub extern "C" fn serialize_transaction(transaction: *mut Transaction, buffer: *mut c_uchar, buffer_size: c_int) -> c_int{
    let tx = unsafe{&(*transaction)};
    let serialized_tx = bincode::serialize(&tx).unwrap();
    if serialized_tx.len() > buffer_size as usize{
        return 0;
    }

    unsafe{
        ptr::copy_nonoverlapping(serialized_tx.as_ptr(), buffer as *mut u8, serialized_tx.len());
    };

    serialized_tx.len() as i32
}


#[no_mangle]
pub extern "C" fn sign_transaction(transaction: *mut Transaction, signers_array: *mut *mut Keypair, signers_array_size: c_int, latest_blockhash: *const Hash) -> c_int{
    let signer_pointer_array = unsafe {
        Vec::from_raw_parts(signers_array, signers_array_size as usize, signers_array_size as usize)
    };

    let latest_blockhash_ref = unsafe{*latest_blockhash};

    let mut signers = vec![];
    for i in 0..signer_pointer_array.len(){
        let signer_ref = unsafe{&(*signer_pointer_array[i])};
        signers.push(signer_ref);
    };

    let transaction_ref = unsafe{&mut(*transaction)};
    match transaction_ref.try_sign(&signers, latest_blockhash_ref){
        Ok(_) => 0,
        Err(_) => 1,
    }
}

#[no_mangle]
pub extern "C" fn partially_sign_transaction(transaction: *mut Transaction, signers_array: *mut *mut Keypair, signers_array_size: c_int, latest_blockhash: *const Hash) -> c_int{
    let signer_pointer_array = unsafe {
        Vec::from_raw_parts(signers_array, signers_array_size as usize, signers_array_size as usize)
    };

    let latest_blockhash_ref = unsafe{*latest_blockhash};

    let mut signers = vec![];
    for i in 0..signer_pointer_array.len(){
        let signer_ref = unsafe{&(*signer_pointer_array[i])};
        signers.push(signer_ref);
    };

    let transaction_ref = unsafe{&mut(*transaction)};
    match transaction_ref.try_partial_sign(&signers, latest_blockhash_ref){
        Ok(_) => 0,
        Err(_) => 1,
    }
}

#[no_mangle]
pub extern "C" fn free_transaction(transaction: *mut Transaction){
    unsafe{
        drop(Box::from_raw(transaction));
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        
    }
}
