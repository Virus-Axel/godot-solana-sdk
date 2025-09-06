#include "meta_data/meta_data.hpp"

#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"

#include "creator.hpp"
#include "meta_data/collection.hpp"
#include "meta_data/uses.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

void MetaData::copy_grouping(const Dictionary &dict) {
	ERR_FAIL_COND_CUSTOM(!dict.has("grouping"));
	const Array groups = dict["grouping"];
	for (unsigned int i = 0; i < groups.size(); i++) {
		const Dictionary group = groups[i];
		if (group.has("group_key")) {
			if (group["group_key"] == "collection" && group.has("group_value")) {
				const Variant new_collection = memnew_custom(MetaDataCollection);
				const Variant collection_key = Pubkey::new_from_string(group["group_value"]);
				Object::cast_to<MetaDataCollection>(new_collection)->set_key(collection_key);

				set_collection(new_collection);
			}
		}
	}
}

void MetaData::copy_creators(const Dictionary &dict) {
	ERR_FAIL_COND_CUSTOM(!dict.has("creators"));
	const Array creator_array = dict["creators"];
	for (unsigned int i = 0; i < creator_array.size(); i++) {
		Dictionary creator = creator_array[i];
		const Variant metadata_creator = memnew_custom(MetaDataCreator);

		if (creator.has("address")) {
			Object::cast_to<MetaDataCreator>(metadata_creator)->set_address(creator["address"]);
		}
		if (creator.has("address")) {
			Object::cast_to<MetaDataCreator>(metadata_creator)->set_share(creator["share"]);
		}
		if (creator.has("address")) {
			Object::cast_to<MetaDataCreator>(metadata_creator)->set_verified(creator["verified"]);
		}

		add_creator(metadata_creator);
	}
	enable_creators = creators.size() > 0;
}

bool MetaData::_set(const StringName &p_name, const Variant &p_value) {
	const String property_name = p_name;
	if (property_name == "name") {
		name = p_value;
	} else if (property_name == "symbol") {
		symbol = p_value;
	} else if (property_name == "uri") {
		uri = p_value;
	} else if (property_name == "seller_fee_basis_points") {
		seller_fee_basis_points = static_cast<uint32_t>(p_value);
	} else if (property_name == "enable_creators") {
		enable_creators = p_value;
		notify_property_list_changed();
	} else if (property_name == "enable_collection") {
		enable_collection = p_value;
		notify_property_list_changed();
	} else if (property_name == "enable_uses") {
		enable_uses = p_value;
		notify_property_list_changed();
	} else if (property_name == "enable_collection_details") {
		enable_collection_details = p_value;
		notify_property_list_changed();
	} else if (property_name == "creators") {
		creators = p_value;
	} else if (property_name == "collection") {
		collection = p_value;
	} else if (property_name == "uses") {
		uses = p_value;
	} else if (property_name == "collection_size") {
		collection_size = p_value;
	} else {
		return false;
	}
	return true;
}

bool MetaData::_get(const StringName &p_name, Variant &r_ret) const {
	const String property_name = p_name;
	if (property_name == "name") {
		r_ret = name;
	} else if (property_name == "symbol") {
		r_ret = symbol;
	} else if (property_name == "uri") {
		r_ret = uri;
	} else if (property_name == "seller_fee_basis_points") {
		r_ret = seller_fee_basis_points;
	} else if (property_name == "enable_creators") {
		r_ret = enable_creators;
	} else if (property_name == "enable_collection") {
		r_ret = enable_collection;
	} else if (property_name == "enable_uses") {
		r_ret = enable_uses;
	} else if (property_name == "enable_collection_details") {
		r_ret = enable_collection_details;
	} else if (property_name == "creators") {
		r_ret = creators;
	} else if (property_name == "collection") {
		r_ret = collection;
	} else if (property_name == "uses") {
		r_ret = uses;
	} else if (property_name == "collection_size") {
		r_ret = collection_size;
	} else {
		return false;
	}
	return true;
}

