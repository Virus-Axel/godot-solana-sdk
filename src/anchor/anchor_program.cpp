#include "anchor/anchor_program.hpp"

#include <cstdint>

#include "anchor/generic_anchor_node.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/stream_peer_gzip.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "account_meta.hpp"
#include "instruction.hpp"
#include "pubkey.hpp"
#include "sha256.hpp"
#include "solana_client.hpp"
#include "solana_utils.hpp"
#include "utils.hpp"

namespace godot {

bool AnchorProgram::detect_writable(const Dictionary &account) const {
	if (account.has("isMut")) {
		return account["isMut"];
	} else if (account.has("writable")) {
		return account["writable"];
	} else {
		return false;
	}
}

bool AnchorProgram::detect_is_signer(const Dictionary &account) const {
	if (account.has("isSigner")) {
		return account["isSigner"];
	} else if (account.has("signer")) {
		return account["signer"];
	} else {
		return false;
	}
}

bool AnchorProgram::detect_optional(const Dictionary &account) const {
	if (account.has("isOptional")) {
		return account["isOptional"];
	} else if (account.has("optional")) {
		return account["optional"];
	} else {
		return false;
	}
}

PackedByteArray AnchorProgram::evaluate_discriminant(const Dictionary &discriminant_info) {
	ERR_FAIL_COND_V(!discriminant_info.has("type"), {});
	ERR_FAIL_COND_V(!discriminant_info.has("value"), {});
	ERR_FAIL_COND_V_EDMSG_CUSTOM(discriminant_info["type"] != "u8", {}, "Only u8 discriminants are supported");

	return { static_cast<uint8_t>(discriminant_info["value"]) };
}

bool AnchorProgram::is_typed_primitive(const Dictionary &dict) {
	return (dict.has("dataType") && dict.has("value") && dict.keys().size() && dict["dataType"] != String("option"));
}

PackedByteArray AnchorProgram::serialize_typed_primitive(const Dictionary &dict) {
	PackedByteArray result;
	if (dict["dataType"] == "u8") {
		result.append(dict["value"]);
	} else if (dict["dataType"] == "u16") {
		result.resize(2);
		result.encode_u16(0, dict["value"]);
	} else if (dict["dataType"] == "u32") {
		result.resize(4);
		result.encode_u32(0, dict["value"]);
	} else if (dict["dataType"] == "u64") {
		result.resize(8);
		result.encode_u64(0, dict["value"]);
	} else if (dict["dataType"] == "i8") {
		result.resize(1);
		result.encode_s8(0, dict["value"]);
	} else if (dict["dataType"] == "i16") {
		result.resize(2);
		result.encode_s16(0, dict["value"]);
	} else if (dict["dataType"] == "i32") {
		result.resize(4);
		result.encode_s32(0, dict["value"]);
	} else if (dict["dataType"] == "i64") {
		result.resize(8);
		result.encode_s64(0, dict["value"]);
	} else if (dict["dataType"] == "f32") {
		result.resize(4);
		result.encode_half(0, dict["value"]);
	} else if (dict["dataType"] == "f64") {
		result.resize(8);
		result.encode_double(0, dict["value"]);
	}

	return result;
}

Dictionary AnchorProgram::find_idl_type(const String &name) {
	if (!((Dictionary)idl).has("types")) {
		internal::gdextension_interface_print_warning("IDL does not contain \"types\" key", "find_idl_type", __FILE__, __LINE__, false);
		return Dictionary();
	}

	Array types = ((Dictionary)idl)["types"];
	for (unsigned int i = 0; i < types.size(); i++) {
		const Dictionary type = types[i];
		if (type["name"] == name) {
			return type;
		}
	}

	return Dictionary();
}

Variant AnchorProgram::deserialize_dict(const PackedByteArray &bytes, const Dictionary &type, int &consumed_bytes) {
	Dictionary result;

	if (type.has("kind")) {
		if (type["kind"] == String("struct")) {
			const Array fields = type["fields"];
			PackedByteArray temp_bytes = bytes;
			for (unsigned int i = 0; i < fields.size(); i++) {
				const Dictionary type_ref = fields[i];
				int byte_offset = 0;
				const Variant val = deserialize_variant(temp_bytes, type_ref["type"], byte_offset);
				consumed_bytes += byte_offset;
				temp_bytes = temp_bytes.slice(byte_offset);
				result[type_ref["name"]] = val;
			}
		} else if (type["kind"] == String("enum")) {
			consumed_bytes += 1;
			return bytes[0];
		} else {
			internal::gdextension_interface_print_warning("Unsupported Object", "deserialize_variant", __FILE__, __LINE__, true);
			return nullptr;
		}
	} else if (type.has("defined")) {
		const String struct_name = type["defined"];
		const Dictionary idl_type = find_idl_type(struct_name);
		ERR_FAIL_COND_V_EDMSG(idl_type.is_empty(), nullptr, "Could not find type " + struct_name);
		return deserialize_dict(bytes, idl_type["type"], consumed_bytes);
	} else if (type.has("vec")) {
		const Variant struct_type = type["vec"];
		Array values;
		const unsigned int array_length = bytes.decode_u32(0);
		consumed_bytes += 4;
		PackedByteArray temp_bytes = bytes.slice(4);
		for (unsigned int i = 0; i < array_length; i++) {
			int byte_offset = 0;
			const Variant val = deserialize_variant(temp_bytes, struct_type, byte_offset);
			values.append(val);
			temp_bytes = temp_bytes.slice(byte_offset);
			consumed_bytes += byte_offset;
		}
		return values;
	} else if (type.has("option")) {
		const Variant struct_type = type["option"];
		consumed_bytes += 1;
		if (bytes[0] == 0) {
			return nullptr;
		} else {
			return deserialize_variant(bytes.slice(1), struct_type, consumed_bytes);
		}
	} else if (type.has("array")) {
		String element_type = ((Array)type["array"])[0];
		const unsigned int array_length = ((Array)type["array"])[1];
		Array ret;
		PackedByteArray temp_bytes = bytes;
		for (unsigned int i = 0; i < array_length; i++) {
			int byte_offset = 0;
			ret.append(deserialize_variant(temp_bytes, element_type, byte_offset));
			consumed_bytes += byte_offset;
			temp_bytes = temp_bytes.slice(byte_offset);
		}
		return ret;
	} else {
		internal::gdextension_interface_print_warning("Unsupported Object", "deserialize_variant", __FILE__, __LINE__, true);
		return nullptr;
	}

	return result;
}

Variant AnchorProgram::deserialize_variant(const PackedByteArray &bytes, const Variant &type, int &consumed_bytes) {
	if (type == "u8") {
		consumed_bytes += 1;
		return bytes.decode_u8(0);
	} else if (type == "u16") {
		consumed_bytes += 2;
		return bytes.decode_u16(0);
	} else if (type == "u32") {
		consumed_bytes += 4;
		return bytes.decode_u32(0);
	} else if (type == "u64") {
		consumed_bytes += 8;
		return bytes.decode_u64(0);
	} else if (type == "i8") {
		consumed_bytes += 1;
		return bytes.decode_s8(0);
	} else if (type == "i16") {
		consumed_bytes += 2;
		return bytes.decode_s16(0);
	} else if (type == "i32") {
		consumed_bytes += 4;
		return bytes.decode_s32(0);
	} else if (type == "i64") {
		consumed_bytes += 8;
		return bytes.decode_s64(0);
	} else if (type == "f32") {
		consumed_bytes += 4;
		return bytes.decode_half(0);
	} else if (type == "f64") {
		consumed_bytes += 8;
		return bytes.decode_double(0);
	} else if (type == "string") {
		const int data_length = bytes.decode_u32(0);
		ERR_FAIL_COND_V_EDMSG(bytes.size() < data_length, nullptr, "Invalid data format.");
		consumed_bytes += 4 + data_length;
		return bytes.slice(4, 4 + data_length).get_string_from_ascii();
	} else if (((String)type).to_lower() == "publickey") {
		consumed_bytes += 32;
		return Pubkey::new_from_bytes(bytes.slice(0, 32));
	} else if (type == "bool") {
		consumed_bytes += 1;
		ERR_FAIL_COND_V_EDMSG(bytes.size() < 1, nullptr, "Invalid data format.");
		return bytes[0] == 1;
	} else if (type.get_type() == Variant::DICTIONARY) {
		return deserialize_dict(bytes, type, consumed_bytes);
	} else {
		internal::gdextension_interface_print_warning("Unsupported Object", "deserialize_variant", __FILE__, __LINE__, true);
		return nullptr;
	}
}

Variant AnchorProgram::decorate_instruction_argument(const Variant &anchor_type, const Variant &argument) {
	if (is_option(anchor_type)) {
		return option(argument);
	}
	const Variant::Type godot_type = get_godot_type(anchor_type);
	if (godot_type == Variant::INT) {
		if (anchor_type == "u8") {
			return u8(argument);
		}
		if (anchor_type == "u16") {
			return u16(argument);
		}
		if (anchor_type == "u32") {
			return u32(argument);
		}
		if (anchor_type == "u64") {
			return u64(argument);
		}
		if (anchor_type == "s8") {
			return s8(argument);
		}
		if (anchor_type == "s16") {
			return s16(argument);
		}
		if (anchor_type == "s32") {
			return s32(argument);
		}
		if (anchor_type == "s64") {
			return s64(argument);
		}
	} else if (godot_type == Variant::FLOAT) {
		if (anchor_type == "f32") {
			return f32(argument);
		}
		if (anchor_type == "f64") {
			return f64(argument);
		}
	}
	return argument;
}

AnchorProgram::AnchorProgram() {
	idl_client = memnew(SolanaClient);
	fetch_client = memnew(SolanaClient);
	idl_client->set_async_override(true);
	fetch_client->set_async_override(true);
}

void AnchorProgram::_process(double delta) {
	idl_client->_process(delta);
	fetch_client->_process(delta);
}

Variant AnchorProgram::get_address_from_idl(const Dictionary &idl) {
	if (idl.has("metadata")) {
		const Dictionary metadata = idl["metadata"];
		if (metadata.has("address")) {
			return Pubkey::new_from_variant(metadata["address"]);
		}
	}
	if (idl.has("address")) {
		return Pubkey::new_from_variant(idl["address"]);
	}
	return nullptr;
}

PackedByteArray AnchorProgram::serialize_variant(const Variant &var) {
	PackedByteArray result;
	switch (var.get_type()) {
		case Variant::INT:
			result.resize(8);
			result.encode_s64(0, (int64_t)var);
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

		case Variant::PACKED_BYTE_ARRAY:
			result.resize(4);
			result.encode_u32(0, (static_cast<PackedByteArray>(var).size()));
			result.append_array(static_cast<PackedByteArray>(var));
			break;

		case Variant::ARRAY:
			result.resize(4);
			result.encode_u32(0, ((Array)var).size());
			for (unsigned int i = 0; i < ((Array)var).size(); i++) {
				result.append_array(serialize_variant(((Array)var)[i]));
			}
			break;

		case Variant::DICTIONARY:
			if (is_typed_primitive(var)) {
				result.append_array(serialize_typed_primitive(var));
			} else if (is_option(var)) {
				const Variant val = ((Dictionary)var)["value"];
				if (val.get_type() == Variant::NIL) {
					result.append(0);
				} else {
					result.append(1);
					result.append_array(serialize_variant(val));
				}
			} else {
				for (unsigned int i = 0; i < ((Dictionary)var).values().size(); i++) {
					result.append_array(serialize_variant(((Dictionary)var).values()[i]));
				}
			}
			break;

		case Variant::OBJECT:
			if (Pubkey::is_pubkey(var)) {
				result.append_array(Object::cast_to<Pubkey>(var)->to_bytes());
			} else if (static_cast<Object *>(var)->has_method("serialize")) {
				result.append_array(static_cast<Object *>(var)->callv("serialize", Array()));
			} else {
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

void AnchorProgram::register_instruction_builders() {
}

PackedByteArray AnchorProgram::discriminator_by_name(const String &name, const String &namespace_string) const {
	SHA256 hasher;
	hasher.update(namespace_string.to_ascii_buffer().ptr(), namespace_string.length());
	if (name == "mint_v_1") {
		String another_name = "mint_v1";
		hasher.update(another_name.to_ascii_buffer().ptr(), another_name.length());
	} else {
		hasher.update(name.to_ascii_buffer().ptr(), name.length());
	}

	uint8_t *hash = hasher.digest();
	PackedByteArray discriminator;

	discriminator.resize(DISCRIMINATOR_LENGTH);

	for (unsigned int i = 0; i < DISCRIMINATOR_LENGTH; i++) {
		discriminator[i] = hash[i];
	}

	return discriminator;
}

Dictionary AnchorProgram::find_idl_instruction(const String &name) const {
	if (!((Dictionary)idl).has("instructions")) {
		internal::gdextension_interface_print_warning("IDL does not contain \"instructions\" key", "find_idl_instruction", __FILE__, __LINE__, false);
		return Dictionary();
	}

	Array instructions = ((Dictionary)idl)["instructions"];
	for (unsigned int i = 0; i < instructions.size(); i++) {
		const Dictionary instruction = instructions[i];
		if (instruction["name"] == name) {
			return instruction;
		}
	}

	return Dictionary();
}

Dictionary AnchorProgram::find_idl_account(const String &name) {
	if (!((Dictionary)idl).has("accounts")) {
		internal::gdextension_interface_print_warning("IDL does not contain \"accounts\" key", "find_idl_account", __FILE__, __LINE__, false);
		return Dictionary();
	}

	Array accounts = ((Dictionary)idl)["accounts"];
	for (unsigned int i = 0; i < accounts.size(); i++) {
		const Dictionary account = accounts[i];
		if (String(account["name"]) == name) {
			return account;
		}
	}

	return Dictionary();
}

Variant AnchorProgram::idl_address(const Variant &pid) {
	const Array seed;
	Variant program_signer = Pubkey::new_pda(seed, pid);
	return Pubkey::new_from_seed(program_signer, "anchor:idl", pid);
}

bool AnchorProgram::load_from_pid(const String &pid) {
	if (pid.length() < 32) {
		return false;
	}

	Callable callback(this, "idl_from_pid_callback");
	idl_client->connect("http_response_received", callback);
	idl_client->get_account_info(pid);
	return false;
}

void AnchorProgram::idl_from_pid_callback(const Dictionary &rpc_result) {
	Callable callback(this, "idl_from_pid_callback");
	if (idl_client->is_connected("http_response_received", callback)) {
		idl_client->disconnect("http_response_received", callback);
	}

	if (!rpc_result.has("result")) {
		return;
	}
	if (!((Dictionary)rpc_result["result"]).has("value")) {
		return;
	}

	Dictionary account = ((Dictionary)rpc_result["result"])["value"];
	String idl_address = pid;

	if ((bool)account["executable"]) {
		idl_address = Pubkey::string_from_variant(AnchorProgram::idl_address(Pubkey::new_from_string(pid)));

		Callable callback(this, "idl_from_pid_callback");
		idl_client->connect("http_response_received", callback);
		idl_client->get_account_info(idl_address);
	} else {
		if (!account.has("data")) {
			internal::gdextension_interface_print_warning("Program does not have an associated anchor account.", "load_from_pid", __FILE__, __LINE__, true);
			return;
		}

		const Array data_info = account["data"];

		extract_idl_from_data(data_info);
	}
}

void AnchorProgram::extract_idl_from_data(const Array &data_info) {
	const PackedByteArray data = SolanaUtils::bs64_decode(data_info[0]);

	const int DATA_OFFSET = 44;

	if (data.size() <= DATA_OFFSET) {
		internal::gdextension_interface_print_warning("Invalid associated Anchor account.", "load_from_pid", __FILE__, __LINE__, true);
		return;
	}

	const int LENGTH_OFFSET = 40;
	const uint32_t compressed_length = data.decode_u32(LENGTH_OFFSET);

	const PackedByteArray compressed_data = data.slice(DATA_OFFSET, DATA_OFFSET + compressed_length);

	StreamPeerGZIP *stream = memnew(StreamPeerGZIP);
	stream->start_decompression(true);

	stream->put_data(compressed_data);

	// TODO(Virax): According to docs I should call finish, but code tells me I shouldn't.
	// When docs are align remove.
	//stream.finish();

	const int DECOMPRESSED_LENGTH = stream->get_available_bytes();

	Array result = stream->get_data(DECOMPRESSED_LENGTH);
	memfree(stream);

	const int error_code = result[0];
	PackedByteArray decompressed_data = result[1];

	Dictionary json_data = JSON::parse_string(decompressed_data.get_string_from_ascii());
	idl = json_data;

	this->try_from_pid = true;
	notify_property_list_changed();
	emit_signal("idl_fetched");
}

bool AnchorProgram::is_int(const Variant &var) {
	if (var.get_type() == Variant::INT) {
		return true;
	} else if (var.get_type() == Variant::DICTIONARY) {
		if (((Dictionary)var).has("dataType")) {
			const String data_type = ((Dictionary)var)["dataType"];
			return (data_type == "u8" || data_type == "i8" || data_type == "u16" || data_type == "i16" ||
					data_type == "u32" || data_type == "i32" || data_type == "u64" || data_type == "i64");
		}
	}
	return false;
}

bool AnchorProgram::is_float(const Variant &var) {
	if (var.get_type() == Variant::FLOAT) {
		return true;
	} else if (var.get_type() == Variant::DICTIONARY) {
		if (((Dictionary)var).has("dataType")) {
			const String data_type = ((Dictionary)var)["dataType"];
			return (data_type == "f32" || data_type == "f64");
		}
	}
	return false;
}

bool AnchorProgram::is_option(const Variant &var) {
	if (var.get_type() == Variant::FLOAT) {
		return true;
	} else if (var.get_type() == Variant::DICTIONARY) {
		if (((Dictionary)var).has("dataType")) {
			const String data_type = ((Dictionary)var)["dataType"];
			return (data_type == "option");
		}
	}
	return false;
}

bool AnchorProgram::check_type(const Variant &expected_type, const Variant &value) {
	if (expected_type.get_type() == Variant::DICTIONARY) {
		return (value.get_type() == Variant::DICTIONARY || value.get_type() == Variant::ARRAY);
	} else if (expected_type.get_type() == Variant::STRING) {
		if (expected_type == "u8" || expected_type == "i8" || expected_type == "u16" || expected_type == "i16" ||
				expected_type == "u32" || expected_type == "i32" || expected_type == "u64" || expected_type == "i64") {
			return is_int(value);
		} else if (expected_type == "f32" || expected_type == "f64") {
			return is_float(value);
		}
	}

	// TODO(Virax): Implement IDL type check.

	return false;
}

bool AnchorProgram::validate_instruction_arguments(const String &instruction_name, const Array &arguments) {
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

void AnchorProgram::_bind_methods() {
	ClassDB::add_signal("AnchorProgram", MethodInfo("idl_fetched"));
	ClassDB::add_signal("AnchorProgram", MethodInfo("account_fetched", PropertyInfo(Variant::DICTIONARY, "account_data")));
	ClassDB::add_signal("AnchorProgram", MethodInfo("account_data_fetched", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "account_data")));
	ClassDB::add_signal("AnchorProgram", MethodInfo("accounts_fetched"));

	ClassDB::bind_method(D_METHOD("idl_from_pid_callback", "rpc_result"), &AnchorProgram::idl_from_pid_callback);
	ClassDB::bind_method(D_METHOD("fetch_account_callback", "rpc_result"), &AnchorProgram::fetch_account_callback);
	ClassDB::bind_method(D_METHOD("fetch_all_accounts_callback", "rpc_result"), &AnchorProgram::fetch_all_accounts_callback);

	ClassDB::bind_method(D_METHOD("get_idl"), &AnchorProgram::get_idl);
	ClassDB::bind_method(D_METHOD("set_idl", "idl"), &AnchorProgram::set_idl);
	ClassDB::bind_method(D_METHOD("get_try_from_pid"), &AnchorProgram::get_try_from_pid);
	ClassDB::bind_method(D_METHOD("set_try_from_pid", "try_from_pid"), &AnchorProgram::set_try_from_pid);
	ClassDB::bind_method(D_METHOD("get_try_from_json_file"), &AnchorProgram::get_try_from_json_file);
	ClassDB::bind_method(D_METHOD("set_try_from_json_file", "try_from_json_file"), &AnchorProgram::set_try_from_json_file);

	ClassDB::bind_method(D_METHOD("get_pid"), &AnchorProgram::get_pid);
	ClassDB::bind_method(D_METHOD("set_pid", "pid"), &AnchorProgram::set_pid);
	ClassDB::bind_method(D_METHOD("get_json_file"), &AnchorProgram::get_json_file);

	ClassDB::bind_method(D_METHOD("build_instruction", "instruction_name", "accounts", "arguments"), &AnchorProgram::build_instruction);
	ClassDB::bind_method(D_METHOD("fetch_account", "name", "account_address"), &AnchorProgram::fetch_account);
	ClassDB::bind_method(D_METHOD("fetch_all_accounts", "additional_filters"), &AnchorProgram::fetch_all_accounts, DEFVAL(Array()));

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

bool AnchorProgram::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "pid") {
		set_pid(p_value);
		return true;
	} else if (name == "try_from_pid") {
		set_try_from_pid(p_value);
		return true;
	} else if (name == "json_file") {
		set_json_file(p_value);
		return true;
	} else if (name == "try_from_json_file") {
		set_try_from_json_file(p_value);
		return true;
	} else if (name == "idl") {
		set_idl(p_value);
		return true;
	} else if (name == "url_override") {
		set_url_override(p_value);
		return true;
	}
	return false;
}

bool AnchorProgram::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "pid") {
		r_ret = pid;
		return true;
	} else if (name == "try_from_pid") {
		r_ret = try_from_pid;
		return true;
	} else if (name == "json_file") {
		r_ret = json_file;
		return true;
	} else if (name == "try_from_json_file") {
		r_ret = try_from_json_file;
		return true;
	} else if (name == "idl") {
		r_ret = idl;
		return true;
	} else if (name == "url_override") {
		r_ret = url_override;
		return true;
	}
	return false;
}

void AnchorProgram::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::STRING, "url_override"));
	p_list->push_back(PropertyInfo(Variant::STRING, "pid", PROPERTY_HINT_NONE, ""));

	if (!try_from_json_file) {
		p_list->push_back(PropertyInfo(Variant::BOOL, "try_from_pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR));
	} else {
		p_list->push_back(PropertyInfo(Variant::BOOL, "try_from_pid", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE));
	}

	if (!try_from_json_file && !try_from_pid) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "json_file", PROPERTY_HINT_RESOURCE_TYPE, "JSON"));
	} else if (try_from_json_file) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "json_file", PROPERTY_HINT_RESOURCE_TYPE, "JSON", PROPERTY_USAGE_STORAGE + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_READ_ONLY));
	} else {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "json_file", PROPERTY_HINT_RESOURCE_TYPE, "JSON", PROPERTY_USAGE_STORAGE));
	}

	if (!try_from_pid) {
		p_list->push_back(PropertyInfo(Variant::BOOL, "try_from_json_file", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR));
	} else {
		p_list->push_back(PropertyInfo(Variant::BOOL, "try_from_json_file", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE));
	}
	p_list->push_back(PropertyInfo(Variant::DICTIONARY, "idl", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE + PROPERTY_USAGE_EDITOR + PROPERTY_USAGE_READ_ONLY));
}

