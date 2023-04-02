#ifndef SOLANA_RESOURCE_HPP
#define SOLANA_RESOURCE_HPP

#include <godot_cpp/classes/resource.hpp>

namespace godot{

class SolanaResource : public Resource {
   // GDCLASS(SolanaResource, Resource)

private:

protected:
    void *data_pointer;
    virtual void _update_pointer() = 0;
    virtual void _free_pointer() = 0;
    void _free_pointer_if_not_null(){
        if(data_pointer != nullptr){
            _free_pointer();
            data_pointer = nullptr;
        }
    }

public:
    SolanaResource(){
        data_pointer = nullptr;
    }

    void* to_ptr(){
        _free_pointer_if_not_null();
        _update_pointer();
        return data_pointer;
    }

    ~SolanaResource() = default;
};
}

#endif