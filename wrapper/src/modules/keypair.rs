pub mod keypair{
    use solana_sdk::signer::keypair::Keypair;

    #[no_mangle]
    pub extern "C" fn create_keypair() -> *mut Keypair{
        Box::into_raw(Box::new(Keypair::new()))
    }

    #[no_mangle]
    pub extern "C" fn free_keypair(key: *mut Keypair){
        unsafe{
            drop(Box::from_raw(key));
        }
    }
}