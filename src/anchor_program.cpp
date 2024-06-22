#include "anchor_program.hpp"

#include "solana_client.hpp"
#include "solana_utils.hpp"
#include "pubkey.hpp"
#include "sha256.hpp"
#include "instruction.hpp"

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/stream_peer_gzip.hpp>

namespace godot{

bool AnchorProgram::is_typed_primitive(const Dictionary &dict){
    return (dict.has("dataType") && dict.has("value") && dict.keys().size() && dict["dataType"] != String("option"));
}

PackedByteArray AnchorProgram::serialize_typed_primitive(const Dictionary &dict){
    PackedByteArray result;
    if(dict["dataType"] == "u8"){
        result.append(dict["value"]);
    }
    else if(dict["dataType"] == "u16"){
        result.resize(2);
        result.encode_u16(0, dict["value"]);
    }
    else if(dict["dataType"] == "u32"){
        result.resize(4);
        result.encode_u32(0, dict["value"]);
    }
    else if(dict["dataType"] == "u64"){
        result.resize(8);
        result.encode_u64(0, dict["value"]);
    }
    else if(dict["dataType"] == "i8"){
        result.resize(1);
        result.encode_s8(0, dict["value"]);
    }
    else if(dict["dataType"] == "i16"){
        result.resize(2);
        result.encode_s16(0, dict["value"]);
    }
    else if(dict["dataType"] == "i32"){
        result.resize(4);
        result.encode_s32(0, dict["value"]);
    }
    else if(dict["dataType"] == "i64"){
        result.resize(8);
        result.encode_s64(0, dict["value"]);
    }
    else if(dict["dataType"] == "f32"){
        result.resize(4);
        result.encode_half(0, dict["value"]);
    }
    else if(dict["dataType"] == "f64"){
        result.resize(8);
        result.encode_double(0, dict["value"]);
    }

    return result;
}

Dictionary AnchorProgram::find_idl_type(const String &name){
    if(!((Dictionary)idl).has("types")){
        internal::gdextension_interface_print_warning("IDL does not contain \"types\" key", "find_idl_type", __FILE__, __LINE__, false);
        return Dictionary();
    }

    Array types = ((Dictionary)idl)["types"];
    for(unsigned int i = 0; i < types.size(); i++){
        const Dictionary type = types[i];
        if(type["name"] == name){
            return type;
        }
    }

    return Dictionary();
}


Variant AnchorProgram::deserialize_dict(const PackedByteArray& bytes, const Dictionary& type, int &consumed_bytes){
    Dictionary result;
    
    if(type.has("kind")){
        if(type["kind"] == String("struct")){
            const Array fields = type["fields"];
            PackedByteArray temp_bytes = bytes;
            for(unsigned int i = 0; i < fields.size(); i++){
                const Dictionary type_ref = fields[i];
                int byte_offset = 0;
                const Variant val = deserialize_variant(temp_bytes, type_ref["type"], byte_offset);
                consumed_bytes += byte_offset;
                temp_bytes = temp_bytes.slice(byte_offset);
                result[type_ref["name"]] = val;
            }
        }
        else if(type["kind"] == String("enum")){
            consumed_bytes += 1;
            return bytes[0];
        }
        else{
            internal::gdextension_interface_print_warning("Unsupported Object", "deserialize_variant", __FILE__, __LINE__, true);
            return nullptr;
        }
    }
    else if(type.has("defined")){
        const String struct_name = type["defined"];
        const Dictionary idl_type = find_idl_type(struct_name);
        ERR_FAIL_COND_V_EDMSG(idl_type.is_empty(), nullptr, "Could not find type " + struct_name);
        return deserialize_dict(bytes, idl_type["type"], consumed_bytes);
    }
    else if(type.has("vec")){
        const Variant struct_type = type["vec"];
        Array values;
        const unsigned int array_length = bytes.decode_u32(0);
        consumed_bytes += 4;
        PackedByteArray temp_bytes = bytes.slice(4);
        for(unsigned int i = 0; i < array_length; i++){
            int byte_offset = 0;
            const Variant val = deserialize_variant(temp_bytes, struct_type, byte_offset);
            values.append(val);
            temp_bytes = temp_bytes.slice(byte_offset);
            consumed_bytes += byte_offset;
        }
        return values;
    }
    else if(type.has("option")){
        const Variant struct_type = type["option"];
        consumed_bytes += 1;
        if(bytes[0] == 0){
            return nullptr;
        }
        else{
            return deserialize_variant(bytes.slice(1), struct_type, consumed_bytes);
        }
    }
    else if(type.has("array")){
        String element_type = ((Array)type["array"])[0];
        const unsigned int array_length = ((Array)type["array"])[1];
        Array ret;
        PackedByteArray temp_bytes = bytes;
        for(unsigned int i = 0; i < array_length; i++){
            int byte_offset = 0;
            ret.append(deserialize_variant(temp_bytes, element_type, byte_offset));
            consumed_bytes += byte_offset;
            temp_bytes = temp_bytes.slice(byte_offset);
        }
        return ret;
    }
    else{
        internal::gdextension_interface_print_warning("Unsupported Object", "deserialize_variant", __FILE__, __LINE__, true);
        return nullptr;
    }

    return result;
}

Variant AnchorProgram::deserialize_variant(const PackedByteArray& bytes, const Variant& type, int &consumed_bytes){
    if(type == "u8"){
        consumed_bytes += 1;
        return bytes.decode_u8(0);
    }
    else if(type == "u16"){
        consumed_bytes += 2;
        return bytes.decode_u16(0);
    }
    else if(type == "u32"){
        consumed_bytes += 4;
        return bytes.decode_u32(0);
    }
    else if(type == "u64"){
        consumed_bytes += 8;
        return bytes.decode_u64(0);
    }
    else if(type == "i8"){
        consumed_bytes += 1;
        return bytes.decode_s8(0);
    }
    else if(type == "i16"){
        consumed_bytes += 2;
        return bytes.decode_s16(0);
    }
    else if(type == "i32"){
        consumed_bytes += 4;
        return bytes.decode_s32(0);
    }
    else if(type == "i64"){
        consumed_bytes += 8;
        return bytes.decode_s64(0);
    }
    else if(type == "f32"){
        consumed_bytes += 4;
        return bytes.decode_half(0);
    }
    else if(type == "f64"){
        consumed_bytes += 8;
        return bytes.decode_double(0);
    }
    else if(type == "string"){
        const int data_length = bytes.decode_u32(0);
        ERR_FAIL_COND_V_EDMSG(bytes.size() < data_length, nullptr, "Invalid data format.");
        consumed_bytes += 4 + data_length;
        return bytes.slice(4, 4 + data_length).get_string_from_ascii();
    }
    else if(((String)type).to_lower() == "publickey"){
        consumed_bytes += 32;
        return Pubkey::new_from_bytes(bytes.slice(0, 32));
    }
    else if(type == "bool"){
        consumed_bytes += 1;
        ERR_FAIL_COND_V_EDMSG(bytes.size() < 1, nullptr, "Invalid data format.");
        return bytes[0] == 1;
    }
    else if(type.get_type() == Variant::DICTIONARY){
        return deserialize_dict(bytes, type, consumed_bytes);
    }
    else{
        internal::gdextension_interface_print_warning("Unsupported Object", "deserialize_variant", __FILE__, __LINE__, true);
        return nullptr;
    }
}

AnchorProgram::AnchorProgram(){
    idl_client = memnew(SolanaClient);
    fetch_client = memnew(SolanaClient);
    idl_client->set_async_override(true);
    fetch_client->set_async_override(true);
}

void AnchorProgram::_process(double delta){
    idl_client->_process(delta);
    fetch_client->_process(delta);
}

PackedByteArray AnchorProgram::serialize_variant(const Variant &var){
    PackedByteArray result;
    switch (var.get_type())
    {
    case Variant::INT:
        result.resize(8);
        result.encode_s64(0, (int64_t) var);
        break;

    case Variant::BOOL:
        result.append((bool)var);
        break;

    case Variant::FLOAT:
        result.resize(8);
        result.encode_float(0, var);
        break;

    case Variant::STRING:
        result.resize(4);
        result.encode_u32(0, ((String)var).length());
        result.append_array(((String)var).to_ascii_buffer());
        break;

    case Variant::ARRAY:
        result.resize(4);
        result.encode_u32(0, ((Array)var).size());
        for(unsigned int i = 0; i < ((Array)var).size(); i++){
            result.append_array(serialize_variant(((Array)var)[i]));
        }
        break;
    
    case Variant::DICTIONARY:
        if(is_typed_primitive(var)){
            result.append_array(serialize_typed_primitive(var));
        }
        else if(is_option(var)){
            const Variant val = ((Dictionary)var)["value"];
            if(val.get_type() == Variant::NIL){
                result.append(0);
            }
            else{
                result.append(1);
                result.append_array(serialize_variant(val));
            }
        }
        else{
            for(unsigned int i = 0; i < ((Dictionary)var).values().size(); i++){
                result.append_array(serialize_variant(((Dictionary)var).values()[i]));
            }
        }
        break;
    
    case Variant::OBJECT:
        if(Pubkey::is_pubkey(var)){
            result.append_array(Object::cast_to<Pubkey>(var)->to_bytes());
        }
        else{
            internal::gdextension_interface_print_warning("Unsupported Object", "serialize_variant", __FILE__, __LINE__, true);
        }
    break;

    case Variant::NIL:
        break;

    default:
        const String message = String("Unsupported Variant ") + String::num_int64(var.get_type()) + String(", please report this issue at https://github.com/Virus-Axel/godot-solana-sdk/issues/new");
        internal::gdextension_interface_print_warning(message.ascii(), "serialize_variant", __FILE__, __LINE__, true);
        break;
    }

    return result;
}

void AnchorProgram::register_instruction_builders(){
}

PackedByteArray AnchorProgram::discriminator_by_name(const String &name){
    SHA256 hasher;
    const String namespace_string = "global:";
    hasher.update(namespace_string.to_ascii_buffer().ptr(), namespace_string.length());
    hasher.update(name.to_ascii_buffer().ptr(), name.length());
    
    uint8_t *hash = hasher.digest();
    PackedByteArray discriminator;

    const int DISCRIMINATOR_LENGTH = 8;

    discriminator.resize(DISCRIMINATOR_LENGTH);

    for(unsigned int i = 0; i < DISCRIMINATOR_LENGTH; i++){
        discriminator[i] = hash[i];
    }

    return discriminator;
}

Dictionary AnchorProgram::find_idl_instruction(const String &name){
    if(!((Dictionary)idl).has("instructions")){
        internal::gdextension_interface_print_warning("IDL does not contain \"instructions\" key", "find_idl_instruction", __FILE__, __LINE__, false);
        return Dictionary();
    }

    Array instructions = ((Dictionary)idl)["instructions"];
    for(unsigned int i = 0; i < instructions.size(); i++){
        const Dictionary instruction = instructions[i];
        if(instruction["name"] == name){
            return instruction;
        }
    }

    return Dictionary();
}

Dictionary AnchorProgram::find_idl_account(const String &name){
    if(!((Dictionary)idl).has("accounts")){
        internal::gdextension_interface_print_warning("IDL does not contain \"accounts\" key", "find_idl_account", __FILE__, __LINE__, false);
        return Dictionary();
    }

    Array accounts = ((Dictionary)idl)["accounts"];
    for(unsigned int i = 0; i < accounts.size(); i++){
        const Dictionary account = accounts[i];
        if(account["name"] == name){
            return account;
        }
    }

    return Dictionary();
}

Variant AnchorProgram::idl_address(const Variant& pid){
    const Array seed;
    Variant program_signer = Pubkey::new_pda(seed, pid);
    return Pubkey::new_from_seed(program_signer, "anchor:idl", pid);
}

bool AnchorProgram::load_from_pid(const String& pid){
    if(pid.length() < 32){
        return false;
    }

    Callable callback(this, "idl_from_pid_callback");
    idl_client->connect("http_response_received", callback);
    Dictionary rpc_result = idl_client->get_account_info(pid);
    return false;
}

void AnchorProgram::idl_from_pid_callback(const Dictionary& rpc_result){
    Callable callback(this, "idl_from_pid_callback");
    if(idl_client->is_connected("http_response_received", callback)){
        idl_client->disconnect("http_response_received", callback);
    }

    if(!rpc_result.has("result")){
        return;
    }
    if(!((Dictionary)rpc_result["result"]).has("value")){
        return;
    }

    Dictionary account = ((Dictionary)rpc_result["result"])["value"];
    String idl_address = pid;

    if((bool)account["executable"]){
        idl_address = Pubkey(AnchorProgram::idl_address(Pubkey::new_from_string(pid))).to_string();

        Callable callback(this, "idl_from_pid_callback");
        idl_client->connect("http_response_received", callback);
        idl_client->get_account_info(idl_address);
    }
    else{
        if(!account.has("data")){
            internal::gdextension_interface_print_warning("Program does not have an associated anchor account.", "load_from_pid", __FILE__, __LINE__, true);
            return;
        }

        const Array data_info = account["data"];

        extract_idl_from_data(data_info);
    }
}

void AnchorProgram::extract_idl_from_data(const Array& data_info){
    const PackedByteArray data = SolanaUtils::bs64_decode(data_info[0]);

    const int DATA_OFFSET = 44;

    if(data.size() <= DATA_OFFSET){
        internal::gdextension_interface_print_warning("Invalid associated Anchor account.", "load_from_pid", __FILE__, __LINE__, true);
        return;
    }

    const int LENGTH_OFFSET = 40;
    const uint32_t compressed_length = data.decode_u32(LENGTH_OFFSET);

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

    this->try_from_pid = true;
    notify_property_list_changed();
    emit_signal("idl_fetched");
}


bool AnchorProgram::is_int(const Variant &var){
    if(var.get_type() == Variant::INT){
        return true;
    }
    else if(var.get_type() == Variant::DICTIONARY){
        if(((Dictionary)var).has("dataType")){
            const String data_type = ((Dictionary)var)["dataType"];
            return (data_type == "u8" || data_type == "i8" || data_type == "u16" || data_type == "i16" ||
                    data_type == "u32" || data_type == "i32" || data_type == "u64" || data_type == "i64");
        }
    }
    return false;
}

bool AnchorProgram::is_float(const Variant &var){
    if(var.get_type() == Variant::FLOAT){
        return true;
    }
    else if(var.get_type() == Variant::DICTIONARY){
        if(((Dictionary)var).has("dataType")){
            const String data_type = ((Dictionary)var)["dataType"];
            return (data_type == "f32" || data_type == "f64");
        }
    }
    return false;
}

bool AnchorProgram::is_option(const Variant &var){
    if(var.get_type() == Variant::FLOAT){
        return true;
    }
    else if(var.get_type() == Variant::DICTIONARY){
        if(((Dictionary)var).has("dataType")){
            const String data_type = ((Dictionary)var)["dataType"];
            return (data_type == "option");
        }
    }
    return false;
}

bool AnchorProgram::check_type(const Variant& expected_type, const Variant& value){
    if(expected_type.get_type() == Variant::DICTIONARY){
        return (value.get_type() == Variant::DICTIONARY || value.get_type() == Variant::ARRAY);
    }
    else if(expected_type.get_type() == Variant::STRING){
        if( expected_type == "u8" || expected_type == "i8" || expected_type == "u16" || expected_type == "i16" ||
            expected_type == "u32" || expected_type == "i32" || expected_type == "u64" || expected_type == "i64"){
            return is_int(value);
        }
        else if(expected_type == "f32" || expected_type == "f64"){
            return is_float(value);
        }
    }

    // TODO(Virax): Implement IDL type check.

    return false;
}

bool AnchorProgram::validate_instruction_arguments(const String &instruction_name, const Array &arguments){
    /*Dictionary instruction = find_idl_instruction(instruction_name);
    if(instruction.is_empty()){
        return false;
    }

    const Array other_args = instruction["args"];

    if(arguments.size() != other_args.size()){
        return false;
    }
    for(unsigned int i = 0; i < other_args.size(); i++){
        const Variant expected_type = other_args[i]["type"];
        // TODO(Virax): Fix check type and call it here. Check IDL defined classes.
    }*/
    return true;
}

void AnchorProgram::_bind_methods(){
    ClassDB::add_signal("AnchorProgram", MethodInfo("idl_fetched"));
    ClassDB::add_signal("AnchorProgram", MethodInfo("account_fetched"));

    ClassDB::bind_method(D_METHOD("idl_from_pid_callback", "rpc_result"), &AnchorProgram::idl_from_pid_callback);
    ClassDB::bind_method(D_METHOD("fetch_account_callback", "rpc_result"), &AnchorProgram::fetch_account_callback);

    ClassDB::bind_method(D_METHOD("get_idl"), &AnchorProgram::get_idl);
    ClassDB::bind_method(D_METHOD("set_idl", "idl"), &AnchorProgram::set_idl);
    ClassDB::bind_method(D_METHOD("get_try_from_pid"), &AnchorProgram::get_try_from_pid);
    ClassDB::bind_method(D_METHOD("set_try_from_pid", "try_from_pid"), &AnchorProgram::set_try_from_pid);
    ClassDB::bind_method(D_METHOD("get_try_from_json_file"), &AnchorProgram::get_try_from_json_file);
    ClassDB::bind_method(D_METHOD("set_try_from_json_file", "try_from_json_file"), &AnchorProgram::set_try_from_json_file);

    ClassDB::bind_method(D_METHOD("get_pid"), &AnchorProgram::get_pid);
    ClassDB::bind_method(D_METHOD("set_pid", "pid"), &AnchorProgram::set_pid);
    ClassDB::bind_method(D_METHOD("get_json_file"), &AnchorProgram::get_json_file);

    ClassDB::bind_method(D_METHOD("build_instruction", "accounts", "arguments"), &AnchorProgram::build_instruction);
    ClassDB::bind_method(D_METHOD("fetch_account", "name", "account_address"), &AnchorProgram::fetch_account);

    ClassDB::bind_method(D_METHOD("set_json_file", "json_file"), &AnchorProgram::set_json_file);

    ClassDB::bind_static_method("AnchorProgram", D_METHOD("serialize_variant", "var"), &AnchorProgram::serialize_variant);

    ClassDB::bind_static_method("AnchorProgram", D_METHOD("f32", "val"), &AnchorProgram::f32);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("f64", "val"), &AnchorProgram::f64);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("s8", "val"), &AnchorProgram::s8);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("s16", "val"), &AnchorProgram::s16);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("s32", "val"), &AnchorProgram::s32);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("s64", "val"), &AnchorProgram::s64);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("u8", "val"), &AnchorProgram::u8);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("u16", "val"), &AnchorProgram::u16);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("u32", "val"), &AnchorProgram::u32);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("u64", "val"), &AnchorProgram::u64);
    ClassDB::bind_static_method("AnchorProgram", D_METHOD("option", "val"), &AnchorProgram::option);

    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::STRING, "pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_pid", "get_pid");
    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::BOOL, "try_from_pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_try_from_pid", "get_try_from_pid");
    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::OBJECT, "json_file", PROPERTY_HINT_RESOURCE_TYPE, "JSON", PROPERTY_USAGE_DEFAULT), "set_json_file", "get_json_file");
    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::BOOL, "try_from_json_file", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_try_from_json_file", "get_try_from_json_file");
    //ClassDB::add_property("AnchorProgram", PropertyInfo(Variant::DICTIONARY, "idl", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_READ_ONLY), "set_idl", "get_idl");
}


