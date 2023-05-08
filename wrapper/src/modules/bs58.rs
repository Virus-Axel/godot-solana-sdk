use core::ffi::{
    c_char,
    c_uint,
};

use core::mem;


#[no_mangle]
pub extern "C" fn bs58_decode(pointer: *const c_char, input_length: c_uint, result: *mut c_char) -> c_uint{
    let key_string = unsafe { core::slice::from_raw_parts(pointer as *const u8, input_length as usize) };
    let decoded = bs58::decode(key_string);
    let decoded_vec = match decoded.into_vec(){
        Ok(v) => v,
        Err(_) => return 0,
    };
    
    unsafe{
        core::ptr::copy_nonoverlapping(decoded_vec.as_ptr(), result as *mut u8, decoded_vec.len());
    }
    
    mem::forget(pointer);

    decoded_vec.len() as c_uint
}

#[no_mangle]
pub extern "C" fn bs58_encode(pointer: *const c_char, input_length: c_uint, result: *mut c_char) -> c_uint{
    let key_string = unsafe { core::slice::from_raw_parts(pointer as *const u8, input_length as usize) };
    let encoded = bs58::encode(key_string);
    let encoded_vec = encoded.into_vec();
    
    unsafe{
        core::ptr::copy_nonoverlapping(encoded_vec.as_ptr(), result as *mut u8, encoded_vec.len());
    }
    
    mem::forget(pointer);

    encoded_vec.len() as c_uint
}
