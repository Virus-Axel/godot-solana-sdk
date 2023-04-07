use crate::modules::account::*;

use solana_sdk::{account::Account, pubkey::Pubkey};


#[test]
fn test_create_account() {
    let id = Pubkey::new_unique();

    let compare_account = Account::new(10, 10, &id);

    let account_ref = create_account(10, 10, &id);
    let account = unsafe{&(*account_ref)};
    assert_eq!(&compare_account, account);

    free_account(account_ref);
}

