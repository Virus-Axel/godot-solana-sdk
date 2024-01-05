#include "anchor_program.hpp"

#include "solana_client.hpp"
#include "solana_sdk.hpp"
#include "pubkey.hpp"

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/stream_peer_gzip.hpp>

namespace godot{

void AnchorProgram::register_instruction_builders(){
}

Variant AnchorProgram::idl_address(const Variant& pid){
    const Array seed;
    Variant program_signer = Pubkey::new_pda(seed, pid);
    return Pubkey::new_from_seed(program_signer, "anchor:idl", pid);
}

void AnchorProgram::load_from_pid(const String& pid){
    if(pid.length() < 32){
        return;
    }

    Dictionary rpc_result = SolanaClient::get_account_info(pid);
    
    if(!rpc_result.has("result")){
        return;
    }
    if(!((Dictionary)rpc_result["result"]).has("value")){
        return;
    }

    Dictionary account = ((Dictionary)rpc_result["result"])["value"];
    String idl_address = pid;

    if((bool)account["executable"]){
        idl_address = Pubkey(AnchorProgram::idl_address(Pubkey::new_from_string(pid))).get_value();

        rpc_result = SolanaClient::get_account_info(idl_address);
        if(!rpc_result.has("result")){
            return;
        }
        if(!((Dictionary)rpc_result["result"]).has("value")){
            return;
        }
        account = ((Dictionary)rpc_result["result"])["value"];
    }

    const PackedByteArray data = SolanaSDK::bs64_decode(((Array)account["data"])[0]);

    const int LENGTH_OFFSET = 40;
    const uint32_t compressed_length = data.decode_u32(LENGTH_OFFSET);

    const int DATA_OFFSET = 44;
    const PackedByteArray compressed_data = data.slice(DATA_OFFSET, DATA_OFFSET + compressed_length);

    StreamPeerGZIP stream;
    stream.start_decompression(true);

    stream.put_data(compressed_data);

    // TODO(Virax): According to docs I should call finish, but code tells me I shouldn't.
    // When docs are align remove.
    //stream.finish();

    const int DECOMPRESSED_LENGTH = stream.get_available_bytes();

    Array result = stream.get_data(DECOMPRESSED_LENGTH);

    const int error_code = result[0];
    PackedByteArray decompressed_data = result[1];

    Dictionary json_data = JSON::parse_string(decompressed_data.get_string_from_ascii());
    idl = json_data;
    notify_property_list_changed();
}

void AnchorProgram::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_idl"), &AnchorProgram::get_idl);
    ClassDB::bind_method(D_METHOD("set_idl", "idl"), &AnchorProgram::set_idl);
    ClassDB::bind_method(D_METHOD("get_try_from_pid"), &AnchorProgram::get_try_from_pid);
    ClassDB::bind_method(D_METHOD("set_try_from_pid", "try_from_pid"), &AnchorProgram::set_try_from_pid);
    ClassDB::bind_method(D_METHOD("get_try_from_json_file"), &AnchorProgram::get_try_from_json_file);
    ClassDB::bind_method(D_METHOD("set_try_from_json_file", "try_from_json_file"), &AnchorProgram::set_try_from_json_file);

    ClassDB::bind_method(D_METHOD("get_pid"), &AnchorProgram::get_pid);
    ClassDB::bind_method(D_METHOD("set_pid", "pid"), &AnchorProgram::set_pid);
    ClassDB::bind_method(D_METHOD("get_json_file"), &AnchorProgram::get_json_file);
    ClassDB::bind_method(D_METHOD("set_json_file", "json_file"), &AnchorProgram::set_json_file);

    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::STRING, "pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_pid", "get_pid");
    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::BOOL, "try_from_pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_try_from_pid", "get_try_from_pid");
    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::OBJECT, "json_file", PROPERTY_HINT_RESOURCE_TYPE, "JSON", PROPERTY_USAGE_DEFAULT), "set_json_file", "get_json_file");
    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::BOOL, "try_from_json_file", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_try_from_json_file", "get_try_from_json_file");
    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::DICTIONARY, "idl", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_READ_ONLY), "set_idl", "get_idl");
}


