#include "candy_machine/config_line.hpp"

#include "instruction.hpp"
#include "instructions/associated_token_account.hpp"
#include "instructions/mpl_token_metadata.hpp"
#include "instructions/spl_token.hpp"
#include "instructions/system_program.hpp"

namespace godot {

void ConfigLine::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_name", "value"), &ConfigLine::set_name);
	ClassDB::bind_method(D_METHOD("get_name"), &ConfigLine::get_name);

	ClassDB::bind_method(D_METHOD("set_uri", "value"), &ConfigLine::set_uri);
	ClassDB::bind_method(D_METHOD("get_uri"), &ConfigLine::get_uri);

	ClassDB::bind_method(D_METHOD("serialize"), &ConfigLine::serialize);

	ClassDB::add_property("ConfigLine", PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
	ClassDB::add_property("ConfigLine", PropertyInfo(Variant::STRING, "uri"), "set_uri", "get_uri");
}

ConfigLine::ConfigLine() {
}

void ConfigLine::set_name(const String &value) {
	this->name = value;
}
String ConfigLine::get_name() {
	return name;
}

void ConfigLine::set_uri(const String &value) {
	this->uri = value;
}
String ConfigLine::get_uri() {
	return uri;
}

PackedByteArray ConfigLine::serialize() {
	PackedByteArray result;
	result.resize(4);
	result.encode_u32(0, name.length());
	result.append_array(name.to_ascii_buffer());
	PackedByteArray temp_array;
	temp_array.resize(4);
	temp_array.encode_u32(0, uri.length());
	result.append_array(temp_array);
	result.append_array(uri.to_ascii_buffer());

	return result;
}
ConfigLine::~ConfigLine() {
}

void ConfigLineSetting::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_prefix_name", "value"), &ConfigLineSetting::set_prefix_name);
	ClassDB::bind_method(D_METHOD("get_prefix_name"), &ConfigLineSetting::get_prefix_name);

	ClassDB::bind_method(D_METHOD("set_name_length", "value"), &ConfigLineSetting::set_name_length);
	ClassDB::bind_method(D_METHOD("get_name_length"), &ConfigLineSetting::get_name_length);

	ClassDB::bind_method(D_METHOD("set_prefix_uri", "value"), &ConfigLineSetting::set_prefix_uri);
	ClassDB::bind_method(D_METHOD("get_prefix_uri"), &ConfigLineSetting::get_prefix_uri);

	ClassDB::bind_method(D_METHOD("set_uri_length", "value"), &ConfigLineSetting::set_uri_length);
	ClassDB::bind_method(D_METHOD("get_uri_length"), &ConfigLineSetting::get_uri_length);

	ClassDB::bind_method(D_METHOD("set_is_sequential", "value"), &ConfigLineSetting::set_is_sequential);
	ClassDB::bind_method(D_METHOD("get_is_sequential"), &ConfigLineSetting::get_is_sequential);

	ClassDB::bind_method(D_METHOD("serialize"), &ConfigLineSetting::serialize);

	ClassDB::add_property("ConfigLineSetting", PropertyInfo(Variant::STRING, "prefix_name"), "set_prefix_name", "get_prefix_name");
	ClassDB::add_property("ConfigLineSetting", PropertyInfo(Variant::INT, "name_length"), "set_name_length", "get_name_length");
	ClassDB::add_property("ConfigLineSetting", PropertyInfo(Variant::STRING, "prefix_uri"), "set_prefix_uri", "get_prefix_uri");
	ClassDB::add_property("ConfigLineSetting", PropertyInfo(Variant::INT, "uri_length"), "set_uri_length", "get_uri_length");
	ClassDB::add_property("ConfigLineSetting", PropertyInfo(Variant::BOOL, "is_sequential"), "set_is_sequential", "get_is_sequential");
}

ConfigLineSetting::ConfigLineSetting() {
}

void ConfigLineSetting::set_prefix_name(const String &value) {
	prefix_name = value;
}
String ConfigLineSetting::get_prefix_name() {
	return prefix_name;
}

void ConfigLineSetting::set_name_length(const uint32_t value) {
	name_length = value;
}
uint32_t ConfigLineSetting::get_name_length() {
	return name_length;
}

void ConfigLineSetting::set_prefix_uri(const String &value) {
	prefix_uri = value;
}
String ConfigLineSetting::get_prefix_uri() {
	return prefix_uri;
}

void ConfigLineSetting::set_uri_length(const uint32_t value) {
	uri_length = value;
}
uint32_t ConfigLineSetting::get_uri_length() {
	return uri_length;
}

void ConfigLineSetting::set_is_sequential(const bool value) {
	is_sequential = value;
}
bool ConfigLineSetting::get_is_sequential() {
	return is_sequential;
}

PackedByteArray ConfigLineSetting::serialize() {
	PackedByteArray result;
	int cursor = 0;

	result.resize((4 + prefix_name.length() + 4 + 4 + prefix_uri.length() + 4 + 1));

	result.encode_u32(cursor, prefix_name.length());
	cursor += 4;
	for (unsigned int i = 0; i < prefix_name.length(); i++) {
		result[cursor + i] = prefix_name[i];
	}
	cursor += prefix_name.length();

	result.encode_u32(cursor, name_length);
	cursor += 4;
	result.encode_u32(cursor, prefix_uri.length());
	cursor += 4;
	for (unsigned int i = 0; i < prefix_uri.length(); i++) {
		result[cursor + i] = prefix_uri[i];
	}

	cursor += prefix_uri.length();
	result.encode_u32(cursor, uri_length);
	cursor += 4;
	result[cursor] = (int)is_sequential;

	return result;
}

ConfigLineSetting::~ConfigLineSetting() {
}

} //namespace godot