#ifndef SOLANA_SDK_ANCHOR_PROGRAM
#define SOLANA_SDK_ANCHOR_PROGRAM

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/node.hpp>
#include <solana_client.hpp>

namespace godot{

const int DISCRIMINATOR_LENGTH = 8;

class AnchorProgram : public Node{
    GDCLASS(AnchorProgram, Node)
private:
    Dictionary idl;
    String pid;
    String url_override = "";
    String pending_account_name = "";
    String pending_accounts_name = "";
    bool try_from_pid = false;
    Variant json_file;
    bool try_from_json_file = false;
    Array instructions;

    SolanaClient *idl_client = nullptr;
    SolanaClient *fetch_client = nullptr;

    bool detect_writable(const Dictionary& account) const;
    bool detect_is_signer(const Dictionary& account) const;
    bool detect_optional(const Dictionary& account) const;
    static PackedByteArray evaluate_discriminant(const Dictionary& discriminant_info);

    static bool is_typed_primitive(const Dictionary &dict);
    static PackedByteArray serialize_typed_primitive(const Dictionary &dict);
    Variant idl_address(const Variant& pid);
    bool load_from_pid(const String& pid);
    void idl_from_pid_callback(const Dictionary& rpc_result);
    void fetch_account_callback(const Dictionary &rpc_result);
    void fetch_all_accounts_callback(const Dictionary &rpc_result);
    void extract_idl_from_rpc_response(const Dictionary& rpc_result);
    void extract_idl_from_data(const Array& data_info);

    bool is_int(const Variant &var);
    bool is_float(const Variant &var);
    static bool is_option(const Variant &var);
    bool check_type(const Variant& expected_type, const Variant& value);
    bool validate_instruction_arguments(const String &instruction_name, const Array &arguments);
    void register_instruction_builders();
    PackedByteArray discriminator_by_name(const String &name, const String &namespace_string) const;

    Dictionary find_idl_type(const String &name);

    Variant deserialize_variant(const PackedByteArray& bytes, const Variant& type, int &consumed_bytes);

    static Variant decorate_instruction_argument(const Variant &anchor_type, const Variant& argument);

    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

protected:
    static void _bind_methods();

public:
    AnchorProgram();

    void _process(double delta) override;

    static Variant get_address_from_idl(const Dictionary& idl);
    static PackedByteArray serialize_variant(const Variant &var);

    void set_idl(const Dictionary& idl);
    Dictionary get_idl() const;
    void set_try_from_pid(const bool try_from_pid);
    bool get_try_from_pid();
    void set_try_from_json_file(const bool try_from_json_file);
    bool get_try_from_json_file();

    void set_url_override(const String& url_override);

    void set_pid(const String& try_from_pid);
    String get_pid();
    String get_idl_name() const;
    void set_json_file(const Variant& try_from_json_file);
    Variant get_json_file();

    static Dictionary f32(float val);
    static Dictionary f64(double val);
    static Dictionary s8(int val);
    static Dictionary s16(int val);
    static Dictionary s32(int32_t val);
    static Dictionary s64(int64_t val);
    static Dictionary u8(unsigned int val);
    static Dictionary u16(unsigned int val);
    static Dictionary u32(uint32_t val);
    static Dictionary u64(uint64_t val);
    static Dictionary option(const Variant &val);

    static bool is_enum(const Variant &anchor_type);
    static String get_object_name(const Variant &anchor_type);
    static bool is_vec_type(const Variant &anchor_type);
    static String get_int_type_info(const Variant &anchor_type);
    static Variant::Type get_godot_type(const Variant &anchor_type);
    static String get_godot_class_hint(const Variant &anchor_type);
    static PropertyHint get_godot_hint(const Variant &anchor_type);
    Variant deserialize_dict(const PackedByteArray& bytes, const Dictionary& type, int& consumed_bytes);
    Dictionary find_idl_account(const String &name);
    Dictionary find_idl_instruction(const String &name) const;

    const String global_prefix = "global:";
    const String account_prefix = "account:";

    Variant build_argument_dictionary(const Array& arguments, const StringName& instruction_name) const;
    Variant build_instruction(String name, Array accounts, Variant arguments) const;
    Error fetch_account(const String name, const Variant &account);
    Error fetch_all_accounts(const String name, const Array& additional_filters = Array());
};

}

#endif