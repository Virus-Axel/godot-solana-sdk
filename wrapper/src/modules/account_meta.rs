use solana_sdk::{
    pubkey::Pubkey,
    instruction::AccountMeta,
};

extern crate alloc;
use alloc::boxed::Box;

#[no_mangle]
pub extern "C" fn create_account_meta(account_key: *const Pubkey, is_signer: bool, is_writeable: bool) -> *mut AccountMeta{
    let account_ref = unsafe{*account_key};
    if is_writeable{
        return Box::into_raw(Box::new(AccountMeta::new(account_ref, is_signer)));
    }
    else{
        return Box::into_raw(Box::new(AccountMeta::new_readonly(account_ref, is_signer)));
    }
}

#[no_mangle]
pub extern "C" fn free_account_meta(account_meta: *mut AccountMeta){
    unsafe{
        drop(Box::from_raw(account_meta));
    }
}
