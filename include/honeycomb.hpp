#ifndef GODOT_SOLANA_SDK_HONEYCOMB_HPP
#define GODOT_SOLANA_SDK_HONEYCOMB_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include "transaction.hpp"

namespace godot{

class HoneyComb: public Node{
GDCLASS(HoneyComb, Node)
private:
    HTTPRequest* api_request;
    Transaction* result_tx;

    String request;
    String method_name;
    Array args;

    Array signers;
    String query_fields = "";

    String build();
    void send_query();

    void bind_method_from_ref(const String ref);
    void add_arg(const String& name, const String& type_name, const String& value, bool optional = false);
    void query_response_callback(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body);
    void transaction_response_callback(const Dictionary& response);

protected:
    HoneyComb();
    static void _bind_methods();
    ~HoneyComb();
public:

    Variant create_project(const Variant& authority, const String& name);
    void create_user(const Variant& user_wallet_keypair);
    void create_profile(const Variant& project, const Variant& payer);
    void create_resource(const Variant& project, const Variant& authority, const String& name, const String& uri, const String& symbol, uint32_t decimals);
};

}

#endif