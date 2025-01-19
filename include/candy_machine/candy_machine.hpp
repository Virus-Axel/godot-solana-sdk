#ifndef SOLANA_SDK_MPL_CANDY_MACHINE
#define SOLANA_SDK_MPL_CANDY_MACHINE

#include <account_meta.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <solana_client.hpp>

namespace godot {

class MplCandyMachine : public SolanaClient {
	GDCLASS(MplCandyMachine, SolanaClient)
private:
protected:
	static void _bind_methods();

public:
	MplCandyMachine();

	static PackedByteArray mint_discriminator();
	static PackedByteArray mint2_discriminator();
	static PackedByteArray initialize_discriminator();
	static PackedByteArray initialize2_discriminator();
	static PackedByteArray add_config_lines_discriminator();

	static const std::string ID;

	static Variant initialize(
			const Variant &authority,
			const Variant &candy_machine_account,
			const Variant &collection_mint,
			const Variant &candy_machine_data,
			bool pnft = false);

	static Variant mint(
			const Variant &payer,
			const Variant &receiver,
			const Variant &mint,
			const Variant &collection_mint,
			const Variant &collection_update_authority,
			const Variant &candy_machine_key);

	static Variant add_config_lines(
			const Variant &candy_machine_key,
			const Variant &authority,
			const Array &config_lines,
			const unsigned int index);

	static Variant new_candy_machine_authority_pda(const Variant &candy_machine_key);
	void get_candy_machine_info(const Variant &candy_machine_key);
	void fetch_account_callback(const Dictionary &params);
	static Variant get_pid();
};

} //namespace godot

#endif