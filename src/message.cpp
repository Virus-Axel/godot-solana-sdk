#include <message.hpp>

#include <keypair.hpp>
#include <phantom.hpp>
#include <solana_sdk.hpp>

TypedArray<Resource> sort_metas(TypedArray<Resource> input){
    TypedArray<AccountMeta> m1;
    TypedArray<AccountMeta> m2;
    TypedArray<AccountMeta> m3;
    TypedArray<AccountMeta> m4;
    std::cout << "in" << std::endl;
    for(unsigned int i = 0; i < input.size(); i++){
        AccountMeta* element = Object::cast_to<AccountMeta>(input[i]);
        const int value = element->get_is_signer() * 2 + element->get_writeable() * 1;

        switch(value){
            case 3:
                m1.append(element);
            break;
            case 2:
                m2.append(element);
            break;
            case 1:
                m3.append(element);
            break;
            case 0:
                m4.append(element);
            break;
        }
    }
    TypedArray<AccountMeta> result;
    result.append_array(m1);
    result.append_array(m2);
    result.append_array(m3);
    result.append_array(m4);

    std::cout << "out" << std::endl;

    return result;
}

Message::Message(){

}

void Message::_bind_methods(){
}

void Message::compile_instruction(Variant instruction){
    Instruction *element = Object::cast_to<Instruction>(instruction);
    const TypedArray<AccountMeta> &account_metas = element->get_accounts();

    CompiledInstruction *compiled_instruction = memnew(CompiledInstruction);

    compiled_instruction->data = element->get_data();
    AccountMeta pid_meta(element->get_program_id(), false, false);
    compiled_instruction->program_id_index = locate_account_meta(merged_metas, pid_meta);

    compiled_instruction->accounts.push_back(payer_index);
    for(unsigned int j = 0; j < account_metas.size(); j++){
        AccountMeta *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);
        compiled_instruction->accounts.push_back(locate_account_meta(merged_metas, *account_meta));
    }
    compiled_instructions.push_back(compiled_instruction);
}

void Message::recalculate_headers(){
    // Phantom signer (fee payer) is mandatory.
    num_required_signatures = 1;
    num_readonly_signed_accounts = 1;

    for(unsigned int i = 0; i < merged_metas.size(); i++){
        AccountMeta *account_meta = Object::cast_to<AccountMeta>(merged_metas[i]);
        if(account_meta->get_is_signer()){
            num_required_signatures++;
            if(!account_meta->get_writeable()){
                num_readonly_signed_accounts++;
            }
        }
        else if(!account_meta->get_writeable()){
            num_readonly_unsigned_accounts++;
        }
        
        if(account_meta->get_is_signer()){
            Pubkey *new_key = memnew(Pubkey);
            new_key->set_bytes(Object::cast_to<Keypair>(account_meta->get_pubkey())->get_public_bytes());
            account_keys.push_back(new_key);
        }
        else{
            account_keys.push_back(account_meta->get_pubkey());
        }
    }

    num_readonly_signed_accounts = 1;
    num_readonly_unsigned_accounts = 2;
}

