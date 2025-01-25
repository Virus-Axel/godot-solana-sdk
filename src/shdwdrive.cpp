#include "shdwdrive.hpp"

#include <cstdint>
#include <string>

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/http_client.hpp"
#include "godot_cpp/classes/http_request.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "sha256.hpp"

#include "account_meta.hpp"
#include "anchor_program.hpp"
#include "hash.hpp"
#include "instruction.hpp"
#include "keypair.hpp"
#include "pubkey.hpp"
#include "solana_client.hpp"
#include "solana_utils.hpp"
#include "spl_token.hpp"
#include "system_program.hpp"
#include "transaction.hpp"

namespace godot {

const std::string ShdwDrive::PID = "2e1wdyNhUvE76y6yUCvah2KaviavMJYKoRun8acMRBZZ";

PackedByteArray StorageAccountV2::discriminator() {
	return PackedByteArray({ 133, 53, 253, 82, 212, 5, 201, 218 }); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void StorageAccountV2::_bind_methods() {
}

void StorageAccountV2::from_bytes(const PackedByteArray &bytes) {
	const int64_t MINIMUM_STORAGE_ACCOUNT_SIZE = 66;
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.size() < MINIMUM_STORAGE_ACCOUNT_SIZE, "Invalid Storage Account");
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.slice(0, DISCRIMINATOR_LENGTH) != discriminator(), "Account is not StorageAccountV2.");

	int offset = DISCRIMINATOR_LENGTH;
	immutable = bytes[offset++] != 0U;
	to_be_deleted = bytes[offset++] != 0U;
	delete_request_epoch = bytes.decode_u32(offset);
	offset += sizeof(uint32_t);
	storage = bytes.decode_u64(offset);
	offset += sizeof(uint64_t);
	owner1 = Pubkey::new_from_bytes(bytes.slice(offset, offset + PUBKEY_LENGTH));
	offset += PUBKEY_LENGTH;
	account_counter_seed = bytes.decode_u32(offset);
	offset += sizeof(uint32_t);
	creation_time = bytes.decode_u32(offset);
	offset += sizeof(uint32_t);
	creation_epoch = bytes.decode_u32(offset);
	offset += sizeof(uint32_t);
	last_fee_epoch = bytes.decode_u32(offset);
	offset += sizeof(uint32_t);
	const uint32_t identifier_size = bytes.decode_u32(offset);
	offset += sizeof(uint32_t);
	const PackedByteArray identifier_bytes = bytes.slice(offset);

	ERR_FAIL_COND_EDMSG_CUSTOM(identifier_bytes.size() < identifier_size, "Invalid Storage Account");
	identifier = identifier_bytes.get_string_from_ascii();
}

String StorageAccountV2::get_identifier() {
	return identifier;
}

Dictionary StorageAccountV2::to_dict() {
	Dictionary result;
	result["immutable"] = immutable;
	result["to_be_deleted"] = to_be_deleted;
	result["delete_request_epoch"] = delete_request_epoch;
	result["storage"] = storage;
	result["owner1"] = owner1;
	result["account_counter_seed"] = account_counter_seed;
	result["creation_time"] = creation_time;
	result["creation_epoch"] = creation_epoch;
	result["last_fee_epoch"] = last_fee_epoch;
	result["identifier"] = identifier;

	return result;
}

PackedByteArray UserInfo::discriminator() {
	return PackedByteArray({ 83, 134, 200, 56, 144, 56, 10, 62 }); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void UserInfo::_bind_methods() {
	ClassDB::bind_method(D_METHOD("from_bytes", "bytes"), &UserInfo::from_bytes);
}

uint32_t UserInfo::get_account_counter() const {
	return account_counter;
}

void UserInfo::from_bytes(const PackedByteArray &bytes) {
	const int64_t EXPECTED_ACCOUNT_SIZE = 18;
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.size() != EXPECTED_ACCOUNT_SIZE, "Invalid user account size.");
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.slice(0, 8) != discriminator(), "Account is not UserInfo.");

	account_counter = bytes.decode_u32(DISCRIMINATOR_LENGTH);
	delete_counter = bytes.decode_u32(DISCRIMINATOR_LENGTH + sizeof(uint32_t));

	const uint8_t AGREED_TO_TERMS_OFFSET = 16;
	const uint8_t HAD_BAD_SCAM_OFFSET = 17;
	agreed_to_terms = bytes[AGREED_TO_TERMS_OFFSET] == 1;
	had_bad_scam_scan = bytes[HAD_BAD_SCAM_OFFSET] == 1;
}