bool AnchorProgram::_set(const StringName &p_name, const Variant &p_value){
    String name = p_name;
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
    else if(name == "url_override"){
        set_url_override(p_value);
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
    else if(name == "url_override"){
        r_ret = url_override;
        return true;
    }
	return false;
}

void AnchorProgram::_get_property_list(List<PropertyInfo> *p_list) const {
    p_list->push_back(PropertyInfo(Variant::STRING, "url_override"));

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

void AnchorProgram::set_url_override(const String& url_override){
    this->url_override = url_override;
    idl_client->set_url_override(url_override);
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

    if(!load_from_pid(pid)){
        this->try_from_pid = false;
    }
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

Dictionary AnchorProgram::f32(float val){
    Dictionary res;
    res["dataType"] = "f32";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::f64(double val){
    Dictionary res;
    res["dataType"] = "f64";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::s8(int val){
    Dictionary res;
    res["dataType"] = "i8";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::s16(int val){
    Dictionary res;
    res["dataType"] = "i16";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::s32(int32_t val){
    Dictionary res;
    res["dataType"] = "i32";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::s64(int64_t val){
    Dictionary res;
    res["dataType"] = "i64";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::u8(unsigned int val){
    Dictionary res;
    res["dataType"] = "u8";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::u16(unsigned int val){
    Dictionary res;
    res["dataType"] = "u16";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::u32(uint32_t val){
    Dictionary res;
    res["dataType"] = "u32";
    res["value"] = val;

    return res;
}
Dictionary AnchorProgram::u64(uint64_t val){
    Dictionary res;
    res["dataType"] = "u64";
    res["value"] = val;

    return res;
}

Dictionary AnchorProgram::option(const Variant &val){
    Dictionary res;
    res["dataType"] = "option";
    res["value"] = val;

    return res;
}

Variant AnchorProgram::build_instruction(String name, Array accounts, Variant arguments){
    Instruction *result = memnew(Instruction);

    PackedByteArray data = discriminator_by_name(name.to_snake_case());

    result->set_program_id(Pubkey::new_from_string(pid));
    data.append_array(serialize_variant(arguments));
    result->set_data(data);

    const Dictionary instruction_info = find_idl_instruction(name);
    if(instruction_info.is_empty()){
        internal::gdextension_interface_print_warning((String("IDL does not contain an instruction named ") + name + ".").ascii(), "build_instruction", __FILE__, __LINE__, true);
        return nullptr;
    }

    if(((Array)instruction_info["accounts"]).size() != accounts.size()){
        internal::gdextension_interface_print_warning("Unexpected amount or accounts", "build_instruction", __FILE__, __LINE__, true);
        return nullptr;
    }

    Array ref_accounts = instruction_info["accounts"];

    for(unsigned int i = 0; i < ref_accounts.size(); i++){
        const bool writable = ((Dictionary)ref_accounts[i])["isMut"];
        const bool is_signer = ((Dictionary)ref_accounts[i])["isSigner"];
        result->append_meta(AccountMeta::new_account_meta(accounts[i], is_signer, writable));
    }

    return result;
}

Error AnchorProgram::fetch_account(const String name, const Variant& account){
    if(!pending_account_name.is_empty()){
        return Error::ERR_ALREADY_IN_USE;
    }
    pending_account_name = name;
    Callable callback(this, "fetch_account_callback");
    fetch_client->connect("http_response_received", callback, ConnectFlags::CONNECT_ONE_SHOT);
    Dictionary rpc_result = fetch_client->get_account_info(Pubkey(account).to_string());

    return Error::OK;
}

void AnchorProgram::fetch_account_callback(const Dictionary &rpc_result){
    String name = pending_account_name;
    
    Dictionary ref_struct = find_idl_account(name);

    if(ref_struct.is_empty()){
        emit_signal("account_fetched", Dictionary());
        ERR_FAIL_EDMSG("Account name was not found in IDL.");
    }

    pending_account_name = "";
    if(!rpc_result.has("result")){
        emit_signal("account_fetched", Dictionary());
        ERR_FAIL_COND_EDMSG(rpc_result.has("error"), String(rpc_result["result"]));
        ERR_FAIL_EDMSG("Unexpected RPC responce, no result.");
    }

    Dictionary result_dict = rpc_result["result"];
    Variant value;
    if(!result_dict.has("value")){
        emit_signal("account_fetched", Dictionary());
        ERR_FAIL_EDMSG("Unexpected RPC responce, no value.");
    }
    value = result_dict["value"];
    if(value.get_type() != Variant::DICTIONARY){
        emit_signal("account_fetched", Dictionary());
        ERR_FAIL_COND_EDMSG(value.get_type() == Variant::NIL, "Account does not exist");
        ERR_FAIL_EDMSG("Unexpected RPC responce, unknown value type.");
    }

    Dictionary account_dict = value;

    Array account_data_tuple = account_dict["data"];
    String encoded_data = account_data_tuple[0];

    PackedByteArray account_data = SolanaUtils::bs64_decode(encoded_data);

    account_data = account_data.slice(8);

    const Array fields = ((Dictionary)ref_struct["type"])["fields"];

    Dictionary result;
    for(unsigned int i = 0; i < fields.size(); i++){
        int data_offset = 0;
        const Variant val = deserialize_variant(account_data, ((Dictionary)fields[i])["type"], data_offset);
        result[((Dictionary)fields[i])["name"]] = val;
        account_data = account_data.slice(data_offset);
    }

    emit_signal("account_fetched", result);
}

}