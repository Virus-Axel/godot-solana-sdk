#ifndef SOLANA_SDK_COMPUTE_BUDGET_HPP
#define SOLANA_SDK_COMPUTE_BUDGET_HPP

#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>

namespace godot{

class ComputeBudget : public Node{
    GDCLASS(ComputeBudget, Node)
private:

    static const int COMPUTE_UNIT_LIMIT_DATA_SIZE = 5;
    static const int COMPUTE_UNIT_PRICE_DATA_SIZE = 9;

protected:
    static void _bind_methods();

public:
    static const std::string ID;

    static Variant set_compute_unit_limit(const uint32_t unit_limit);
    static Variant set_compute_unit_price(const uint32_t unit_price);
};

}

#endif