ShdwDrive::ShdwDrive() {
	api_request->set_skip_id(true);

	create_storage_account_client->set_async_override(true);

	fetch_user_info_client->set_async_override(true);
	const Callable fetch_user_info_callable = Callable(this, "fetch_userinfo_callback");
	fetch_user_info_client->set_callback(fetch_user_info_callable);

	fetch_storage_account_client->set_async_override(true);
	const Callable fetch_storage_account_callable = Callable(this, "fetch_storage_account_callback");
	fetch_storage_account_client->set_callback(fetch_storage_account_callable);

	fetch_all_storage_accounts_client->set_async_override(true);
}

void ShdwDrive::_process(double delta) {
	api_request->process(delta);
	create_storage_account_transaction->_process(delta);
	fetch_storage_account_client->_process(delta);
	fetch_user_info_client->_process(delta);
	fetch_all_storage_accounts_client->_process(delta);
}

void ShdwDrive::send_fetch_account_infos() {
	ERR_FAIL_COND_EDMSG_CUSTOM(user_info == nullptr, "User info is not fetched.");

	const uint32_t owned_accounts = user_info->get_account_counter();
	PackedStringArray accounts;

	storage_name_list.clear();
	storage_key_list.clear();
	for (uint64_t i = 0; i < owned_accounts; i++) {
		const Variant storage_account = new_storage_account_pubkey(owner_keypair, i);
		accounts.append(Pubkey::string_from_variant(storage_account));

		storage_key_list.append(Pubkey::string_from_variant(storage_account));
	}

	const Callable callback = Callable(this, "get_multiple_accounts_callback");

	fetch_all_storage_accounts_client->connect("http_response_received", callback, CONNECT_ONE_SHOT);
	fetch_all_storage_accounts_client->get_multiple_accounts(accounts);
}

void ShdwDrive::get_multiple_accounts_callback(const Dictionary &response) {
	ERR_FAIL_COND_EDMSG_CUSTOM(!response.has("result"), "Fetching storage account failed.");
	ERR_FAIL_COND_EDMSG_CUSTOM(!static_cast<Dictionary>(response["result"]).has("value"), "Getting storage accounts failed.");

	Array accounts = static_cast<Dictionary>(response["result"])["value"];

	storage_name_list.resize(storage_key_list.size());
	storage_list.resize(storage_key_list.size());

	for (uint32_t i = 0; i < accounts.size(); i++) {
		Dictionary account = accounts[i];
		StorageAccountV2 *storage_account = memnew_custom(StorageAccountV2);
		const String encoded_data = static_cast<Array>(account["data"])[0];
		storage_account->from_bytes(SolanaUtils::bs64_decode(encoded_data));

		const String identifier = storage_account->get_identifier();
		storage_name_list[i] = identifier;
		storage_list[i] = storage_account;
	}
	emit_signal("all_storage_accounts_fetched");
}

Array ShdwDrive::get_cached_storage_accounts() {
	Array result;
	for (unsigned int i = 0; i < storage_list.size(); i++) {
		Dictionary dict = Object::cast_to<StorageAccountV2>(storage_list[i])->to_dict();
		dict["address"] = storage_key_list[i];
		result.append(dict);
	}
	return result;
}

Variant ShdwDrive::fetch_user_info(const Variant &address) {
	fetch_user_info_client->get_account_info(Pubkey::string_from_variant(address));
	return Error::OK;
}

