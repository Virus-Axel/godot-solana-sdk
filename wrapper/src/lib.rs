#[macro_use]
extern crate lazy_static;

pub mod utils;

use crate::utils::{
    pointer_to_key,
    pointers_to_keypair,
};

use std::sync::Mutex;
use core::ffi::{
    c_char,
    c_int,
};

use solana_sdk::{
    instruction::AccountMeta,
    signer::keypair::Keypair,
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
pub extern fn append_account(account_pointer: *const c_char) -> c_int {
    // Convert the key
    let pub_key = match pointer_to_key(account_pointer){
        Ok(v) => v,
        Err(e) => return e,
    };

    let mut account_manager = match ACCOUNT_MANAGER.lock(){
        Ok(v) => v,
        Err(_) => return -1,
    };

    // Append account to vector
    account_manager.accounts.push(AccountMeta::new(pub_key, false));

    0
}

#[no_mangle]
pub extern fn append_signer(priv_key_pointer: *const c_char, pub_key_pointer: *const c_char) -> c_int{
    // Convert the keys
    let keypair = match pointers_to_keypair(priv_key_pointer, pub_key_pointer){
        Ok(v) => v,
        Err(e) => return e,
    };

    let mut account_manager = match ACCOUNT_MANAGER.lock(){
        Ok(v) => v,
        Err(_) => return -1,
    };

    // Append signer to vector
    account_manager.signers.push(keypair);

    0
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
