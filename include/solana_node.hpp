#ifndef SOLANA_NODE_HPP
#define SOLANA_NODE_HPP

#include <godot_cpp/classes/node.hpp>

namespace godot{

class SolanaNode : public Node {
   // GDCLASS(SolanaResource, Resource)

private:

protected:
    void _free_pointer_if_not_null(){
        if(data_pointer != nullptr){
            _free_pointer();
            data_pointer = nullptr;
        }
    }

protected:
    void *data_pointer;
    virtual void _update_pointer() = 0;
    virtual void _free_pointer() = 0;

public:
    SolanaNode(){
        data_pointer = nullptr;
    }

    void* to_ptr(){
        if (data_pointer == nullptr){
            _update_pointer();
        }
        return data_pointer;
    }

    ~SolanaNode() = default;
};
}

#endif