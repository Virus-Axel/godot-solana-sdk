#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SOCIALINFO
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SOCIALINFO
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class SocialInfo : public Resource {
	GDCLASS(SocialInfo, Resource)
private:
	String twitter;
	String discord;
	String steam;
	Array civic;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_twitter(const String &val);
	String get_twitter() const;
	void set_discord(const String &val);
	String get_discord() const;
	void set_steam(const String &val);
	String get_steam() const;
	void set_civic(const Array &val);
	Array get_civic() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
