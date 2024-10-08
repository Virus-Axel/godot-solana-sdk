#ifndef GODOT_SOLANA_SDK_HONEYCOMB_HPP
#define GODOT_SOLANA_SDK_HONEYCOMB_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include "transaction.hpp"

namespace godot{

enum ResourceStorageEnum{
    AccountState = 0,
    LedgerState = 1,
};

class InitResourceInput: public Resource{
GDCLASS(InitResourceInput, Resource)
private:
    String resource_name = "";
    String symbol = "";
    String uri = "";
    int32_t decimals = 0;
    ResourceStorageEnum storage = AccountState;
protected:
    static void _bind_methods();
public:
    void set_resource_name(const String& resource_name);
    String get_resource_name();
    void set_symbol(const String& symbol);
    String get_symbol();
    void set_uri(const String& uri);
    String get_uri();
    void set_decimals(int32_t decimals);
    int32_t get_decimals();
    void set_storage(int32_t storage);
    int32_t get_storage();

    Dictionary to_dict();
};


class UserInfoInput: public Resource{
GDCLASS(UserInfoInput, Resource)
private:
    String username = "";
    String real_name = "";
    String bio = "";
    String pfp = "";
protected:
    static void _bind_methods();
public:
    Dictionary to_dict();
};


class BasicTreeConfig : public Resource{
GDCLASS(BasicTreeConfig, Resource)
private:
    int32_t num_assets = 0;
protected:
    static void _bind_methods();
public:
};

class AdvancedTreeConfig : public Resource{
GDCLASS(AdvancedTreeConfig, Resource)
private:
    int32_t max_depth = 0;
    int32_t max_buffer_size = 0;
    int32_t canopy_depth = 0;
protected:
    static void _bind_methods();
public:
    Dictionary to_dict();
};

class TreeSetupConfig : public Resource{
GDCLASS(TreeSetupConfig, Resource)
private:
    Variant basic;
    Variant advanced;
protected:
    static void _bind_methods();
public:
    Dictionary to_dict();
};

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
    void transaction_response_callback(const Dictionary& response);

    static void bind_non_changing_methods();

protected:
    HoneyComb();
    static void _bind_methods();
    ~HoneyComb();
public:

    Variant create_project(const Variant& authority, const String& name);
    void create_user(const Variant& user_wallet_keypair);
    void create_profile(const Variant& project, const Variant& payer);
    void create_resource(const Variant& project, const Variant& authority, const String& name, const String& uri, const String& symbol, uint32_t decimals);
    Variant createCreateNewResourceTransaction(const Variant& project, const Variant& authority, Variant params, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createCreateNewResourceTreeTransaction(const Variant& project, const Variant& resource, const Variant& authority, Variant treeConfig, const Variant& delegateAuthority = "", const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createMintResourceTransaction(const Variant& resource, const Variant& owner, const Variant& authority, int64_t amount, const Variant& delegateAuthority = "", const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createBurnResourceTransaction(const Variant& resource, int64_t amount, const Variant& authority, const Variant& owner = "", const Variant& payer = "", const Variant& delegateAuthority = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
    Variant createNewUserTransaction(const Variant& wallet, Variant info = Variant(nullptr), const Variant& payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
};

}

#endif