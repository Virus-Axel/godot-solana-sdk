use solana_sdk::{
    instruction::Instruction,
    transaction::Transaction,
    signer::keypair::Keypair,
    pubkey::Pubkey,
    hash::Hash,
};
use std::{
    ffi::{
        c_int,
        c_uchar,
    },
    ptr,
};

#[no_mangle]
pub extern "C" fn create_transaction_signed_with_payer(instruction_array: *mut *mut Instruction, array_size: c_int, payer: *const Pubkey, signers_array: *mut *mut Keypair, signers_array_size: c_int, latest_blockhash: *const Pubkey) -> *const Transaction{
    let instruction_pointer_array = unsafe {
        Vec::from_raw_parts(instruction_array, array_size as usize, array_size as usize)
    };
    let signer_pointer_array = unsafe {
        Vec::from_raw_parts(signers_array, signers_array_size as usize, signers_array_size as usize)
    };

    let payer_ref = unsafe{*payer};
    let latest_blockhash_ref = unsafe{*latest_blockhash};

    let mut instructions = vec![];
    for i in 0..instruction_pointer_array.len(){
        let instruction_ref = unsafe{(*instruction_pointer_array[i]).clone()};
        instructions.push(instruction_ref);
    };

    let mut signers = vec![];
    for i in 0..signer_pointer_array.len(){
        let signer_ref = unsafe{&(*signer_pointer_array[i])};
        signers.push(signer_ref);
    };

    let hash = Hash::new_from_array(latest_blockhash_ref.to_bytes());
    let mut ret = Transaction::new_with_payer(&instructions, Some(&payer_ref));
    match ret.try_sign(&signers, hash){
        Ok(_) => Box::into_raw(Box::new(ret)),
        Err(_) => std::ptr::null(),
    }
}

#[no_mangle]
pub extern "C" fn create_transaction_unsigned_with_payer(instruction_array: *mut *mut Instruction, array_size: c_int, payer: *const Pubkey) -> *mut Transaction{
    let instruction_pointer_array = unsafe {
        Vec::from_raw_parts(instruction_array, array_size as usize, array_size as usize)
    };

    let payer_ref = unsafe{*payer};

    let mut instructions = vec![];
    for i in 0..instruction_pointer_array.len(){
        let instruction_ref = unsafe{(*instruction_pointer_array[i]).clone()};
        instructions.push(instruction_ref);
    };

    std::mem::forget(instruction_pointer_array);

    let ret = Transaction::new_with_payer(&instructions, Some(&payer_ref));
    Box::into_raw(Box::new(ret))
}

#[no_mangle]
pub extern "C" fn serialize_transaction(transaction: *mut Transaction, buffer: *mut c_uchar, buffer_size: c_int) -> c_int{
    let tx = unsafe{&(*transaction)};
    let serialized_tx = bincode::serialize(&tx).unwrap();
    if serialized_tx.len() > buffer_size as usize{
        return 0;
    }

    unsafe{
        ptr::copy_nonoverlapping(serialized_tx.as_ptr(), buffer as *mut u8, serialized_tx.len());
    };

    serialized_tx.len() as i32
}


#[no_mangle]
pub extern "C" fn sign_transaction(transaction: *mut Transaction, signers_array: *mut *mut Keypair, signers_array_size: c_int, latest_blockhash: *const Hash) -> c_int{
    let signer_pointer_array = unsafe {
        Vec::from_raw_parts(signers_array, signers_array_size as usize, signers_array_size as usize)
    };

    let latest_blockhash_ref = unsafe{*latest_blockhash};

    let mut signers = vec![];
    for i in 0..signer_pointer_array.len(){
        let signer_ref = unsafe{&(*signer_pointer_array[i])};
        signers.push(signer_ref);
    };

    std::mem::forget(signer_pointer_array);

    let transaction_ref = unsafe{&mut(*transaction)};
    match transaction_ref.try_sign(&signers, latest_blockhash_ref){
        Ok(_) => 0,
        Err(_) => 1,
    }
}

#[no_mangle]
pub extern "C" fn partially_sign_transaction(transaction: *mut Transaction, signers_array: *mut *mut Keypair, signers_array_size: c_int, latest_blockhash: *const Hash) -> c_int{
    let signer_pointer_array = unsafe {
        Vec::from_raw_parts(signers_array, signers_array_size as usize, signers_array_size as usize)
    };

    let latest_blockhash_ref = unsafe{*latest_blockhash};

    let mut signers = vec![];
    for i in 0..signer_pointer_array.len(){
        let signer_ref = unsafe{&(*signer_pointer_array[i])};
        signers.push(signer_ref);
    };

    std::mem::forget(signer_pointer_array);

    let transaction_ref = unsafe{&mut(*transaction)};
    match transaction_ref.try_partial_sign(&signers, latest_blockhash_ref){
        Ok(_) => 0,
        Err(_) => 1,
    }
}

#[no_mangle]
pub extern "C" fn free_transaction(transaction: *mut Transaction){
    unsafe{
        drop(Box::from_raw(transaction));
    }
}
