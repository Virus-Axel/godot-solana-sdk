#include <message.hpp>

#include <keypair.hpp>
#include <phantom.hpp>
#include <solana_sdk.hpp>
#include "compute_budget.hpp"

TypedArray<AccountMeta> sort_metas(TypedArray<AccountMeta> input){
    TypedArray<AccountMeta> m1;
    TypedArray<AccountMeta> m2;
    TypedArray<AccountMeta> m3;
    TypedArray<AccountMeta> m4;

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
            default:
                // TODO: Handle error.
                return Array();
            break;
        }
    }
    TypedArray<AccountMeta> result;
    result.append_array(m1);
    result.append_array(m2);
    result.append_array(m3);
    result.append_array(m4);

    return result;
}

Message::Message(){
    latest_blockhash = "";
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

    //compiled_instruction->accounts.push_back(payer_index);
    for(unsigned int j = 0; j < account_metas.size(); j++){
        AccountMeta *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);
        compiled_instruction->accounts.push_back(locate_account_meta(merged_metas, *account_meta));
    }
    compiled_instructions.push_back(compiled_instruction);
}

void Message::recalculate_headers(){
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
        
        account_keys.push_back(account_meta->get_pubkey());
    }
}

void Message::merge_account_meta(const AccountMeta &account_meta){
    const int index = locate_account_meta(merged_metas, account_meta);
    if(index != -1){
        AccountMeta *meta_1 = Object::cast_to<AccountMeta>(merged_metas[index]);
        meta_1->set_is_signer(meta_1->get_is_signer() || account_meta.get_is_signer());
        meta_1->set_writeable(meta_1->get_writeable() || account_meta.get_writeable());
    }
    else{
        Variant new_element = AccountMeta::new_from_fields(account_meta.get_pubkey(), account_meta.get_is_signer(), account_meta.get_writeable());
        merged_metas.append(new_element);
    }}

void Message::merge_signer(const Variant& signer){
    for(unsigned int i = 0; i < signers.size(); i++){
        if(Pubkey(signers[i]) == Pubkey(signer)){
            return;
        }
    }

    signers.append(signer);
}

Message::Message(TypedArray<Instruction> instructions, const Variant &payer){
    // Payer is signer.
    signers.append(payer);

    AccountMeta *payer_meta = memnew(AccountMeta(payer, true, true));
    merged_metas.append(payer_meta);

    latest_blockhash = "";

    // Prepend ComputeBudget instructions.
    instructions.insert(0, ComputeBudget::set_compute_unit_limit(200000));
    instructions.insert(1, ComputeBudget::set_compute_unit_price(8000));
    for(unsigned int i = 0; i < instructions.size(); i++){
        Instruction *element = Object::cast_to<Instruction>(instructions[i]);
        const TypedArray<AccountMeta> &account_metas = element->get_accounts();

        merge_account_meta(AccountMeta(element->get_program_id(), false, false));

        for(unsigned int j = 0; j < account_metas.size(); j++){
            AccountMeta *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);

            if(account_meta->get_is_signer()){
                // Actually a keypair.
                merge_signer(account_meta->get_signer());
            }

            merge_account_meta(*account_meta);
        }
    }

    // Sort with custom function that looks at signer/writeable.
    merged_metas = sort_metas(merged_metas);

    // Store payer index.
    payer_index = locate_account_meta(merged_metas, *payer_meta);

    for(unsigned int i = 0; i < instructions.size(); i++){
        compile_instruction(instructions[i]);
    }

    recalculate_headers();
}

void Message::set_latest_blockhash(const String& blockhash){
    latest_blockhash = blockhash;
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

    result.append_array(serialize_blockhash());

    result.append(compiled_instructions.size());

    for(unsigned int i = 0; i < compiled_instructions.size(); i++){
        CompiledInstruction *inst = Object::cast_to<CompiledInstruction>(compiled_instructions[i]);
        result.append_array(inst->serialize());
    }
    return result;
}

Array &Message::get_signers(){
    return signers;
}

Message& Message::operator=(const Message& other){
    payer_index = other.payer_index;
    num_required_signatures = other.num_required_signatures;
    num_readonly_signed_accounts = other.num_readonly_signed_accounts;
    num_readonly_unsigned_accounts = other.num_readonly_unsigned_accounts;
    account_keys = other.account_keys;
    latest_blockhash = other.latest_blockhash;
    compiled_instructions = other.compiled_instructions;
    signers = other.signers;

    merged_metas = other.merged_metas;
    return *this;
}

int Message::locate_account_meta(const TypedArray<AccountMeta>& arr, const AccountMeta &input){
    for(unsigned int i = 0; i < arr.size(); i++){
        if (Pubkey(merged_metas[i]) == input.get_pubkey()){
            return i;
        }
    }
    return -1;
}

PackedByteArray Message::serialize_blockhash(){
    if(latest_blockhash.is_empty()){
        PackedByteArray result;
        result.resize(32);
        return result;
    }

    return SolanaSDK::bs58_decode(latest_blockhash);
}

int Message::get_amount_signers(){
    return num_required_signatures;
}

Message::~Message(){
}