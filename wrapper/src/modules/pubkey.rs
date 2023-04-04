use std::mem;

use solana_sdk::pubkey::Pubkey;

#[no_mangle]
pub extern "C" fn print_pubkey(key: *const Pubkey){
    unsafe{
        println!("{}\n", (*key).to_string());
    }
}

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
pub extern "C" fn free_pubkey(key: *mut Pubkey){
    unsafe{
        drop(Box::from_raw(key));
    }
}