Message::Message(TypedArray<Instruction> instructions, Variant &payer, Variant &latest_blockhash){
    
    this->latest_blockhash = latest_blockhash;

    // Append payer
    PhantomController *phantom = Object::cast_to<PhantomController>(payer);
    if(!phantom->is_connected()){
        std::cout << "TERROR" << std::endl;
    }
    PackedByteArray payer_key_bytes = phantom->get_connected_key();
    Pubkey *payer_key = memnew(Pubkey);
    payer_key->set_bytes(payer_key_bytes);
    AccountMeta *payer_meta = memnew(AccountMeta(payer_key, true, true));
    merged_metas.append(payer_meta);
    std::cout << payer_key_bytes.size() << " DEBUGGG: " << SolanaSDK::bs58_encode(payer_key_bytes).to_utf8_buffer().ptr() << std::endl;

    for(unsigned int i = 0; i < instructions.size(); i++){
        Instruction *element = Object::cast_to<Instruction>(instructions[i]);
        const TypedArray<AccountMeta> &account_metas = element->get_accounts();

        AccountMeta *pid_meta = memnew(AccountMeta(element->get_program_id(), false, false));

        // If keys match, merge metas
        if(locate_account_meta(merged_metas, *pid_meta) != -1){
            std::cout << "NO" << std::endl;
            const int index = locate_account_meta(merged_metas, *pid_meta);
            AccountMeta *meta_1 = Object::cast_to<AccountMeta>(merged_metas[index]);
            meta_1->set_is_signer(meta_1->get_is_signer() || pid_meta->get_is_signer());
            meta_1->set_writeable(meta_1->get_writeable() || pid_meta->get_writeable());
        }
        else{
            std::cout << "NOP" << std::endl;
            merged_metas.append(pid_meta);
        }

        for(unsigned int j = 0; j < account_metas.size(); j++){
            AccountMeta *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);
            
            if(account_meta->get_is_signer()){
                // Actually a keypair.
                signers.push_back(account_meta->get_pubkey());
            }

            std::cout << "sAA" << std::endl;

            // If keys match, merge metas
            if(locate_account_meta(merged_metas, *account_meta) != -1){
                std::cout << "POP" << std::endl;
                const int index = locate_account_meta(merged_metas, *account_meta);

                AccountMeta *meta_1 = Object::cast_to<AccountMeta>(merged_metas[index]);
                AccountMeta *meta_2 = account_meta;
                meta_1->set_is_signer(meta_1->get_is_signer() || meta_2->get_is_signer());
                meta_1->set_writeable(meta_1->get_writeable() || meta_2->get_writeable());

                // Signers should make Keypair override pubkey.
                if(meta_2->get_is_signer()){
                    std::cout << "POP" << std::endl;
                    meta_1->set_pubkey(meta_2->get_pubkey());
                }
            }
            else{
                AccountMeta *new_meta = Object::cast_to<AccountMeta>(account_metas[j]);
                merged_metas.push_back(new_meta);
            }
        }
    }

    // Sort with custom function that looks at signer/writeable.
    TypedArray<Resource> temp;
    merged_metas = sort_metas(merged_metas);

    std::cout << "Sorted merged meta size is: " << merged_metas.size() << std::endl;

    // Store payer index.
    payer_index = locate_account_meta(merged_metas, *payer_meta);

    for(unsigned int i = 0; i < instructions.size(); i++){
        compile_instruction(instructions[i]);
    }

    recalculate_headers();
}

PackedByteArray Message::serialize(){
    PackedByteArray result;

    result.append(num_required_signatures);
    result.append(num_readonly_signed_accounts);
    result.append(num_readonly_unsigned_accounts);

    result.append(account_keys.size());
    for(unsigned int i = 0; i < account_keys.size(); i++){
        Pubkey *account_key = Object::cast_to<Pubkey>(account_keys[i]);
        result.append_array(account_key->get_bytes());
    }

    Hash *latest_blockhash_pubkey = Object::cast_to<Hash>(latest_blockhash);
    result.append_array(latest_blockhash_pubkey->get_bytes());

    result.append(compiled_instructions.size());

    for(unsigned int i = 0; i < compiled_instructions.size(); i++){
        CompiledInstruction *inst = Object::cast_to<CompiledInstruction>(compiled_instructions[i]);
        result.append_array(inst->serialize());
    }
    return result;
}

TypedArray<Resource> &Message::get_signers(){
    return signers;
}

int Message::locate_account_meta(const TypedArray<Resource>& arr, const AccountMeta &input){
    for(unsigned int i = 0; i < arr.size(); i++){
        AccountMeta *element = Object::cast_to<AccountMeta>(arr[i]);
        
        PackedByteArray key;
        PackedByteArray other_key;
        if(element->get_is_signer()){
            Keypair *kp = Object::cast_to<Keypair>(element->get_pubkey());
            key = kp->get_public_bytes();
        }
        else{
            key = Object::cast_to<Pubkey>(element->get_pubkey())->get_bytes();
        }
        if(input.get_is_signer()){
            other_key = Object::cast_to<Keypair>(input.get_pubkey())->get_public_bytes();
        }
        else{
            other_key = Object::cast_to<Pubkey>(input.get_pubkey())->get_bytes();
        }
        if (other_key == key){
            return i;
        }
    }
    return -1;
}

Message::~Message(){
}