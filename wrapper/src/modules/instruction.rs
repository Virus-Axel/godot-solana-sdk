use solana_sdk::{
    instruction::{
        Instruction,
        AccountMeta,
    },
    pubkey::Pubkey,
};
use std::ffi::{
    c_uchar,
    c_int,
};

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

    std::mem::forget(pointer_array);

    let data = unsafe {
        Vec::from_raw_parts(data_array, data_size as usize, data_size as usize)
    };

    let ret = Instruction::new_with_bytes(program_id_ref, &data, account_metas);

    std::mem::forget(data);

    Box::into_raw(Box::new(ret))
}

#[no_mangle]
pub extern "C" fn free_instruction(account_meta: *mut Instruction){
    unsafe{
        drop(Box::from_raw(account_meta));
    }
}
