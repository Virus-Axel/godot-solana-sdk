#include "honeycomb/types/SocialInfo_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SocialInfo::set_twitter(const String &val) {
	this->twitter = val;
}

String SocialInfo::get_twitter() const {
	return this->twitter;
}

void SocialInfo::set_discord(const String &val) {
	this->discord = val;
}

String SocialInfo::get_discord() const {
	return this->discord;
}

void SocialInfo::set_steam(const String &val) {
	this->steam = val;
}

String SocialInfo::get_steam() const {
	return this->steam;
}

void SocialInfo::set_civic(const Array &val) {
	this->civic = val;
}

Array SocialInfo::get_civic() const {
	return this->civic;
}

void SocialInfo::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_twitter"), &SocialInfo::get_twitter);
	ClassDB::bind_method(D_METHOD("set_twitter", "value"), &SocialInfo::set_twitter);
	ClassDB::add_property("SocialInfo", PropertyInfo(Variant::Type::STRING, "twitter"), "set_twitter", "get_twitter");
	ClassDB::bind_method(D_METHOD("get_discord"), &SocialInfo::get_discord);
	ClassDB::bind_method(D_METHOD("set_discord", "value"), &SocialInfo::set_discord);
	ClassDB::add_property("SocialInfo", PropertyInfo(Variant::Type::STRING, "discord"), "set_discord", "get_discord");
	ClassDB::bind_method(D_METHOD("get_steam"), &SocialInfo::get_steam);
	ClassDB::bind_method(D_METHOD("set_steam", "value"), &SocialInfo::set_steam);
	ClassDB::add_property("SocialInfo", PropertyInfo(Variant::Type::STRING, "steam"), "set_steam", "get_steam");
	ClassDB::bind_method(D_METHOD("get_civic"), &SocialInfo::get_civic);
	ClassDB::bind_method(D_METHOD("set_civic", "value"), &SocialInfo::set_civic);
	ClassDB::add_property("SocialInfo", PropertyInfo(Variant::Type::ARRAY, "civic"), "set_civic", "get_civic");
	ClassDB::bind_method(D_METHOD("to_dict"), &SocialInfo::to_dict);
}
Dictionary SocialInfo::to_dict() {
	Dictionary res;
	if (!twitter.is_empty()) {
		res["twitter"] = twitter;
	}
	if (!discord.is_empty()) {
		res["discord"] = discord;
	}
	if (!steam.is_empty()) {
		res["steam"] = steam;
	}
	res["civic"] = civic;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot