use solana_sdk::signer::keypair::Keypair;
use std::mem;

#[no_mangle]
pub extern "C" fn create_keypair() -> *mut Keypair{
    Box::into_raw(Box::new(Keypair::new()))
}

#[no_mangle]
pub extern "C" fn create_keypair_from_bytes(bytes: *mut u8) -> *mut Keypair{
    let arr = unsafe{Vec::from_raw_parts(bytes, 64, 64)};

    let ret = match Keypair::from_bytes(arr.clone().as_slice()){
        Ok(v) => v,
        Err(_) => return std::ptr::null::<Keypair>() as *mut Keypair,
    };

    mem::forget(arr);

    Box::into_raw(Box::new(ret))
}

#[no_mangle]
pub extern "C" fn free_keypair(key: *mut Keypair){
    unsafe{
        drop(Box::from_raw(key));
    }
}
