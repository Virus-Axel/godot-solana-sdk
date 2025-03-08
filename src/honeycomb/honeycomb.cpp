#include "honeycomb/honeycomb.hpp"

#include "godot_cpp/classes/json.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"
#include "transaction.hpp"

namespace godot {

void HoneyComb::query_response_callback(int result, int response_code, const PackedStringArray &headers, const PackedByteArray &body) {
	Dictionary response = JSON::parse_string(body.get_string_from_ascii());
	std::cout << body.get_string_from_ascii().ascii() << std::endl;

	args.clear();

	// Handle JSON Parsing Error
	if (response["data"].get_type() != Variant::DICTIONARY) {
		query_error_function("Invalid Response Format", body.get_string_from_ascii());
		ERR_FAIL_EDMSG("Error in request, check console logs");
	}

	Dictionary method_response = response["data"];
	query_response_function(method_response);
}

void HoneyComb::set_honeycomb_url(const String &url) {
	if (url.is_empty()) {
		honeycomb_url = "https://edge.test.honeycombprotocol.com/"; // Reset to default
	} else {
		honeycomb_url = url;
	}
	std::cout << "HoneyComb URL set to: " << honeycomb_url.ascii() << std::endl;
}

// void HoneyComb::query_response_callback(int result, int response_code, const PackedStringArray &headers, const PackedByteArray &body)
// {
//   Dictionary response = JSON::parse_string(body.get_string_from_ascii());
//   std::cout << body.get_string_from_ascii().ascii() << std::endl;

//   // Handle JSON Parsing Error
//   if (response["data"].get_type() != Variant::DICTIONARY)
//   {
//     query_error_function("Invalid Response Format", body.get_string_from_ascii());
//     ERR_FAIL_EDMSG("Error in request, check console logs");
//   }

//   Dictionary method_response = ((Dictionary)response["data"])[method_name];
//   String encoded_transaction = "";

//   // Handle Missing Transaction Key
//   if (method_response.has("tx"))
//   {
//     encoded_transaction = ((Dictionary)method_response["tx"])["transaction"];
//   }
//   else if (method_response.has("transaction"))
//   {
//     encoded_transaction = method_response["transaction"];
//   }
//   else
//   {
//     emit_signal("query_response_received", method_response);
//     // query_error_function("Missing Transaction Data", method_name);
//     // ERR_FAIL_EDMSG("Transaction data is missing.");
//   }

//   // Handle Empty Transaction
//   if (encoded_transaction.is_empty())
//   {
//     pending = false;
//     query_error_function("Empty Transaction", method_name);
//     ERR_FAIL_EDMSG("Transaction is empty.");
//   }

//   if (!result_tx->is_inside_tree())
//   {
//     memfree(result_tx);
//   }
//   else
//   {
//     remove_child(result_tx);
//   }

//   // Process the Transaction
//   PackedByteArray decoded_tx = SolanaUtils::bs58_decode(encoded_transaction);
//   result_tx = (Transaction *)(Object *)Transaction::new_from_bytes(decoded_tx);
//   emit_signal("query_response_received", result_tx);

//   add_child(result_tx, false, INTERNAL_MODE_BACK);

//   result_tx->set_signers(signers);

//   Callable callback = Callable(this, "query_response_function");
//   result_tx->connect("query_response_received", callback, CONNECT_ONE_SHOT);

//   for (unsigned int i = 0; i < signers.size(); i++)
//   {
//     if (signers[i].get_type() == Variant::OBJECT)
//     {
//       if (((Object *)signers[i])->get_class() == "Keypair")
//       {
//         Array signer_array;
//         signer_array.append(signers[i]);
//         result_tx->partially_sign(signer_array);
//       }
//     }
//   }

//   result_tx->send();
// }

void HoneyComb::query_response_function(const Dictionary &response) {
	// PackedByteArray bt = result_tx->serialize();

	pending = false;
	remove_child(child);
	emit_signal("query_response_received", response);
}

void HoneyComb::query_error_function(const String &error_type, const String &raw_body) {
	pending = false;
	remove_child(child);
	Dictionary error_info;
	error_info["error_type"] = error_type;
	error_info["raw_body"] = raw_body;

	emit_signal("query_error_occurred", error_info);
}

void HoneyComb::send_query() {
	const String HONEYCOMB_URL = honeycomb_url;
	std::cout << "HoneyComb URL = " << HONEYCOMB_URL.ascii() << std::endl;

	// Set up the callback for handling the response
	Callable callback = Callable(this, "query_response_callback");

	// Ensure the `HTTPRequest` signal is connected to avoid duplicate connections
	if (!api_request->is_connected("request_completed", callable_mp(this, &HoneyComb::query_response_callback))) {
		api_request->connect("request_completed", callable_mp(this, &HoneyComb::query_response_callback));
	}

	// Add the HTTPRequest node as a child
	if (!is_inside_tree() || !api_request->is_inside_tree()) {
		add_child(api_request);
	}

	child = api_request;
	pending = true;

	// Debugging: Print the request payload
	std::cout << "HoneyComb request payload: " << build().ascii() << std::endl;

	// Attempt to send the HTTP request asynchronously
	Error err = api_request->request(HONEYCOMB_URL, headers, HTTPClient::METHOD_POST, build());

	// Handle potential request initiation errors
	if (err != OK) {
		pending = false;
		query_error_function("Request Initialization Failed", String(itos(err)));
		ERR_PRINT("Failed to initiate HTTP request.");
	}
}

String HoneyComb::build() {
	String args_type_list = "";
	String args_list = "";
	String args_values = "";

	for (unsigned int i = 0; i < args.size(); i++) {
		Array format_params;
		Dictionary arg = args[i];
		format_params.append(arg["name"]);
		format_params.append(arg["type"]);
		args_type_list += String("${0}: {1},").format(format_params);

		format_params.clear();
		format_params.append(arg["name"]);
		args_list += String("{0}: ${0},").format(format_params);

		format_params.clear();
		format_params.append(arg["name"]);
		Variant value = arg["value"];

		// Debugging the type of value
		std::cout << "Value type: " << String(value).ascii() << std::endl;

		// Updated Serialization Logic
		if (value.get_type() == Variant::ARRAY) {
			// Fix for the `order` field: Directly append JSON array without wrapping in quotes
			args_values += "\"" + String(arg["name"]) + "\": " + JSON::stringify(value) + ",";
			std::cout << "Serialized array: " << JSON::stringify(value).ascii() << std::endl;
		}
		std::cout << String(value.get_type_name(value.get_type())).ascii() << std::endl;
		if (value.get_type() == Variant::OBJECT) {
			Object *obj = Object::cast_to<Object>(value);
			if (obj && obj->has_method("to_dict")) {
				Dictionary serialized_obj = obj->call("to_dict");
				args_values += "\"" + String(arg["name"]) + "\": " + JSON::stringify(serialized_obj) + ",";
				std::cout << "Serialized object: " << JSON::stringify(serialized_obj).ascii() << std::endl;
			} else {
				args_values += "\"" + String(arg["name"]) + "\": " + JSON::stringify(value) + ",";
				std::cout << "Non-serializable object, added as stringified value." << std::endl;
			}
		} else if (value.get_type() == Variant::DICTIONARY || value.get_type() == Variant::ARRAY || value.get_type() == Variant::PACKED_STRING_ARRAY) {
			Variant processed_value = process_wrapped_objects(value);
			args_values += "\"" + String(arg["name"]) + "\": " + JSON::stringify(processed_value) + ",";
			std::cout << "Serialized array/dictionary: " << JSON::stringify(processed_value).ascii() << std::endl;
		} else if (value.get_type() == Variant::PACKED_INT32_ARRAY) {
			// Serialize PackedInt32Array as a raw JSON array
			PackedInt32Array int_array = value;
			args_values += "\"" + String(arg["name"]) + "\": " + JSON::stringify(int_array) + ",";
			std::cout << "Serialized PackedInt32Array: " << JSON::stringify(int_array).ascii() << std::endl;
		} else if (value.get_type() == Variant::BOOL) {
			// Serialize Boolean as raw true/false without quotes
			args_values += "\"" + String(arg["name"]) + "\": " + (value ? "true" : "false") + ",";
			std::cout << "Serialized boolean: " << (value ? "true" : "false") << std::endl;
		} else if (value.get_type() == Variant::INT || value.get_type() == Variant::FLOAT) {
			args_values += "\"" + String(arg["name"]) + "\": " + String::num(value) + ",";
			std::cout << "Serialized number: " << String::num(value).ascii() << std::endl;
		} else {
			args_values += "\"" + String(arg["name"]) + "\": \"" + String(value) + "\",";
			std::cout << "Primitive value: " << String(value).ascii() << std::endl;
		}
	}

	if (!args.is_empty()) {
		// Remove trailing commas
		args_type_list = args_type_list.substr(0, args_type_list.length() - 1);
		args_list = args_list.substr(0, args_list.length() - 1);
		args_values = args_values.substr(0, args_values.length() - 1);
	}

	Array format_params;
	format_params.append(method_name);
	format_params.append(args_type_list);
	format_params.append(args_list);
	format_params.append(args_values);
	format_params.append(query_fields);

	String query = String("{ \"query\":\"query {0}({1}) { {0}({2}) { {4} } }\", \"variables\":{{3}}}").format(format_params);
	// String query = String("{ \"query\":\"query {0}({1}) { {0}({2}) { {4} } }\", \"variables\":{3}}").format(format_params);

	return query;
}
void HoneyComb::bind_method_from_ref(const String ref) {
}

PackedStringArray HoneyComb::get_headers() {
	return headers;
}

void HoneyComb::set_auth_token(const String &p_token) {
	headers.clear();
	headers.append("content-type: application/json");
	headers.append("Authorization: Bearer " + p_token);
}

void HoneyComb::add_arg(const String &name, const String &type_name, const Variant &value, bool optional) {
	Dictionary entry;
	entry["name"] = name;
	if (!optional) {
		entry["type"] = type_name + String("!");
	} else {
		entry["type"] = type_name;
	}
	entry["value"] = value;
	args.append(entry);
}

HoneyComb::HoneyComb() {
	headers.append("content-type: application/json");
	api_request = memnew(HTTPRequest);
	result_tx = memnew(Transaction);
}

void HoneyComb::bind_non_changing_methods() {
	ClassDB::add_signal("HoneyComb", MethodInfo("query_response_received", PropertyInfo(Variant::DICTIONARY, "response")));
	ClassDB::add_signal("HoneyComb", MethodInfo("query_error_occurred", PropertyInfo(Variant::DICTIONARY, "error_info"))); // Error Signal

	// Other binding methods
	ClassDB::bind_method(D_METHOD("query_response_callback", "result", "response_code", "headers", "body"), &HoneyComb::query_response_callback);
	// ClassDB::bind_method(D_METHOD("query_response_function", "response"), &HoneyComb::query_response_function);
	ClassDB::bind_method(D_METHOD("set_auth_token", "token"), &HoneyComb::set_auth_token);
	ClassDB::bind_method(D_METHOD("get_headers"), &HoneyComb::get_headers);
	ClassDB::bind_method(D_METHOD("set_honeycomb_url", "url"), &HoneyComb::set_honeycomb_url);
}

HoneyComb::~HoneyComb() {
}
} //namespace godot

// Function to process and deserialize wrapped objects
Variant HoneyComb::process_wrapped_objects(const Variant &value) {
	if (value.get_type() == Variant::DICTIONARY) {
		Dictionary dict = value;
		Dictionary processed_dict;
		for (int i = 0; i < dict.size(); i++) {
			Variant key = dict.keys()[i];
			Variant item = dict[key];
			processed_dict[key] = process_wrapped_objects(item);
		}
		return processed_dict;
	} else if (value.get_type() == Variant::ARRAY) {
		Array arr = value;
		Array processed_array;
		for (int i = 0; i < arr.size(); i++) {
			processed_array.append(process_wrapped_objects(arr[i]));
		}
		return processed_array;
	} else if (value.get_type() == Variant::OBJECT) {
		Object *obj = Object::cast_to<Object>(value);
		if (obj && obj->has_method("to_dict")) {
			return obj->call("to_dict");
		}
		return String("[Unsupported Object]");
	}
	return value;
}
