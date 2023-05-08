use core::{mem, ffi::{c_char, c_uint}};

extern crate alloc;

use solana_sdk::pubkey::Pubkey;
use alloc::{boxed::Box, vec::Vec};

#[no_mangle]
pub extern "C" fn create_unique_pubkey() -> *mut Pubkey{
    Box::into_raw(Box::new(Pubkey::new_unique()))
}

#[no_mangle]
pub extern "C" fn create_pubkey_from_array(bytes: *mut u8) -> *mut Pubkey{
    let arr = unsafe{Vec::from_raw_parts(bytes, 32, 32)};
    let ret = Pubkey::new_from_array(arr.clone().try_into().unwrap());

    mem::forget(arr);
    Box::into_raw(Box::new(ret))
}

#[no_mangle]
pub extern "C" fn create_pubkey_with_seed(base: *mut Pubkey, seed: *mut c_char, seed_length: c_uint, owner: *mut Pubkey) -> *const Pubkey{
    let seed_bytes = unsafe { core::slice::from_raw_parts(seed as *const u8, seed_length as usize) };
    let base_key = unsafe{*base};
    let owner_key = unsafe{*owner};

    mem::forget(seed);

    let seed_slice = match core::str::from_utf8(seed_bytes){
        Ok(v) => v,
        Err(_) => return core::ptr::null(),
    };

    let ret = match Pubkey::create_with_seed(&base_key, seed_slice, &owner_key){
        Ok(v) => v,
        Err(_) => return core::ptr::null(),
    };

    Box::into_raw(Box::new(ret))
}

#[no_mangle]
pub extern "C" fn free_pubkey(key: *mut Pubkey){
    unsafe{
        drop(Box::from_raw(key));
    }
}
