use solana_sdk::hash::Hash;

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
