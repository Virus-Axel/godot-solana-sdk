#ifndef SOLANA_SDK_ANCHOR_PROGRAM
#define SOLANA_SDK_ANCHOR_PROGRAM

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/node.hpp>

namespace godot{

class AnchorProgram : public Node{
    GDCLASS(AnchorProgram, Node)
private:
    Variant idl;
    String pid;
    bool try_from_pid = false;
    Variant json_file;
    bool try_from_json_file = false;
    Array instructions;

    static bool is_typed_primitive(const Dictionary &dict);
    static PackedByteArray serialize_typed_primitive(const Dictionary &dict);
    Variant idl_address(const Variant& pid);
    bool load_from_pid(const String& pid);

    bool is_int(const Variant &var);
    bool is_float(const Variant &var);
    bool check_type(const Variant& expected_type, const Variant& value);
    bool validate_instruction_arguments(const String &instruction_name, const Array &arguments);
    void register_instruction_builders();
    PackedByteArray discriminator_by_name(const String &name);
    Dictionary find_idl_instruction(const String &name);
    Dictionary find_idl_account(const String &name);
    Dictionary find_idl_type(const String &name);

    Variant deserialize_dict(const PackedByteArray& bytes, const Dictionary& type, int& consumed_bytes);
    Variant deserialize_variant(const PackedByteArray& bytes, const Variant& type, int &consumed_bytes);

    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

protected:
    static void _bind_methods();

public:
    static PackedByteArray serialize_variant(const Variant &var);

    void set_idl(const Dictionary& idl);
    Dictionary get_idl();
    void set_try_from_pid(const bool try_from_pid);
    bool get_try_from_pid();
    void set_try_from_json_file(const bool try_from_json_file);
    bool get_try_from_json_file();

    void set_pid(const String& try_from_pid);
    String get_pid();
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

    Variant build_instruction(String name, Array accounts, Variant arguments);
    Dictionary fetch_account(const String name, const Variant &account);
};

}

#endif