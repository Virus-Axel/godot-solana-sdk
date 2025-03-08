#ifndef GODOT_SOLANA_SDK_HONEYCOMB_HPP
#define GODOT_SOLANA_SDK_HONEYCOMB_HPP

#include "honeycomb/honeycomb_generated.hpp"
#include "transaction.hpp"
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>

namespace godot {

class HoneyComb : public Node {
	GDCLASS(HoneyComb, Node)
private:
	bool pending = false;
	Node *child = nullptr;
	HTTPRequest *api_request;
	Transaction *result_tx;
	String honeycomb_url = "https://edge.test.honeycombprotocol.com/"; // Default URL
	PackedStringArray headers;

	String request;
	String method_name;
	Array args;

	String query_fields = "";
	PackedStringArray get_headers();

	String build();
	void send_query();

	void bind_method_from_ref(const String ref);
	void add_arg(const String &name, const String &type_name, const Variant &value, bool optional = false);
	void query_response_callback(int result, int response_code, const PackedStringArray &headers, const PackedByteArray &body);
	void query_response_function(const Dictionary &response);
	void query_error_function(const String &error_type, const String &raw_body);
	void set_auth_token(const String &p_token);
	void set_honeycomb_url(const String &url);
	Variant process_wrapped_objects(const Variant &value);
	static void bind_non_changing_methods();

protected:
	HoneyComb();
	static void _bind_methods();
	~HoneyComb();

public:
	HONEYCOMB_METHOD_DEFS
};

} //namespace godot

#endif