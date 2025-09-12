#ifndef SOLANA_SDK_SYSTEM_PROGRAM_HPP
#define SOLANA_SDK_SYSTEM_PROGRAM_HPP

#include <cstdint>
#include <string>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Instruction builder class for Solana System Program.
 */
class SystemProgram : public Node {
	GDCLASS_CUSTOM(SystemProgram, Node)
private:
	enum MethodNumber : uint8_t {
		CREATE_ACCOUNT = 0,
		CREATE_ACCOUNT_WITH_SEED = 3,
	};

	static const int TRANSFER_DATA_SIZE = 12;
	static const int CREATE_ACCOUNT_DATA_SIZE = 52;
	static const int CREATE_ACCOUNT_WITH_SEED_DATA_SIZE = 92;

	static const std::string PID;

protected:
	/**
	 * @bindmethods{SystemProgram, Node}
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Get the sysvar clock pubkey Pubkey
	 *
	 * @return Variant with the sysvar clock Pubkey
	 */
	static Variant get_sysvar_clock_pubkey();

	/**
	 * @brief Get the sysvar epoch schedule pubkey Pubkey
	 *
	 * @return Variant with the sysvar epoch schedule Pubkey
	 */
	static Variant get_sysvar_epoch_schedule_pubkey();

	/**
	 * @brief Get the sysvar fees pubkey Pubkey
	 *
	 * @return Variant with the sysvar fees Pubkey
	 */
	static Variant get_sysvar_fees_pubkey();

	/**
	 * @brief Get the sysvar instructions pubkey Pubkey
	 *
	 * @return Variant with the sysvar instructions Pubkey
	 */
	static Variant get_sysvar_instructions_pubkey();

	/**
	 * @brief Get the sysvar recent blockhashes pubkey Pubkey
	 *
	 * @return Variant with the sysvar recent blockhashes Pubkey
	 */
	static Variant get_sysvar_recent_blockhashes_pubkey();

	/**
	 * @brief Get the sysvar rent pubkey Pubkey
	 *
	 * @return Variant with the sysvar rent Pubkey
	 */
	static Variant get_sysvar_rent_pubkey();

	/**
	 * @brief Get the sysvar slot hashes pubkey Pubkey
	 *
	 * @return Variant with the sysvar slot hashes Pubkey
	 */
	static Variant get_sysvar_slot_hashes_pubkey();

	/**
	 * @brief Get the sysvar slot history pubkey Pubkey
	 *
	 * @return Variant with the sysvar slot history Pubkey
	 */
	static Variant get_sysvar_slot_history_pubkey();

	/**
	 * @brief Get the sysvar stake history pubkey Pubkey
	 *
	 * @return Variant with the sysvar stake history Pubkey
	 */
	static Variant get_sysvar_stake_history_pubkey();

	/**
	 * @brief Get the sysvar epoch rewards pubkey Pubkey
	 *
	 * @return Variant with the sysvar epoch rewards Pubkey
	 */
	static Variant get_sysvar_epoch_rewards_pubkey();

	/**
	 * @brief Get the sysvar restart pubkey Pubkey
	 *
	 * @return Variant with the restart sysvar Pubkey
	 */
	static Variant get_last_restart_slot_pubkey();

	/**
	 * @brief Create an Instruction that creates a new account.
	 *
	 * @param from_keypair Payer of the transaction. @signer @writable
	 * @param to_keypair Key of the account to be created. @signer @writable
	 * @param lamports Lamports to send to the new account.
	 * @param space Space to allocate on the new account.
	 * @param owner Owner of the new account.
	 * @return Variant Create account Instruction.
	 */
	static Variant create_account(const Variant &from_keypair, const Variant &to_keypair, uint64_t lamports, uint64_t space, const Variant &owner);

	/**
	 * @brief Create an Instruction that creates a new account from a seed.
	 *
	 * @param from_keypair Payer of the transaction. @signer @writable
	 * @param base_keypair Base key to create account from. @signer
	 * @param seed Seeds string to use for created account address.
	 * @param lamports Lamports to send to the new account.
	 * @param space Space to allocate on the new account.
	 * @param owner Owner of the new account.
	 * @return Variant
	 */
	static Variant create_account_with_seed(const Variant &from_keypair, const Variant &base_keypair, const String &seed, uint64_t lamports, uint64_t space, const Variant &owner);

	/**
	 * @brief Create an Instruction that transfers lamports to another account.
	 *
	 * @param sender Account to transfer from. @signer @writable
	 * @param reciever Account to transfer to. @writable
	 * @param lamports Amount of lamports to send.
	 * @return Variant Transfer Instruction.
	 */
	static Variant transfer(const Variant &sender, const Variant &reciever, uint64_t lamports);

	/**
	 * @brief Get the program ID of SystemProgram.
	 *
	 * @return Variant Program ID.
	 */
	static Variant get_pid();
};

} //namespace godot

#endif