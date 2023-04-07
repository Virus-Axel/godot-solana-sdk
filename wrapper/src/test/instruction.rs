use crate::modules::instruction::*;

use solana_sdk::{pubkey::Pubkey, instruction::{AccountMeta, Instruction}};


#[test]
fn test_create_instruction_with_bytes() {
    let program_id = Pubkey::new_unique();
    let test_account_meta = AccountMeta::new(Pubkey::new_unique(), true);
    let mut bytes = vec![1, 2, 3];

    let account_metas = vec![
        test_account_meta.clone(),
    ];

    let compare_instruction = Instruction::new_with_bytes(program_id, &bytes, account_metas);

    let program_ref = [program_id].as_mut_ptr();
    let account_pointers = [[test_account_meta].as_mut_ptr()].as_mut_ptr();
    
    let instruction_ptr = create_instruction_with_bytes(program_ref, bytes.as_mut_ptr(), 3, account_pointers, 1);
    let instruction = unsafe{&(*instruction_ptr)};

    assert_eq!(&compare_instruction, instruction);

    free_instruction(instruction_ptr);
}

#[test]
fn test_pointer_free() {
    let program_id = Pubkey::new_unique();
    let test_account_meta = AccountMeta::new(Pubkey::new_unique(), true);
    let mut bytes = vec![1, 2, 3];

    let program_ref = [program_id].as_mut_ptr();
    let account_pointers = [[test_account_meta].as_mut_ptr()].as_mut_ptr();
    
    let instruction_ptr = create_instruction_with_bytes(program_ref, bytes.as_mut_ptr(), 3, account_pointers, 1);
    let real_instruction = unsafe{(*instruction_ptr).clone()};

    free_instruction(instruction_ptr);
    let fake_instruction = unsafe{(*instruction_ptr).clone()};

    assert_ne!(fake_instruction, real_instruction);
}