void MetaData::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_token_name"), &MetaData::get_token_name);
	ClassDB::bind_method(D_METHOD("set_token_name", "token_name"), &MetaData::set_token_name);
	ClassDB::bind_method(D_METHOD("get_symbol"), &MetaData::get_symbol);
	ClassDB::bind_method(D_METHOD("set_symbol", "symbol"), &MetaData::set_symbol);
	ClassDB::bind_method(D_METHOD("get_uri"), &MetaData::get_uri);
	ClassDB::bind_method(D_METHOD("set_uri", "uri"), &MetaData::set_uri);

	ClassDB::bind_method(D_METHOD("get_creators"), &MetaData::get_creators);
	ClassDB::bind_method(D_METHOD("get_collection"), &MetaData::get_collection);

	ClassDB::bind_method(D_METHOD("get_primary_sale_happened"), &MetaData::get_primary_sale_happened);
	ClassDB::bind_method(D_METHOD("set_primary_sale_happened", "primary_sale_happened"), &MetaData::set_primary_sale_happened);
	ClassDB::bind_method(D_METHOD("get_is_mutable"), &MetaData::get_is_mutable);
	ClassDB::bind_method(D_METHOD("set_is_mutable", "is_mutable"), &MetaData::set_is_mutable);
	ClassDB::bind_method(D_METHOD("get_mint"), &MetaData::get_mint);
	ClassDB::bind_method(D_METHOD("set_mint", "mint"), &MetaData::set_mint);
	ClassDB::bind_method(D_METHOD("get_update_authority"), &MetaData::get_update_authority);
	ClassDB::bind_method(D_METHOD("set_update_authority", "update_authority"), &MetaData::set_update_authority);
	ClassDB::bind_method(D_METHOD("get_seller_fee_basis_points"), &MetaData::get_seller_fee_basis_points);
	//ClassDB::bind_method(D_METHOD("set_seller_fee_basis_points", "set_seller_fee_basis_points"), &MetaData::set_seller_fee_basis_points);

	ClassDB::bind_method(D_METHOD("copy_from_dict", "dictionary"), &MetaData::copy_from_dict);

	ClassDB::bind_method(D_METHOD("get_edition_nonce"), &MetaData::get_edition_nonce);
	ClassDB::bind_method(D_METHOD("set_edition_nonce", "edition_nonce"), &MetaData::set_edition_nonce);
	ClassDB::bind_method(D_METHOD("get_token_standard"), &MetaData::get_token_standard);
	ClassDB::bind_method(D_METHOD("set_token_standard", "token_standard"), &MetaData::set_token_standard);
	ClassDB::bind_method(D_METHOD("get_collection_details"), &MetaData::get_collection_details);
	ClassDB::bind_method(D_METHOD("set_collection_details", "collection_detail"), &MetaData::set_collection_details);
	ClassDB::bind_method(D_METHOD("get_programmable_config"), &MetaData::get_programmable_config);
	ClassDB::bind_method(D_METHOD("set_programmable_config", "programmable_config"), &MetaData::set_programmable_config);
}

