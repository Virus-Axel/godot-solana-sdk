#ifndef SOLANA_SDK_MPL_CONFIG_LINE
#define SOLANA_SDK_MPL_CONFIG_LINE

#include <account_meta.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <solana_client.hpp>

namespace godot {

class ConfigLine : public Resource {
	GDCLASS(ConfigLine, Resource)
private:
	String name;
	String uri;

protected:
	static void _bind_methods();

public:
	ConfigLine();

	void set_name(const String &value);
	String get_name();

	void set_uri(const String &value);
	String get_uri();

	PackedByteArray serialize();
	~ConfigLine();
};

class ConfigLineSetting : public Resource {
	GDCLASS(ConfigLineSetting, Resource)
private:
	String prefix_name;
	String prefix_uri;
	uint32_t name_length = 0;
	uint32_t uri_length = 0;
	bool is_sequential = false;

protected:
	static void _bind_methods();

public:
	ConfigLineSetting();

	void set_prefix_name(const String &value);
	String get_prefix_name();

	void set_name_length(const uint32_t value);
	uint32_t get_name_length();

	void set_prefix_uri(const String &value);
	String get_prefix_uri();

	void set_uri_length(const uint32_t value);
	uint32_t get_uri_length();

	void set_is_sequential(const bool value);
	bool get_is_sequential();

	PackedByteArray serialize();
	~ConfigLineSetting();
};

} //namespace godot

#endif