Variant ShdwDrive::fetch_storage_account(const Variant &address) {
	fetch_storage_account_client->get_account_info(Pubkey::string_from_variant(address));
	return Error::OK;
}

PackedByteArray ShdwDrive::initialize_accountv2_discriminator() {
	return PackedByteArray({ 8, 182, 149, 144, 185, 31, 209, 105 }); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

uint64_t ShdwDrive::human_size_to_bytes(const String &human_size) {
	const int64_t KILO = 1000;
	const int64_t MEGA = 1000000;
	const int64_t GIGA = 1000000000;
	if (human_size.ends_with("kb") || human_size.ends_with("KB")) {
		return human_size.substr(0, human_size.length() - 2).to_int() * KILO;
	}
	if (human_size.ends_with("mb") || human_size.ends_with("MB")) {
		return human_size.substr(0, human_size.length() - 2).to_int() * MEGA;
	}
	if (human_size.ends_with("gb") || human_size.ends_with("GB")) {
		return human_size.substr(0, human_size.length() - 2).to_int() * GIGA;
	}
	ERR_FAIL_V_EDMSG_CUSTOM(0, "Example input 10000, 10KB, 300mb, 1GB");
}

Variant ShdwDrive::get_uploader() {
	return Pubkey::new_from_string("972oJTFyjmVNsWM4GHEGPWUomAiJf2qrVotLtwnKmWem");
}

void ShdwDrive::get_all_storage_accounts(const Variant &owner_key) {
	const unsigned int MAX_ACCOUNTS_PER_RPC_CALL = 100;
	owner_keypair = owner_key;

	const Callable callback = Callable(this, "send_fetch_account_infos");

	connect("user_info_fetched", callback, CONNECT_ONE_SHOT);
	//connect("storage_account_fetched", callback, CONNECT_ONE_SHOT);

	fetch_user_info(new_user_info_pubkey(owner_keypair));

	//fetch_all_storage_accounts_client->enable_account_filter()
}

void ShdwDrive::fetch_userinfo_callback(const Dictionary &params) {
	// TODO(Virax): Add helper for this common pattern.
	if (!params.has("result")) {
		ERR_PRINT_ED_CUSTOM("Fetching user info account failed.");
		return;
	}
	if (!static_cast<Dictionary>(params["result"]).has("value")) {
		ERR_PRINT_ED_CUSTOM("User is not init");
		return;
	}

	Dictionary account = static_cast<Dictionary>(params["result"])["value"];

	if (account.has("data")) {
		Array encoded_data = account["data"];
		const PackedByteArray data_info = SolanaUtils::bs64_decode(encoded_data[0]);

		user_info->from_bytes(data_info);
	} else {
		new_user = true;
	}

	emit_signal("user_info_fetched");
}

void ShdwDrive::fetch_storage_account_callback(const Dictionary &params) {
	// TODO(Virax): Add helper for this common pattern.
	if (!params.has("result")) {
		ERR_PRINT_ED_CUSTOM("Fetching storage account failed.");
		return;
	}
	if (!static_cast<Dictionary>(params["result"]).has("value")) {
		return;
	}

	Dictionary account = static_cast<Dictionary>(params["result"])["value"];
	const PackedByteArray data_info = SolanaUtils::bs64_decode(account["data"]);

	StorageAccountV2 *storage_account = memnew_custom(StorageAccountV2);
	storage_account->from_bytes(data_info);

	emit_signal("storage_account_fetched", storage_account);
}

void ShdwDrive::_bind_methods() {
	ClassDB::add_signal("ShdwDrive", MethodInfo("user_info_fetched", PropertyInfo(Variant::OBJECT, "user_info")));
	ClassDB::add_signal("ShdwDrive", MethodInfo("storage_account_fetched", PropertyInfo(Variant::OBJECT, "storage_account")));
	ClassDB::add_signal("ShdwDrive", MethodInfo("storage_account_response", PropertyInfo(Variant::DICTIONARY, "response")));
	ClassDB::add_signal("ShdwDrive", MethodInfo("upload_response", PropertyInfo(Variant::DICTIONARY, "response")));
	ClassDB::add_signal("ShdwDrive", MethodInfo("all_storage_accounts_fetched"));

	ClassDB::bind_static_method("ShdwDrive", D_METHOD("new_user_info_pubkey", "base_key"), &ShdwDrive::new_user_info_pubkey);
	ClassDB::bind_static_method("ShdwDrive", D_METHOD("new_storage_config_pubkey"), &ShdwDrive::new_storage_config_pubkey);
	ClassDB::bind_static_method("ShdwDrive", D_METHOD("new_stake_account_pubkey", "base_key"), &ShdwDrive::new_stake_account_pubkey);
	ClassDB::bind_static_method("ShdwDrive", D_METHOD("new_storage_account_pubkey", "base_key", "account_seed"), &ShdwDrive::new_storage_account_pubkey);

	ClassDB::bind_method(D_METHOD("send_fetch_account_infos"), &ShdwDrive::send_fetch_account_infos);
	ClassDB::bind_method(D_METHOD("get_all_storage_accounts", "owner_key"), &ShdwDrive::get_all_storage_accounts);
	ClassDB::bind_method(D_METHOD("fetch_userinfo_callback", "params"), &ShdwDrive::fetch_userinfo_callback);
	ClassDB::bind_method(D_METHOD("get_multiple_accounts_callback"), &ShdwDrive::get_multiple_accounts_callback);
	ClassDB::bind_method(D_METHOD("upload_file_callback", "result", "response_code", "headers", "body"), &ShdwDrive::upload_file_callback);
	ClassDB::bind_method(D_METHOD("fetch_storage_account_callback", "params"), &ShdwDrive::fetch_storage_account_callback);
	ClassDB::bind_method(D_METHOD("create_storage_account", "owner_keypair", "storage_name", "storage_size"), &ShdwDrive::create_storage_account);
	ClassDB::bind_method(D_METHOD("send_create_storage_tx"), &ShdwDrive::send_create_storage_tx);
	ClassDB::bind_method(D_METHOD("send_create_storage_tx_signed"), &ShdwDrive::send_create_storage_tx_signed);
	ClassDB::bind_method(D_METHOD("create_store_api_response", "response"), &ShdwDrive::create_store_api_response);
	ClassDB::bind_method(D_METHOD("fetch_storage_key_by_name", "storage_owner", "storage_name"), &ShdwDrive::fetch_storage_key_by_name);
	ClassDB::bind_method(D_METHOD("fetch_storage_key_by_name_callback", "storage_name"), &ShdwDrive::fetch_storage_key_by_name_callback);
	ClassDB::bind_method(D_METHOD("get_cached_storage_accounts"), &ShdwDrive::get_cached_storage_accounts);

	ClassDB::bind_method(D_METHOD("upload_file_to_storage", "filename", "storage_owner_keypair", "storage_account"), &ShdwDrive::upload_file_to_storage);
}

Variant ShdwDrive::create_storage_account(const Variant &owner_keypair, const String &name, const Variant &size) { // NOLINT(bugprone-easily-swappable-parameters)
	this->owner_keypair = owner_keypair;
	this->storage_name = name;
	if (size.get_type() == Variant::INT) {
		this->storage_size = size;
	} else if (size.get_type() == Variant::STRING) {
		this->storage_size = human_size_to_bytes(size);
	} else {
		ERR_FAIL_V_EDMSG_CUSTOM(nullptr, "Invalid storage input size");
	}

	const Callable callback = Callable(this, "send_create_storage_tx");

	connect("user_info_fetched", callback, CONNECT_ONE_SHOT);
	connect("storage_account_fetched", callback, CONNECT_ONE_SHOT);

	fetch_user_info(new_user_info_pubkey(owner_keypair));

	return nullptr;
}

Variant ShdwDrive::fetch_storage_key_by_name_callback(const String &storage_name) {
	ERR_FAIL_COND_V_EDMSG_CUSTOM(storage_key_list.size() != storage_name_list.size(), nullptr, "Internal Error when fetching storages.");
	const int64_t index = storage_name_list.find(storage_name);
	if (index >= 0) {
		const Variant storage_account_pk = Pubkey::new_from_string(storage_key_list[index]);
		emit_signal("storage_account_fetched", storage_account_pk);
		return storage_key_list[index];
	}
	emit_signal("storage_account_fetched", nullptr);
	return nullptr;
}

Variant ShdwDrive::fetch_storage_key_by_name(const Variant &owner_keypair, const String &storage_name) { // NOLINT(bugprone-easily-swappable-parameters)
	ERR_FAIL_COND_V_EDMSG_CUSTOM(storage_key_list.size() != storage_name_list.size(), nullptr, "Internal Error when fetching storages.");
	const int64_t index = storage_name_list.find(storage_name);

	if (index >= 0) {
		const Variant storage_account_pk = Pubkey::new_from_string(storage_key_list[index]);
		emit_signal("storage_account_fetched", storage_account_pk);
		return storage_key_list[index];
	}

	connect("all_storage_accounts_fetched", Callable(this, "fetch_storage_key_by_name_callback").bind(storage_name));
	get_all_storage_accounts(owner_keypair);
	return nullptr;
}

void ShdwDrive::send_create_storage_tx() {
	ERR_FAIL_COND_CUSTOM((user_info == nullptr) && (new_user == false));

	const Variant instruction = initialize_account({ .owner_keypair = owner_keypair, .name = this->storage_name, .storage = this->storage_size });

	create_storage_account_transaction->set_payer(owner_keypair);
	create_storage_account_transaction->add_instruction(instruction);
	create_storage_account_transaction->update_latest_blockhash();

	Array partially_signers;
	partially_signers.append(owner_keypair);

	const Callable callback = Callable(this, "send_create_storage_tx_signed");
	create_storage_account_transaction->connect("signer_state_changed", callback, CONNECT_ONE_SHOT);
	create_storage_account_transaction->partially_sign(partially_signers);
}

void ShdwDrive::send_create_storage_tx_signed() {
	const PackedByteArray ser = create_storage_account_transaction->serialize();

	const String SHDW_DRIVE_ENDPOINT = "https://shadow-storage.genesysgo.net:443/storage-account";

	PackedStringArray http_headers;
	http_headers.append("Content-Type: application/json");
	http_headers.append("Accept-Encoding: json");

	Dictionary dict;
	dict["transaction"] = SolanaUtils::bs64_encode(ser);

	const Dictionary url = SolanaClient::parse_url(SHDW_DRIVE_ENDPOINT);

	const Callable callback = Callable(this, "create_store_api_response");
	api_request->asynchronous_request(dict, url, callback);
}

void ShdwDrive::create_store_api_response(const Dictionary &response) {
	emit_signal("storage_account_response", response);
}

Variant ShdwDrive::new_user_info_pubkey(const Variant &base_keypair) {
	Array seeds;
	seeds.append(String("user-info").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(base_keypair));
	return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::new_storage_config_pubkey() {
	Array seeds;
	seeds.append(String("storage-config").to_ascii_buffer());
	return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::new_stake_account_pubkey(const Variant &storage_account) {
	Array seeds;
	seeds.append(String("stake-account").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(storage_account));
	return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::new_storage_account_pubkey(const Variant &owner_key, uint64_t account_seed) { // NOLINT(bugprone-easily-swappable-parameters)
	Array seeds;
	seeds.append(String("storage-account").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(owner_key));
	PackedByteArray account_seed_buffer;
	account_seed_buffer.resize(4);
	account_seed_buffer.encode_u32(0, static_cast<int64_t>(account_seed));
	seeds.append(account_seed_buffer);

	return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::initialize_account(const InitParams &initialize_params) {
	Instruction *result = memnew_custom(Instruction);

	PackedByteArray data = initialize_accountv2_discriminator();
	const int32_t INITIALIZE_ACCOUNT_DATA_SIZE = 12;
	const int32_t DISCRIMINATOR_LENGTH = 12;

	data.resize(INITIALIZE_ACCOUNT_DATA_SIZE);
	data.encode_u32(DISCRIMINATOR_LENGTH, initialize_params.name.length());
	data.append_array(initialize_params.name.to_ascii_buffer());
	PackedByteArray storage_bytes;
	storage_bytes.resize(sizeof(uint64_t));
	storage_bytes.encode_u64(0, static_cast<int64_t>(initialize_params.storage));
	data.append_array(storage_bytes);

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	const Variant TOKEN_MINT = Pubkey::new_from_string("SHDWyBxihqiCj6YekG2GUr7wqKLeLAMK1gHZck9pL6y");
	const Variant ata_account = Pubkey::new_associated_token_address(owner_keypair, TOKEN_MINT);
	const Variant STORAGE_CONFIG = new_storage_config_pubkey();
	const Variant USER_INFO_PUBKEY = new_user_info_pubkey(owner_keypair);

	uint64_t account_seed = 0;
	if (!new_user) {
		account_seed = user_info->get_account_counter();
	}
	const Variant STORAGE_ACCOUNT = new_storage_account_pubkey(owner_keypair, account_seed);
	const Variant STAKE_ACCOUNT = new_stake_account_pubkey(STORAGE_ACCOUNT);
	const Variant UPLOADER = get_uploader();

	result->append_meta(*memnew_custom(AccountMeta(STORAGE_CONFIG, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(USER_INFO_PUBKEY, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(STORAGE_ACCOUNT, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(STAKE_ACCOUNT, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(TOKEN_MINT, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(owner_keypair, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(UPLOADER, true, false)));
	result->append_meta(*memnew_custom(AccountMeta(ata_account, false, true)));

	const Variant SYSTEM_PROGRAM_PID = SystemProgram::get_pid();
	const Variant TOKEN_PID = TokenProgram::get_pid();
	const Variant SYSVAR_RENT = Pubkey::new_from_string("SysvarRent111111111111111111111111111111111");

	result->append_meta(*memnew_custom(AccountMeta(SYSTEM_PROGRAM_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(TOKEN_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(SYSVAR_RENT, false, false)));

	return result;
}

PackedByteArray ShdwDrive::create_form_line(const String &line) {
	PackedByteArray result;
	result.append_array(line.to_utf8_buffer());
	result.append_array(String("\r\n").to_utf8_buffer());
	return result;
}

PackedByteArray ShdwDrive::create_form_line(const PackedByteArray &content) {
	PackedByteArray result;
	result.append_array(content);
	result.append_array(String("\r\n").to_utf8_buffer());
	return result;
}

String ShdwDrive::get_upload_message(const UploadParams &upload_params) {
	Array arr;
	arr.append(Pubkey::string_from_variant(upload_params.account_key));
	arr.append(upload_params.filename_hash);
	return String("Shadow Drive Signed Message:\nStorage Account: {0}\nUpload files with hash: {1}").format(arr);
}

String ShdwDrive::get_filename_hash(const String &filename) {
	SHA256 hasher;
	const PackedByteArray filename_bytes = filename.to_ascii_buffer();
	hasher.update(filename_bytes.ptr(), filename_bytes.size());
	const uint8_t *sha256_hash = hasher.digest();

	PackedByteArray res;
	res.resize(HASH_LENGTH);

	// NOLINTBEGIN(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-bounds-pointer-arithmetic)
	for (unsigned int i = 0; i < HASH_LENGTH; i++) {
		res[i] = sha256_hash[i];
	}
	delete[] sha256_hash;
	// NOLINTEND(cppcoreguidelines-owning-memory,cppcoreguidelines-pro-bounds-pointer-arithmetic)

	return res.hex_encode();
}

void ShdwDrive::upload_file_to_storage(const String &filename, const Variant &storage_owner_keypair, const Variant &storage_account) { // NOLINT(bugprone-easily-swappable-parameters)
	const String boundary = "--GODOTSOLANASDKBOUNDARY";

	const PackedByteArray file_content = FileAccess::get_file_as_bytes(filename);

	ERR_FAIL_COND_EDMSG_CUSTOM(file_content.is_empty(), "Failed to read file contents.");

	PackedStringArray splits = filename.split("/", false);

	String name_without_path = filename;
	if (splits.size() > 1) {
		name_without_path = splits[splits.size() - 1];
	}

	const String filename_hash = get_filename_hash(name_without_path);

	const String upload_message = get_upload_message({ .account_key = storage_account, .filename_hash = filename_hash });

	const PackedByteArray signature = Object::cast_to<Keypair>(storage_owner_keypair)->sign_message(upload_message.to_ascii_buffer());

	PackedByteArray request_body;
	request_body.append_array(create_form_line(boundary));
	request_body.append_array(create_form_line(R"(Content-Disposition: form-data; name="file"; filename=")" + name_without_path + "\""));
	request_body.append_array(create_form_line("Content-Type: application/octet-stream"));
	request_body.append_array(create_form_line(""));
	request_body.append_array(create_form_line(file_content));
	request_body.append_array(create_form_line(boundary));
	request_body.append_array(create_form_line("Content-Disposition: form-data; name=\"message\""));
	request_body.append_array(create_form_line(""));
	request_body.append_array(create_form_line(SolanaUtils::bs58_encode(signature)));
	request_body.append_array(create_form_line(boundary));
	request_body.append_array(create_form_line("Content-Disposition: form-data; name=\"signer\""));
	request_body.append_array(create_form_line(""));
	request_body.append_array(create_form_line(Pubkey::string_from_variant(storage_owner_keypair)));
	request_body.append_array(create_form_line(boundary));
	request_body.append_array(create_form_line("Content-Disposition: form-data; name=\"storage_account\""));
	request_body.append_array(create_form_line(""));
	request_body.append_array(create_form_line(Pubkey::string_from_variant(storage_account)));
	request_body.append_array(create_form_line(boundary));
	request_body.append_array(create_form_line("Content-Disposition: form-data; name=\"fileNames\""));
	request_body.append_array(create_form_line(""));
	request_body.append_array(create_form_line(name_without_path));
	request_body.append_array(create_form_line(boundary + String("--")));

	PackedStringArray headers;
	headers.append(String("Content-Type: multipart/form-data;boundary=") + boundary.substr(2));
	/// @cond
	// Disabling doxygen due to misinterpretation on the header.
	headers.append(String("accept: */*"));
	/// @endcond
	headers.append("accept-encoding: gzip, br, deflate");
	headers.append(String("content-length: ") + String::num_int64(request_body.size()));

	const String SHDW_DRIVE_ENDPOINT = "https://shadow-storage.genesysgo.net:443/upload";

	add_child(upload_file_request, false, INTERNAL_MODE_BACK);

	const Callable upload_file_response = Callable(this, "upload_file_callback");
	upload_file_request->connect("request_completed", upload_file_response, CONNECT_ONE_SHOT);

	upload_file_request->request_raw(SHDW_DRIVE_ENDPOINT, headers, HTTPClient::METHOD_POST, request_body);
}

void ShdwDrive::upload_file_callback(int result, int response_code, const PackedStringArray &headers, const PackedByteArray &body) { // NOLINT(misc-unused-parameters)
	if (response_code != HTTPClient::RESPONSE_OK || result != HTTPRequest::RESULT_SUCCESS) {
		WARN_PRINT_ONCE_ED_CUSTOM("Upload request failed");
		return;
	}
	const Dictionary response = JSON::parse_string(body.get_string_from_ascii());
	emit_signal("upload_response", response);
}

Variant ShdwDrive::get_pid() {
	return Pubkey::new_from_string(PID.c_str());
}

ShdwDrive::~ShdwDrive() {
	memfree(api_request);
	memfree(user_info);
	memfree(create_storage_account_client);
	memfree(create_storage_account_transaction);
	memfree(fetch_user_info_client);
	memfree(fetch_storage_account_client);
}

} //namespace godot
