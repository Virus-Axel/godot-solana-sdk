use solana_sdk::{
    pubkey::Pubkey,
    account::Account,
};
use core::ffi::c_ulonglong;

extern crate alloc;
use alloc::boxed::Box;

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