void MetaData::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::STRING, "name"));
	p_list->push_back(PropertyInfo(Variant::STRING, "symbol"));
	p_list->push_back(PropertyInfo(Variant::STRING, "uri"));

	p_list->push_back(PropertyInfo(Variant::INT, "seller_fee_basis_points"));

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_creators"));
	if (enable_creators) {
		p_list->push_back(PropertyInfo(Variant::ARRAY, "creators", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("MetaDataCreator")));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_collection"));
	if (enable_collection) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "collection", PROPERTY_HINT_RESOURCE_TYPE, "MetaDataCollection"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_uses"));
	if (enable_uses) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "uses", PROPERTY_HINT_RESOURCE_TYPE, "MetaDataUses"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_collection_details"));
	if (enable_collection_details) {
		p_list->push_back(PropertyInfo(Variant::INT, "collection_size", PROPERTY_HINT_NONE));
	}

	p_list->push_back(PropertyInfo(Variant::OBJECT, "mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey"));
	p_list->push_back(PropertyInfo(Variant::OBJECT, "update_authority", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey"));

	p_list->push_back(PropertyInfo(Variant::INT, "edition_nonce"));
	p_list->push_back(PropertyInfo(Variant::INT, "token_standard"));
	p_list->push_back(PropertyInfo(Variant::INT, "collection_detail"));
	p_list->push_back(PropertyInfo(Variant::INT, "programmable_config"));
}

void MetaData::set_token_name(const String &token_name) {
	name = token_name;
}
String MetaData::get_token_name() const {
	return name;
}
void MetaData::set_symbol(const String &symbol) {
	this->symbol = symbol;
}
String MetaData::get_symbol() const {
	return symbol;
}
void MetaData::set_uri(const String &uri) {
	this->uri = uri;
}
String MetaData::get_uri() const {
	return uri;
}
void MetaData::set_seller_fee_basis_points(const uint16_t seller_fee_basis_points) {
	this->seller_fee_basis_points = seller_fee_basis_points;
}
uint16_t MetaData::get_seller_fee_basis_points() const {
	return seller_fee_basis_points;
}

void MetaData::set_primary_sale_happened(bool primary_sale_happened) {
	this->primary_sale_happened = primary_sale_happened;
}
bool MetaData::get_primary_sale_happened() const {
	return primary_sale_happened;
}

void MetaData::set_is_mutable(bool is_mutable) {
	this->is_mutable = is_mutable;
}
bool MetaData::get_is_mutable() const {
	return is_mutable;
}

void MetaData::set_mint(const Variant &mint) {
	this->mint = mint;
}

Variant MetaData::get_mint() const {
	return mint;
}

void MetaData::set_update_authority(const Variant &update_authority) {
	this->update_authority = update_authority;
}

Variant MetaData::get_update_authority() const {
	return update_authority;
}

void MetaData::set_edition_nonce(const Variant &edition_nonce) {
	this->edition_nonce = edition_nonce;
}

Variant MetaData::get_edition_nonce() const {
	return edition_nonce;
}

void MetaData::set_token_standard(const Variant &token_standard) {
	this->token_standard = token_standard;
}

Variant MetaData::get_token_standard() const {
	return token_standard;
}

void MetaData::set_collection_details(const Variant &collection_details) {
	this->collection_details = collection_details;
}

Variant MetaData::get_collection_details() const {
	return collection_details;
}

void MetaData::set_programmable_config(const Variant &programmable_config) {
	this->programmable_config = programmable_config;
}

Variant MetaData::get_programmable_config() const {
	return programmable_config;
}

void MetaData::add_creator(const Variant &creator) {
	creators.append(creator);
}

Array MetaData::get_creators() const {
	return creators;
}

Variant MetaData::get_collection() const {
	return collection;
}

void MetaData::copy_from_dict(const Dictionary &other) {
	if (other.has("id")) {
		const Variant mint_pubkey = Pubkey::new_from_string(other["id"]);
		set_mint(mint_pubkey);
	}

	if (SolanaUtils::nested_dict_has_keys(other, "royalty/primary_sale_happened")) {
		set_token_name(SolanaUtils::get_nested_value(other, "royalty/primary_sale_happened"));
	}
	if (SolanaUtils::nested_dict_has_keys(other, "royalty/basis_points")) {
		set_token_name(SolanaUtils::get_nested_value(other, "royalty/basis_points"));
	}
	if (other.has("mutable")) {
		set_is_mutable(other["mutable"]);
	}

	copy_creators(other);
	copy_grouping(other);

	if (!other.has("content")) {
		return;
	}

	const Dictionary content = other["content"];
	if (content.has("json_uri")) {
		set_uri(content["json_uri"]);
	}

	if (SolanaUtils::nested_dict_has_keys(content, "metadata/name")) {
		set_token_name(SolanaUtils::get_nested_value(content, "metadata/name"));
	}
	if (SolanaUtils::nested_dict_has_keys(content, "metadata/symbol")) {
		set_symbol(SolanaUtils::get_nested_value(content, "metadata/symbol"));
	}
	if (SolanaUtils::nested_dict_has_keys(content, "metadata/token_standard")) {
		const String token_standard = SolanaUtils::get_nested_value(content, "metadata/token_standard");

		if (token_standard == "NonFungible") {
			set_token_standard(0);
		} else if (token_standard == "FungibleAsset") {
			set_token_standard(1);
		} else if (token_standard == "Fungible") {
			set_token_standard(2);
		} else if (token_standard == "NonFungibleEdition") {
			set_token_standard(3);
		} else if (token_standard == "ProgrammableNonFungible") {
			set_token_standard(4);
		}
	}

	// TODO(Virax): Edition nonce and update authority is missing.
}

void MetaData::set_collection(const Variant &collection) {
	this->collection = collection;
}

PackedByteArray MetaData::serialize(const bool is_mutable) const {
	PackedByteArray res;
	const uint32_t DATA_LENGTH = 14 + name.length() + symbol.length() + uri.length();
	res.resize(DATA_LENGTH);

	int64_t offset = 0;
	res.encode_u32(offset, name.length());
	offset += sizeof(uint32_t);
	for (unsigned int i = 0; i < name.length(); i++) {
		res[i + offset] = name.to_ascii_buffer()[i];
	}
	offset += name.length();

	res.encode_u32(offset, symbol.length());
	offset += sizeof(uint32_t);
	for (unsigned int i = 0; i < symbol.length(); i++) {
		res[offset + i] = symbol.to_ascii_buffer()[i];
	}
	offset += symbol.length();

	res.encode_u32(offset, uri.length());
	offset += sizeof(uint32_t);
	for (unsigned int i = 0; i < uri.length(); i++) {
		res[offset + i] = uri.to_ascii_buffer()[i];
	}
	offset += uri.length();

	res.encode_u16(offset, seller_fee_basis_points);

	res.append(static_cast<int64_t>(enable_creators && !creators.is_empty()));
	if (enable_creators && !creators.is_empty()) {
		PackedByteArray serialized_creators;
		serialized_creators.resize(4);
		serialized_creators.encode_u32(0, creators.size());
		for (unsigned int i = 0; i < creators.size(); i++) {
			const MetaDataCreator *creators_ptr = Object::cast_to<MetaDataCreator>(creators[i]);
			const PackedByteArray serialized_collection = creators_ptr->serialize();
			serialized_creators.append_array(serialized_collection);
		}
		res.append_array(serialized_creators);
	}

	res.append(static_cast<int64_t>(enable_collection));
	if (enable_collection) {
		const MetaDataCollection *collection_ptr = Object::cast_to<MetaDataCollection>(collection);
		const PackedByteArray serialized_collection = collection_ptr->serialize();
		res.append_array(serialized_collection);
	}

	res.append(static_cast<int64_t>(enable_uses));
	if (enable_uses) {
		const MetaDataUses *uses_ptr = Object::cast_to<MetaDataUses>(uses);
		const PackedByteArray serialized_collection = uses_ptr->serialize();
		res.append_array(serialized_collection);
	}

	res.append(static_cast<int64_t>(is_mutable));

	res.append(static_cast<int64_t>(enable_collection_details));
	if (enable_collection_details) {
		res.append(0);
		PackedByteArray collection_size_data;

		const int32_t COLLECTION_DATA_SIZE = 8;
		collection_size_data.resize(COLLECTION_DATA_SIZE);
		collection_size_data.encode_u64(0, static_cast<int64_t>(collection_size));
		res.append_array(collection_size_data);
	}

	return res;
}

} //namespace godot