void AnchorProgram::set_idl(const Dictionary &idl) {
	this->idl = idl;
}

void AnchorProgram::set_url_override(const String &url_override) {
	this->url_override = url_override;
	idl_client->set_url_override(url_override);
}

Dictionary AnchorProgram::get_idl() const {
	return idl;
}

void AnchorProgram::set_try_from_pid(const bool try_from_pid) {
	this->try_from_pid = try_from_pid;
	this->try_from_json_file = false;

	notify_property_list_changed();

	if (!try_from_pid) {
		return;
	}

	if (!load_from_pid(pid)) {
		this->try_from_pid = false;
	}
}

bool AnchorProgram::get_try_from_pid() {
	return try_from_pid;
}

void AnchorProgram::set_try_from_json_file(const bool try_from_json_file) {
	this->try_from_json_file = try_from_json_file;
	this->try_from_pid = false;

	notify_property_list_changed();

	if (!try_from_json_file) {
		return;
	}

	if (json_file.get_type() == Variant::OBJECT) {
		const JSON *json_file = Object::cast_to<JSON>(this->json_file);
		idl = json_file->get_data();
		if (idl.has("address")) {
			set_pid(idl["address"]);
		} else {
			WARN_PRINT_ONCE("Could not find Program ID from IDL.");
		}
	} else {
		idl = Dictionary();
	}
}

