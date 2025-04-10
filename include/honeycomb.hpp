#ifndef GODOT_SOLANA_SDK_HONEYCOMB_HPP
#define GODOT_SOLANA_SDK_HONEYCOMB_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include "transaction.hpp"

namespace godot{

/**
 * @brief Godot base class used by generated HoneyComb classes and types.
 * 
 * @warning Might be replaced by new generated honeycomb code.
 */
class HoneyComb: public Node{
GDCLASS(HoneyComb, Node)
private:
    bool pending = false;
    Node* child = nullptr;
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
    void add_arg(const String& name, const String& type_name, const Variant& value, bool optional = false);
    void query_response_callback(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body);

    static void bind_non_changing_methods();

protected:
    HoneyComb();

    /**
     * @brief Bind methods of HoneyComb Node.
     */
    static void _bind_methods();
    ~HoneyComb();
};

}

#endif