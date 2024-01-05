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

    Variant idl_address(const Variant& pid);
    void load_from_pid(const String& pid);
    void register_instruction_builders();


    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

protected:
    static void _bind_methods();

public:

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
};

}

#endif