bool AnchorProgram::get_try_from_json_file() {
	return try_from_json_file;
}

void AnchorProgram::set_pid(const String &pid) {
	try_from_pid = false;
	this->pid = pid;
}

String AnchorProgram::get_pid() {
	return pid;
}

String AnchorProgram::get_idl_name() const {
	return idl["name"];
}

void AnchorProgram::set_json_file(const Variant &json_file) {
	try_from_json_file = false;
	this->json_file = json_file;
}

Variant AnchorProgram::get_json_file() {
	return json_file;
}

Dictionary AnchorProgram::f32(float val) {
	Dictionary res;
	res["dataType"] = "f32";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::f64(double val) {
	Dictionary res;
	res["dataType"] = "f64";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::s8(int val) {
	Dictionary res;
	res["dataType"] = "i8";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::s16(int val) {
	Dictionary res;
	res["dataType"] = "i16";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::s32(int32_t val) {
	Dictionary res;
	res["dataType"] = "i32";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::s64(int64_t val) {
	Dictionary res;
	res["dataType"] = "i64";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::u8(unsigned int val) {
	Dictionary res;
	res["dataType"] = "u8";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::u16(unsigned int val) {
	Dictionary res;
	res["dataType"] = "u16";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::u32(uint32_t val) {
	Dictionary res;
	res["dataType"] = "u32";
	res["value"] = val;

	return res;
}
Dictionary AnchorProgram::u64(uint64_t val) {
	Dictionary res;
	res["dataType"] = "u64";
	res["value"] = val;

	return res;
}

Dictionary AnchorProgram::option(const Variant &val) {
	Dictionary res;
	res["dataType"] = "option";
	res["value"] = val;

	return res;
}

bool AnchorProgram::is_enum(const Variant &anchor_type) {
	if (anchor_type.get_type() == Variant::DICTIONARY) {
		const Dictionary anchor_type_dict = anchor_type;
		if (anchor_type_dict.has("kind")) {
			return (anchor_type_dict["kind"] == "enum");
		}
		if (anchor_type_dict.has("type")) {
			return is_enum(anchor_type_dict["type"]);
		}
	}
	return false;
}

String AnchorProgram::get_object_name(const Variant &anchor_type) {
	if (anchor_type.get_type() == Variant::DICTIONARY) {
		const Dictionary anchor_type_dict = anchor_type;
		if (anchor_type_dict.has("option")) {
			return AnchorProgram::get_object_name(anchor_type_dict["option"]);
		}
		if (anchor_type_dict.has("vec")) {
			return AnchorProgram::get_object_name(anchor_type_dict["vec"]);
		}
		if (anchor_type_dict.has("defined")) {
			return anchor_type_dict["defined"];
		}
	}

	return "";
}

bool AnchorProgram::is_vec_type(const Variant &anchor_type) {
	if (anchor_type.get_type() == Variant::DICTIONARY) {
		const Dictionary anchor_type_dict = anchor_type;
		if (anchor_type_dict.has("option")) {
			return AnchorProgram::is_vec_type(anchor_type_dict["option"]);
		}
		if (anchor_type_dict.has("vec")) {
			return true;
		}
	}

	return false;
}

String AnchorProgram::get_int_type_info(const Variant &anchor_type) {
	if (anchor_type.get_type() == Variant::STRING) {
		const String type_string = anchor_type;
		if (type_string == "u8" || type_string == "u16" || type_string == "u32" || type_string == "u64" ||
				type_string == "i8" || type_string == "i16" || type_string == "i32" || type_string == "i64" ||
				type_string == "f32" || type_string == "f64") {
			return type_string;
		}
	}

	return "";
}

Variant::Type AnchorProgram::get_godot_type(const Variant &anchor_type) {
	if (anchor_type.get_type() == Variant::DICTIONARY) {
		const Dictionary anchor_type_dict = anchor_type;
		if (anchor_type_dict.has("option")) {
			return AnchorProgram::get_godot_type(anchor_type_dict["option"]);
		}
		if (anchor_type_dict.has("kind")) {
			if (anchor_type_dict["kind"] == "enum") {
				return Variant::INT;
			} else {
				return Variant::OBJECT;
			}
		}
		if (anchor_type_dict.has("defined") || anchor_type_dict.has("type")) {
			return Variant::OBJECT;
		}
		if (anchor_type_dict.has("array") || anchor_type_dict.has("vec")) {
			return Variant::ARRAY;
		}
	} else if (anchor_type.get_type() == Variant::STRING) {
		const String type_string = anchor_type;
		if (type_string == "publicKey") {
			return Variant::OBJECT;
		}
		if (type_string == "string") {
			return Variant::STRING;
		}
		if (type_string == "u8" || type_string == "u16" || type_string == "u32" || type_string == "u64" ||
				type_string == "i8" || type_string == "i16" || type_string == "i32" || type_string == "i64") {
			return Variant::INT;
		}
		if (type_string == "f32" || type_string == "f64") {
			return Variant::FLOAT;
		}
		if (type_string == "bool") {
			return Variant::BOOL;
		}
		if (type_string == "bytes") {
			return Variant::PACKED_BYTE_ARRAY;
		}
	}
	WARN_PRINT_ONCE_ED("Unknown anchor type");
	return Variant::NIL;
}

String AnchorProgram::get_godot_class_hint(const Variant &anchor_type) {
	if (anchor_type.get_type() == Variant::DICTIONARY) {
		const Dictionary anchor_type_dict = anchor_type;
		if (anchor_type_dict.has("option")) {
			return AnchorProgram::get_godot_class_hint(anchor_type_dict["option"]);
		}
		if (anchor_type_dict.has("kind")) {
			return "";
		}
		if (anchor_type_dict.has("defined")) {
			return anchor_type_dict["defined"];
		}
		if (anchor_type_dict.has("array")) {
			return MAKE_RESOURCE_TYPE_HINT(AnchorProgram::get_godot_class_hint(anchor_type_dict["array"]));
		}
		if (anchor_type_dict.has("vec")) {
			return MAKE_RESOURCE_TYPE_HINT(AnchorProgram::get_godot_class_hint(anchor_type_dict["vec"]));
		}
		if (anchor_type_dict.has("tuple")) {
			// TODO(Virax): Implement tuple support.
			return "";
		}
		if (anchor_type_dict.has("type")) {
			return get_godot_class_hint(anchor_type_dict["type"]);
		}
	} else if (anchor_type.get_type() == Variant::ARRAY) {
		return "";
	} else if (anchor_type.get_type() == Variant::STRING) {
		const String type_string = anchor_type;
		if (type_string == "publicKey") {
			return "Pubkey";
		}
		return "";
	}
	WARN_PRINT_ONCE_ED("Unknown anchor type");
	return "";
}

PropertyHint AnchorProgram::get_godot_hint(const Variant &anchor_type) {
	if (anchor_type.get_type() == Variant::DICTIONARY) {
		const Dictionary anchor_type_dict = anchor_type;
		if (anchor_type_dict.has("option")) {
			return AnchorProgram::get_godot_hint(anchor_type_dict["option"]);
		}
		if (anchor_type_dict.has("kind") || anchor_type_dict.has("defined")) {
			return PROPERTY_HINT_RESOURCE_TYPE;
		}
		if (anchor_type_dict.has("array")) {
			return PROPERTY_HINT_ARRAY_TYPE;
		}
		if (anchor_type_dict.has("vec")) {
			return PROPERTY_HINT_ARRAY_TYPE;
		}
		if (anchor_type_dict.has("type")) {
			return AnchorProgram::get_godot_hint(anchor_type_dict["type"]);
		}
	} else if (anchor_type.get_type() == Variant::STRING) {
		const String type_string = anchor_type;
		if (type_string == "publicKey") {
			return PROPERTY_HINT_RESOURCE_TYPE;
		}
		return PROPERTY_HINT_NONE;
	}
	WARN_PRINT_ONCE_ED("Unknown anchor type");
	return PROPERTY_HINT_NONE;
}

Variant AnchorProgram::build_argument_dictionary(const Array &arguments, const StringName &instruction_name) const {
	const Dictionary instruction = find_idl_instruction(instruction_name);
	const Array anchor_arguments = instruction["args"];

	Dictionary result;

	ERR_FAIL_COND_V(arguments.size() != anchor_arguments.size(), nullptr);
	for (unsigned int i = 0; i < anchor_arguments.size(); i++) {
		const Dictionary anchor_argument = anchor_arguments[i];
		result[anchor_argument["name"]] = decorate_instruction_argument(anchor_argument["type"], arguments[i]);
	}

	return result;
}

Variant AnchorProgram::build_instruction(String name, Array accounts, Variant arguments) const {
	ERR_FAIL_COND_V_EDMSG(idl.is_empty(), nullptr, "IDL is empty, try loading from PID or JSON file.");
	const Dictionary instruction_info = find_idl_instruction(name);

	ERR_FAIL_COND_V_EDMSG(instruction_info.is_empty(), nullptr, (String("IDL does not contain an instruction named ") + name + ".").ascii());

	if (GenericAnchorNode::has_extra_accounts(get_idl_name(), name)) {
		ERR_FAIL_COND_V_EDMSG(((Array)instruction_info["accounts"]).size() > accounts.size(), nullptr, "Unexpected amount or accounts");
	} else {
		ERR_FAIL_COND_V_EDMSG(((Array)instruction_info["accounts"]).size() != accounts.size(), nullptr, "Unexpected amount or accounts");
	}

	Instruction *result = memnew(Instruction);

	PackedByteArray data;
	if (instruction_info.has("discriminant")) {
		data = evaluate_discriminant(instruction_info["discriminant"]);
	} else {
		data = discriminator_by_name(name.to_snake_case(), global_prefix);
	}

	String use_pid = pid;

	if (idl.has("address") && pid.length() != PUBKEY_LENGTH) {
		use_pid = idl["address"];
	}
	result->set_program_id(Pubkey::new_from_string(use_pid));
	data.append_array(serialize_variant(arguments));

	result->set_data(data);

	Array ref_accounts = instruction_info["accounts"];

	for (unsigned int i = 0; i < ref_accounts.size(); i++) {
		const bool is_optional = detect_optional(ref_accounts[i]);

		const bool writable = detect_writable(ref_accounts[i]);
		const bool is_signer = detect_is_signer(ref_accounts[i]);
		if (is_optional && accounts[i].get_type() == Variant::NIL) {
			// optional accounts are passed in as pid, no signer, immutable
			const Variant pid_key = Pubkey::new_from_string(use_pid);
			result->append_meta(*memnew(AccountMeta(pid_key, false, false)));
			continue;
		}
		result->append_meta(*memnew(AccountMeta(accounts[i], is_signer, writable)));
	}

	// Extra accounts
	for (unsigned int i = ref_accounts.size(); i < accounts.size(); i++) {
		ERR_FAIL_COND_V_EDMSG_CUSTOM(accounts[i].get_type() != Variant::OBJECT, result, "Unknown object");
		ERR_FAIL_COND_V_EDMSG_CUSTOM(!static_cast<Object *>(accounts[i])->is_class("AccountMeta"), result, "Extra Args are supposed to be AccountMetas");
		result->append_meta(*memnew(AccountMeta(accounts[i])));
	}

	return result;
}

Error AnchorProgram::fetch_account(const String name, const Variant &account) {
	if (!pending_account_name.is_empty()) {
		return Error::ERR_ALREADY_IN_USE;
	}
	pending_account_name = name;
	Callable callback(this, "fetch_account_callback");
	fetch_client->connect("http_response_received", callback, ConnectFlags::CONNECT_ONE_SHOT);
	fetch_client->get_account_info(Pubkey::string_from_variant(account));

	return Error::OK;
}

void AnchorProgram::fetch_account_callback(const Dictionary &rpc_result) {
	String name = pending_account_name;
	pending_account_name = "";

	Dictionary ref_struct = find_idl_account(name);

	if (ref_struct.is_empty()) {
		ERR_FAIL_EDMSG("Account name was not found in IDL.");
	}

	if (!rpc_result.has("result")) {
		emit_signal("account_fetched", Dictionary());
		ERR_FAIL_COND_EDMSG(rpc_result.has("error"), String(rpc_result["error"]));
		ERR_FAIL_EDMSG("Unexpected RPC response, no result.");
	}

	Dictionary result_dict = rpc_result["result"];
	if (!result_dict.has("value")) {
		emit_signal("account_fetched", Dictionary());
		ERR_FAIL_EDMSG("Unexpected RPC response, no value.");
	}
	Variant value = result_dict["value"];
	if (value.get_type() != Variant::DICTIONARY) {
		if (value.get_type() == Variant::NIL) {
			emit_signal("account_fetched", Dictionary());
			return;
		}
		emit_signal("account_fetched", Dictionary());
		ERR_FAIL_EDMSG("Unexpected RPC response, unknown value type.");
	}

	Dictionary account_dict = value;
	Array account_data_tuple = account_dict["data"];
	String encoded_data = account_data_tuple[0];
	PackedByteArray account_data = SolanaUtils::bs64_decode(encoded_data);
	emit_signal("account_data_fetched", account_data);
	account_data = account_data.slice(8);

	const Array fields = ((Dictionary)ref_struct["type"])["fields"];

	Dictionary result;
	for (unsigned int i = 0; i < fields.size(); i++) {
		int data_offset = 0;
		const Variant val = deserialize_variant(account_data, ((Dictionary)fields[i])["type"], data_offset);
		result[((Dictionary)fields[i])["name"]] = val;
		account_data = account_data.slice(data_offset);
	}

	emit_signal("account_fetched", result);
}

Error AnchorProgram::fetch_all_accounts(const String &name, const Array &additional_filters) {
	if (!pending_accounts_name.is_empty()) {
		return Error::ERR_ALREADY_IN_USE;
	}
	pending_accounts_name = name;
	Array filters = additional_filters.duplicate();
	Dictionary memcmp_filter;
	memcmp_filter["offset"] = 0;
	memcmp_filter["bytes"] = SolanaUtils::bs58_encode(discriminator_by_name(name, account_prefix));
	Dictionary type_filter;
	type_filter["memcmp"] = memcmp_filter;

	filters.insert(0, type_filter);

	const Callable callback(this, "fetch_all_accounts_callback");
	fetch_client->connect("http_response_received", callback, ConnectFlags::CONNECT_ONE_SHOT);
	fetch_client->get_program_accounts(get_pid(), filters, false);

	return Error::OK;
}

void AnchorProgram::fetch_all_accounts_callback(const Dictionary &rpc_result) {
	const String name = pending_accounts_name;
	pending_accounts_name = "";

	Dictionary ref_struct = find_idl_account(name);

	if (ref_struct.is_empty()) {
		ERR_FAIL_EDMSG_CUSTOM("Account name was not found in IDL.");
	}

	if (!rpc_result.has("result")) {
		emit_signal("accounts_fetched", Dictionary());
		ERR_FAIL_COND_EDMSG_CUSTOM(rpc_result.has("error"), String(rpc_result["error"]));
		ERR_FAIL_EDMSG_CUSTOM("Unexpected RPC response, no result.");
	}

	if (rpc_result["result"].get_type() != Variant::ARRAY) {
		emit_signal("accounts_fetched", Dictionary());
		ERR_FAIL_EDMSG_CUSTOM("Unexpected RPC response, result is not an array.");
	}

	Array accounts = rpc_result["result"];
	Dictionary result_accounts;
	const Array fields = static_cast<Dictionary>(ref_struct["type"])["fields"];
	for (int i = 0; i < accounts.size(); i++) {
		Dictionary account_entry = accounts[i];
		const String pubkey = account_entry["pubkey"];
		Dictionary account_data = account_entry["account"];

		Array data_tuple = account_data["data"];
		const String encoded_data = data_tuple[0];

		PackedByteArray account_bytes = SolanaUtils::bs64_decode(encoded_data);
		account_bytes = account_bytes.slice(DISCRIMINATOR_LENGTH);

		Dictionary parsed_account;
		for (int j = 0; j < fields.size(); j++) {
			int data_offset = 0;
			const Variant val = deserialize_variant(account_bytes, static_cast<Dictionary>(fields[j])["type"], data_offset);
			parsed_account[static_cast<Dictionary>(fields[j])["name"]] = val;
			account_bytes = account_bytes.slice(data_offset);
		}

		result_accounts[pubkey] = parsed_account;
	}

	emit_signal("accounts_fetched", result_accounts);
}

} //namespace godot