#include "instruction.hpp"
#include "keypair.hpp"
#include "solana_sdk.hpp"

#include "utils.hpp"

#include <vector>
#include <algorithm>

#include <phantom.hpp>

#include <godot_cpp/core/class_db.hpp>

namespace godot{

using internal::gdextension_interface_print_warning;

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


CompiledKeys::CompiledKeys(){

}

void CompiledKeys::_bind_methods(){
}

CompiledKeys::CompiledKeys(TypedArray<Instruction> instructions, Variant &payer, Variant &latest_blockhash){
    unsigned int writable_signer_total = 0;
    unsigned int readonly_signer_total = 0;
    unsigned int writable_non_signer_total = 0;
    unsigned int readonly_non_signer_total = 0;

    TypedArray<Resource> merged_metas;
    
    this->latest_blockhash = latest_blockhash;

    // Append payer
    PhantomController *phantom = Object::cast_to<PhantomController>(payer);
    if(!phantom->is_connected()){
        std::cout << "TERROR" << std::endl;
    }
    PackedByteArray payer_key_bytes = phantom->get_connected_key();
    Pubkey *payer_key = memnew(Pubkey);
    payer_key->set_bytes(payer_key_bytes);
    merged_metas.append(memnew(AccountMeta(payer_key, true, true)));
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

    for(unsigned int i = 0; i < instructions.size(); i++){
        Instruction *element = Object::cast_to<Instruction>(instructions[i]);
        const TypedArray<AccountMeta> &account_metas = element->get_accounts();

        CompiledInstruction *compiled_instruction = memnew(CompiledInstruction);
        compiled_instruction->data = element->get_data();
        AccountMeta pid_meta(element->get_program_id(), false, false);
        compiled_instruction->program_id_index = locate_account_meta(merged_metas, pid_meta);

        for(unsigned int j = 0; j < account_metas.size(); j++){
            AccountMeta *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);
            compiled_instruction->accounts.push_back(locate_account_meta(merged_metas, *account_meta));
        }
        compiled_instructions.push_back(compiled_instruction);
    }

    for(unsigned int i = 0; i < merged_metas.size(); i++){
        AccountMeta *account_meta = Object::cast_to<AccountMeta>(merged_metas[i]);
        if(account_meta->get_is_signer()){
            num_required_signatures++;
            if(!account_meta->get_writeable()){
                readonly_signer_total++;
            }
        }
        else if(!account_meta->get_writeable()){
            readonly_non_signer_total++;
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

    num_required_signatures = writable_signer_total + readonly_signer_total;
    num_readonly_unsigned_accounts = readonly_non_signer_total;
    num_readonly_signed_accounts = readonly_signer_total;
}

PackedByteArray CompiledKeys::serialize(){
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

TypedArray<Resource> &CompiledKeys::get_signers(){
    return signers;
}

int CompiledKeys::locate_account_meta(const TypedArray<Resource>& arr, const AccountMeta &input){
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

CompiledKeys::~CompiledKeys(){
}

void Instruction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_program_id"), &Instruction::get_program_id);
    ClassDB::bind_method(D_METHOD("set_program_id", "p_value"), &Instruction::set_program_id);
    ClassDB::bind_method(D_METHOD("get_data"), &Instruction::get_data);
    ClassDB::bind_method(D_METHOD("set_data", "p_value"), &Instruction::set_data);
    ClassDB::bind_method(D_METHOD("get_accounts"), &Instruction::get_accounts);
    ClassDB::bind_method(D_METHOD("set_accounts", "p_value"), &Instruction::set_accounts);
    ClassDB::add_property("Instruction", PropertyInfo(Variant::OBJECT, "program_id", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", PROPERTY_USAGE_DEFAULT), "set_program_id", "get_program_id");
    ClassDB::add_property("Instruction", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data"), "set_data", "get_data");
    ClassDB::add_property("Instruction", PropertyInfo(Variant::ARRAY, "accounts", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("AccountMeta")), "set_accounts", "get_accounts");}

Instruction::Instruction() {
}

void Instruction::set_program_id(const Variant& p_value){
    program_id = p_value;
}
Variant Instruction::get_program_id(){
    return program_id;
}

void Instruction::set_data(const PackedByteArray& p_value){
    data = p_value;
}
PackedByteArray Instruction::get_data(){
    return data;
}

void Instruction::set_accounts(const TypedArray<AccountMeta>& p_value){
    accounts = p_value;
}

TypedArray<AccountMeta> Instruction::get_accounts(){
    return accounts;
}

Instruction::~Instruction() {
}

CompiledInstruction::CompiledInstruction(){
    accounts = PackedByteArray();
    data = PackedByteArray();
}

void CompiledInstruction::_bind_methods(){

}

PackedByteArray CompiledInstruction::serialize(){
    PackedByteArray result;

    result.append(program_id_index);
    result.append(accounts.size());

    for(unsigned int i = 0; i < accounts.size(); i++){
        result.append(accounts[i]);
    }

    result.append(data.size());
    for(unsigned int i = 0; i < data.size(); i++){
        result.append(data[i]);
    }

    return result;
}

/*const CompiledInstruction& CompiledInstruction::operator=(const CompiledInstruction& other){
    this->accounts = other.accounts;
    this->data = other.data;
    this->program_id_index = other.program_id_index;

    return *this;
}*/

CompiledInstruction::~CompiledInstruction(){

}

}