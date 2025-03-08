#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEPROJECTTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEPROJECTTRANSACTIONRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CreateCreateProjectTransactionResponse : public Resource {
	GDCLASS(CreateCreateProjectTransactionResponse, Resource)
private:
	Variant tx;
	String project;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tx(const Variant &val);
	Variant get_tx() const;
	void set_project(const String &val);
	String get_project() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