bool AnchorProgram::_set(const StringName &p_name, const Variant &p_value){
    String name = p_name;
    std::cout << name.ascii() << std::endl;
	if (name == "pid") {
		set_pid(p_value);
		return true;
	}
    else if(name == "try_from_pid"){
        set_try_from_pid(p_value);
		return true;
    }
    else if(name == "json_file"){
        set_json_file(p_value);
        return true;
    }
    else if(name == "try_from_json_file"){
        set_try_from_json_file(p_value);
        return true;
    }
    else if(name == "idl"){
        set_idl(p_value);
        return true;
    }
	return false;
}

bool AnchorProgram::_get(const StringName &p_name, Variant &r_ret) const{
    String name = p_name;
	if (name == "pid") {
		r_ret = pid;
		return true;
	}
    else if(name == "try_from_pid"){
        r_ret = try_from_pid;
		return true;
    }
    else if(name == "json_file"){
        r_ret = json_file;
        return true;
    }
    else if(name == "try_from_json_file"){
        r_ret = try_from_json_file;
        return true;
    }
    else if(name == "idl"){
        r_ret = idl;
        return true;
    }
	return false;
}

void AnchorProgram::_get_property_list(List<PropertyInfo> *p_list) const {
    if(!try_from_json_file && !try_from_pid){
        p_list->push_back(PropertyInfo(Variant::STRING, "pid", PROPERTY_HINT_NONE, ""));
    }
    else if(try_from_pid){
        p_list->push_back(PropertyInfo(Variant::STRING, "pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_READ_ONLY));
    }
    else{
        p_list->push_back(PropertyInfo(Variant::STRING, "pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
    }

    if(!try_from_json_file){
        p_list->push_back(PropertyInfo(Variant::BOOL, "try_from_pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR));
    }
    else{
        p_list->push_back(PropertyInfo(Variant::BOOL, "try_from_pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE));
    }

    if(!try_from_json_file && !try_from_pid){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "json_file", PROPERTY_HINT_RESOURCE_TYPE, "JSON"));
    }
    else if(try_from_json_file){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "json_file", PROPERTY_HINT_RESOURCE_TYPE, "JSON", PROPERTY_USAGE_STORAGE + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_READ_ONLY));
    }
    else{
        p_list->push_back(PropertyInfo(Variant::OBJECT, "json_file", PROPERTY_HINT_RESOURCE_TYPE, "JSON", PROPERTY_USAGE_STORAGE));
    }

    if(!try_from_pid){
        p_list->push_back(PropertyInfo(Variant::BOOL, "try_from_json_file", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR));
    }
    else{
        p_list->push_back(PropertyInfo(Variant::BOOL, "try_from_json_file", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE));

    }
	p_list->push_back(PropertyInfo(Variant::DICTIONARY, "idl", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_READ_ONLY));
}

void AnchorProgram::set_idl(const Dictionary& idl){
    this->idl = idl;
}

Dictionary AnchorProgram::get_idl(){
    return idl;
}

void AnchorProgram::set_try_from_pid(const bool try_from_pid){
    this->try_from_pid = try_from_pid;
    this->try_from_json_file = false;

    notify_property_list_changed();

    if(!try_from_pid){
        return;
    }

    load_from_pid(pid);
}

bool AnchorProgram::get_try_from_pid(){
    return try_from_pid;
}

void AnchorProgram::set_try_from_json_file(const bool try_from_json_file){
    this->try_from_json_file = try_from_json_file;
    this->try_from_pid = false;

    notify_property_list_changed();

    if(!try_from_json_file){
        return;
    }

    if(json_file.get_type() == Variant::OBJECT){
        const JSON *json_file = Object::cast_to<JSON>(this->json_file);
        idl = json_file->get_data();
    }
    else{
        idl = Dictionary();
    }
}

bool AnchorProgram::get_try_from_json_file(){
    return try_from_json_file;
}

void AnchorProgram::set_pid(const String& pid){
    try_from_pid = false;
    this->pid = pid;
}

String AnchorProgram::get_pid(){
    return pid;
}

void AnchorProgram::set_json_file(const Variant& json_file){
    try_from_json_file = false;
    this->json_file = json_file;
}

Variant AnchorProgram::get_json_file(){
    return json_file;
}

}