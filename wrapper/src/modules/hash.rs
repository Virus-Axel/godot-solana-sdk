use core::mem;

use solana_sdk::hash::Hash;

extern crate alloc;
use alloc::{boxed::Box, vec::Vec};

#[no_mangle]
pub extern "C" fn create_unique_hash() -> *mut Hash{
    Box::into_raw(Box::new(Hash::new_unique()))
}

#[no_mangle]
pub extern "C" fn create_hash_from_array(bytes: *mut u8) -> *mut Hash{
    let arr = unsafe{Vec::from_raw_parts(bytes, 32, 32)};
    let ret = Hash::new_from_array(arr.clone().try_into().unwrap());

    mem::forget(arr);

    Box::into_raw(Box::new(ret))
}

#[no_mangle]
pub extern "C" fn free_hash(key: *mut Hash){
    unsafe{
        drop(Box::from_raw(key));
    }
}
