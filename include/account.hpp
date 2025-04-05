#ifndef SOLANA_SDK_ACCOUNT_HPP
#define SOLANA_SDK_ACCOUNT_HPP

#include "pubkey.hpp"
#include "account.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

/**
 * @brief Account data storage class.
 * 
 * Class meant to store the contents of a solana account.
 * @todo Implement a real functionality for this class.
 * @warning WIP, do not use.
 */
class Account : public Node {
    GDCLASS(Account, Node)

private:
    void *data_pointer;

protected:
    /**
     * @brief Binds methods for Account Node.
     */
    static void _bind_methods();

public:
    Account();

    /**
     * @brief Does nothing
     * 
     * @todo Implement if needed, otherwise remove.
     * 
     * @param lamports Not used
     * @param space Not used
     * @param owner Not used
     */
    void create_new(uint64_t lamports, uint64_t space, const Variant& owner);

    ~Account();
};
}

#endif