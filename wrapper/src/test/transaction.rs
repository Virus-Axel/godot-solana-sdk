use crate::modules::transaction::*;
use crate::modules::instruction::*;

use solana_sdk::signature::Keypair;
use solana_sdk::signer::Signer;
use solana_sdk::transaction::Transaction;
use solana_sdk::{pubkey::Pubkey, hash::Hash, instruction::{AccountMeta, Instruction}};


#[test]
fn test_create_unsigned_transaction_with_payer() {
    let program_id = Pubkey::new_unique();
    let payer = Pubkey::new_unique();
    let test_account_meta = AccountMeta::new(Pubkey::new_unique(), true);
    let mut bytes = vec![1, 2, 3];

    let account_metas = vec![
        test_account_meta.clone(),
    ];

    let compare_instruction = Instruction::new_with_bytes(program_id, &bytes, account_metas);

    let program_ref = [program_id].as_mut_ptr();
    let account_pointers = [[test_account_meta].as_mut_ptr()].as_mut_ptr();
    
    let instruction_ptr = create_instruction_with_bytes(program_ref, bytes.as_mut_ptr(), 3, account_pointers, 1);

    let compare_transaction = Transaction::new_with_payer(&[compare_instruction], Some(&payer));
    let transaction_ptr = create_transaction_unsigned_with_payer(&mut[instruction_ptr] as *mut *mut Instruction, 1, &payer);
    let transaction = unsafe{&(*transaction_ptr)};

    assert_eq!(&compare_transaction, transaction);

    free_instruction(instruction_ptr);
    free_transaction(transaction_ptr);
}


#[test]
fn test_transaction_sign() {
    let program_id = Pubkey::new_unique();
    let mut signing_keypair = Keypair::new();
    let payer = signing_keypair.pubkey();
    let hash = Hash::new_unique();
    let test_account_meta = AccountMeta::new(Pubkey::new_unique(), false);
    let mut bytes = vec![1, 2, 3];

    let account_metas = vec![
        test_account_meta.clone(),
    ];

    let compare_instruction = Instruction::new_with_bytes(program_id, &bytes, account_metas);

    let program_ref = [program_id].as_mut_ptr();
    let account_pointers = [[test_account_meta].as_mut_ptr()].as_mut_ptr();
    
    let instruction_ptr = create_instruction_with_bytes(program_ref, bytes.as_mut_ptr(), 3, account_pointers, 1);

    let mut compare_transaction = Transaction::new_with_payer(&[compare_instruction], Some(&payer));
    let transaction_ptr = create_transaction_unsigned_with_payer(&mut[instruction_ptr] as *mut *mut Instruction, 1, &payer);
    let transaction = unsafe{&(*transaction_ptr)};

    compare_transaction.sign(&[&signing_keypair], hash);
    sign_transaction(transaction_ptr, &mut [&mut signing_keypair as *mut Keypair] as *mut *mut Keypair, 1, &hash);

    assert_eq!(&compare_transaction, transaction);

    free_instruction(instruction_ptr);
    free_transaction(transaction_ptr);
}

#[test]
fn test_transaction_partial_sign() {
    let program_id = Pubkey::new_unique();
    let mut signing_keypair = Keypair::new();
    let payer = Pubkey::new_unique();
    let hash = Hash::new_unique();
    let test_account_meta = AccountMeta::new(signing_keypair.pubkey(), true);
    let mut bytes = vec![1, 2, 3];

    let account_metas = vec![
        test_account_meta.clone(),
    ];

    let compare_instruction = Instruction::new_with_bytes(program_id, &bytes, account_metas);

    let program_ref = [program_id].as_mut_ptr();
    let account_pointers = [[test_account_meta].as_mut_ptr()].as_mut_ptr();
    
    let instruction_ptr = create_instruction_with_bytes(program_ref, bytes.as_mut_ptr(), 3, account_pointers, 1);

    let mut compare_transaction = Transaction::new_with_payer(&[compare_instruction], Some(&payer));
    let transaction_ptr = create_transaction_unsigned_with_payer(&mut[instruction_ptr] as *mut *mut Instruction, 1, &payer);
    let transaction = unsafe{&(*transaction_ptr)};

    compare_transaction.partial_sign(&[&signing_keypair], hash);
    partially_sign_transaction(transaction_ptr, &mut [&mut signing_keypair as *mut Keypair] as *mut *mut Keypair, 1, &hash);

    assert_eq!(&compare_transaction, transaction);

    free_instruction(instruction_ptr);
    free_transaction(transaction_ptr);
}

#[test]
fn test_serialize_transaction() {
    let program_id = Pubkey::new_unique();
    let payer = Pubkey::new_unique();
    let test_account_meta = AccountMeta::new(Pubkey::new_unique(), true);
    let mut bytes = vec![1, 2, 3];

    let account_metas = vec![
        test_account_meta.clone(),
    ];

    let compare_instruction = Instruction::new_with_bytes(program_id, &bytes, account_metas);

    let program_ref = [program_id].as_mut_ptr();
    let account_pointers = [[test_account_meta].as_mut_ptr()].as_mut_ptr();
    
    let instruction_ptr = create_instruction_with_bytes(program_ref, bytes.as_mut_ptr(), 3, account_pointers, 1);

    let compare_transaction = Transaction::new_with_payer(&[compare_instruction], Some(&payer));
    let transaction_ptr = create_transaction_unsigned_with_payer(&mut[instruction_ptr] as *mut *mut Instruction, 1, &payer);

    let compare_serialized = bincode::serialize(&compare_transaction).unwrap();
    let max_buffer_size = 2048;
    let mut buffer = vec![0; max_buffer_size];

    let serialized_size = serialize_transaction(transaction_ptr, buffer.as_mut_ptr(), max_buffer_size as i32);
    let serialized = &buffer[0..serialized_size as usize];

    assert_eq!(compare_serialized, serialized);

    free_instruction(instruction_ptr);
    free_transaction(transaction_ptr);
}
