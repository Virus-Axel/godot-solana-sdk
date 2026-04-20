#include "isigner.hpp"

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot_solana_sdk {

void ISigner::_bind_methods() {
	using namespace godot;

	ClassDB::add_signal("ISigner",
			MethodInfo("sign_completed",
					PropertyInfo(Variant::STRING, "request_id"),
					PropertyInfo(Variant::ARRAY, "signatures")));

	ClassDB::add_signal("ISigner",
			MethodInfo("sign_failed",
					PropertyInfo(Variant::STRING, "request_id"),
					PropertyInfo(Variant::STRING, "error_code"),
					PropertyInfo(Variant::STRING, "message")));
}

} //namespace godot_solana_sdk
