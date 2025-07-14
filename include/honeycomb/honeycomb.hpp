#ifndef GODOT_SOLANA_SDK_HONEYCOMB_HPP
#define GODOT_SOLANA_SDK_HONEYCOMB_HPP

#include "transaction.hpp"
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <mutex>

namespace godot {

class HoneyComb : public Node {
	GDCLASS(HoneyComb, Node)
private:
	std::mutex pending_mutex;
	bool pending = false;
	Node *child = nullptr;
	HTTPRequest *api_request;
	Transaction *result_tx;
	String honeycomb_url = "https://edge.test.honeycombprotocol.com/"; // Default URL
	PackedStringArray headers;

	String request;
	String method_name;
	Array args;

	String query_fields = "";
	PackedStringArray get_headers();

	String build();
	void send_query();

	/**
	 * @brief Binds a method to the Godot engine using a reference string.
	 * @param ref The reference string containing method information.
	 * @note This method is currently a placeholder for future implementation.
	 */
	void bind_method_from_ref(const String ref);
	void add_arg(const String &name, const String &type_name, const Variant &value, bool optional = false);
	void query_response_callback(int result, int response_code, const PackedStringArray &headers, const PackedByteArray &body);
	void query_response_function(const Dictionary &response);
	void query_error_function(const String &error_type, const String &raw_body);
	void set_auth_token(const String &p_token);
	void set_honeycomb_url(const String &url);
	Variant process_wrapped_objects(const Variant &value);
	static void bind_non_changing_methods();

protected:
	HoneyComb();
	/**
	 * @brief Binds methods and properties to Godot.
	 * This function is required by Godot's NativeScript API.
	 */
	static void _bind_methods();
	~HoneyComb() {
		if (api_request) {
			memdelete(api_request);
			api_request = nullptr;
		}
		if (result_tx) {
			memdelete(result_tx);
			result_tx = nullptr;
		}
	}

public:
	/**
	 * @brief Executes the find_compressed_accounts() GraphQL operation in Godot
	 * @param addresses is Required parameter. Type: [Bytes!]. List of addresses to search for.
	 * @param identity Optional parameter. Type: DisrcriminatorFilter. Default: Variant(nullptr). Identity filter for accounts.
	 * @param leaves Optional parameter. Type: [TreeLeaf!]. Default: Array(). List of tree leaves to search in.
	 * @param parsedData Optional parameter. Type: JSON. Default: Dictionary(). Parsed data to filter accounts by.
	 * @return Result object containing: leaf_idx parsed_data raw_data schema_validated tree_id
	 */

	Variant findCompressedAccounts(Array addresses, Variant identity = Variant(nullptr), Array leaves = Array(), Dictionary parsedData = Dictionary());
	/**
	 * @brief Executes the fetch_proofs() GraphQL operation in Godot
	 * @param leaves is Required parameter. Type: [TreeLeaf!]. List of tree leaves to get proofs for.
	 * @return Result object containing: root proof maxDepth node_index leaf leaf_index tree_id canopy_depth
	 */

	Variant fetchProofs(Array leaves);
	/**
	 * @brief Executes the find_accounts() GraphQL operation in Godot
	 * @param programId is Required parameter. Type: Bytes. Program ID to filter accounts by.
	 * @param discriminator Optional parameter. Type: Bytes. Default: PackedByteArray(). Discriminator to filter accounts by.
	 * @param parsedData Optional parameter. Type: JSON. Default: Dictionary(). Parsed data to filter accounts by.
	 * @return Result object containing: address program_id discriminator parsed_data
	 */

	Variant findAccounts(PackedByteArray programId, PackedByteArray discriminator = PackedByteArray(), Dictionary parsedData = Dictionary());
	/**
	 * @brief Send transaction bundles with options.
	 * @param txs is Required parameter. Type: [Bytes!]. List of transactions to send.
	 * @param blockhash is Required parameter. Type: String. Blockhash for the transactions.
	 * @param lastValidBlockHeight is Required parameter. Type: Int. Last valid block height.
	 * @param options Optional parameter. Type: SendTransactionBundlesOptions. Default: Variant(nullptr). Options for sending transactions.
	 * @return Result object containing: bundleId responses { signature error status }
	 */

	Variant sendTransactionBundles(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options = Variant(nullptr));
	/**
	 * @brief Sign with shadow signer and send transaction bundles.
	 * @param txs is Required parameter. Type: [Bytes!]. List of transactions to sign and send.
	 * @param blockhash is Required parameter. Type: String. Blockhash for the transactions.
	 * @param lastValidBlockHeight is Required parameter. Type: Int. Last valid block height.
	 * @param authToken is Required parameter. Type: String. Authentication token for the shadow signer.
	 * @param options Optional parameter. Type: SendTransactionBundlesOptions. Default: Variant(nullptr). Options for sending transactions.
	 * @return Result object containing: bundleId responses { signature error status }
	 */

	Variant signWithShadowSignerAndSendTransactionBundles(Array txs, String blockhash, int32_t lastValidBlockHeight, String authToken, Variant options = Variant(nullptr));
	/**
	 * @brief Send bulk transactions (deprecated).
	 * @param txs is Required parameter. Type: [Bytes!]. List of transactions to send.
	 * @param blockhash is Required parameter. Type: String. Blockhash for the transactions.
	 * @param lastValidBlockHeight is Required parameter. Type: Int. Last valid block height.
	 * @param options Optional parameter. Type: SendTransactionBundlesOptions. Default: Variant(nullptr). Options for sending transactions.
	 * @return Result object containing: signature error status
	 */

	Variant sendBulkTransactions(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options = Variant(nullptr));
	/**
	 * @brief Sign with shadow signer and send bulk transactions (deprecated).
	 * @param txs is Required parameter. Type: [Bytes!]. List of transactions to sign and send.
	 * @param blockhash is Required parameter. Type: String. Blockhash for the transactions.
	 * @param lastValidBlockHeight is Required parameter. Type: Int. Last valid block height.
	 * @param options Optional parameter. Type: SendTransactionBundlesOptions. Default: Variant(nullptr). Options for sending transactions.
	 * @return Result object containing: signature error status
	 */

	Variant signWithShadowSignerAndSendBulkTransactions(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options = Variant(nullptr));
	/**
	 * @brief Request authentication for a wallet.
	 * @param wallet is Required parameter. Type: String. Wallet address to authenticate.
	 * @param useTx Optional parameter. Type: Boolean. Default: false. Whether to use transaction-based authentication.
	 * @param useRpc Optional parameter. Type: String. Default: "". RPC endpoint to use.
	 * @return Result object containing: message tx
	 */

	Variant authRequest(String wallet, bool useTx = false, String useRpc = "");
	/**
	 * @brief Confirm authentication with a signature.
	 * @param wallet is Required parameter. Type: String. Wallet address to confirm.
	 * @param signature is Required parameter. Type: String. Signature to verify.
	 * @return Result object containing: accessToken user { address id info { bio name pfp } wallets { shadow wallets }  }
	 */

	Variant authConfirm(String wallet, String signature);
	/**
	 * @brief Create transactions for transferring assets.
	 * @param addresses is Required parameter. Type: [String!]. List of asset addresses to transfer.
	 * @param to is Required parameter. Type: String. Recipient address.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createTransferAssetsTransactions(PackedStringArray addresses, String to, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for burning assets.
	 * @param addresses is Required parameter. Type: [String!]. List of asset addresses to burn.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createBurnAssetsTransactions(PackedStringArray addresses, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Executes the find_global() GraphQL operation in Godot
	 * @param env Optional parameter. Type: String. Default: "". Environment to get configuration for.
	 * @return Result object containing: address bump config userTrees { active merkle_trees schema } totalUsers
	 */

	Variant findGlobal(String env = "");
	/**
	 * @brief Executes the find_projects() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of project addresses to search for.
	 * @param names Optional parameter. Type: [String!]. Default: PackedStringArray(). List of project names to search for.
	 * @param authorities Optional parameter. Type: [Pubkey!]. Default: Array(). List of project authorities to search for.
	 * @return Result object containing: associatedPrograms { address trustedActions { kind }  } authority bump driver address key name profileDataConfig { achievements customDataFields } profileTrees { active merkle_trees schema } badgeCriteria { index condition endTime startTime } services { kind params { ... on ServiceParamsAssembler { assemblerId } ... on ServiceParamsAssetManager { assetManagerId } ... on ServiceParamsStaking { poolId } ... on ServiceParamsMissions { poolId } ... on ServiceParamsRaffles { poolId } ... on ServiceParamsGuildKit { kitId }  }  } subsidyFees
	 */

	Variant findProjects(Array addresses = Array(), PackedStringArray names = PackedStringArray(), Array authorities = Array());
	/**
	 * @brief Executes the find_delegate_authority() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of authority addresses to search for.
	 * @param delegates Optional parameter. Type: [Pubkey!]. Default: Array(). List of delegate addresses to search for.
	 * @param projects Optional parameter. Type: [Pubkey!]. Default: Array(). List of projects to search in.
	 * @return Result object containing: authority bump delegations { kind index permission }
	 */

	Variant findDelegateAuthority(Array addresses = Array(), Array delegates = Array(), Array projects = Array());
	/**
	 * @brief Executes the find_users() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of user addresses to search for.
	 * @param ids Optional parameter. Type: [Int!]. Default: PackedInt32Array(). List of user IDs to search for.
	 * @param wallets Optional parameter. Type: [Pubkey!]. Default: Array(). List of wallet addresses to search for.
	 * @param includeProof Optional parameter. Type: Boolean. Default: false. Whether to include proof in the response.
	 * @param includeProjectProfiles Optional parameter. Type: [Pubkey!]. Default: Array(). List of project profiles to include.
	 * @return Result object containing: address id info { bio name pfp username } socialInfo { discord twitter civic { expiry gatekeeperNetwork walletIndex } steam } wallets { shadow wallets } profiles { customData address identity info { bio name pfp } platformData { achievements custom xp } project userId leaf_idx tree_id proof { leaf leaf_index node_index proof maxDepth root tree_id canopy_depth }  } leaf_idx tree_id proof { leaf leaf_index node_index maxDepth proof root tree_id canopy_depth }
	 */

	Variant findUsers(Array addresses = Array(), PackedInt32Array ids = PackedInt32Array(), Array wallets = Array(), bool includeProof = false, Array includeProjectProfiles = Array());
	/**
	 * @brief Executes the find_profiles() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of profile addresses to search for.
	 * @param projects Optional parameter. Type: [Pubkey!]. Default: Array(). List of projects to search in.
	 * @param userIds Optional parameter. Type: [Int!]. Default: PackedInt32Array(). List of user IDs to search for.
	 * @param identities Optional parameter. Type: [String!]. Default: PackedStringArray(). List of identities to search for.
	 * @param includeProof Optional parameter. Type: Boolean. Default: false. Whether to include proof in the response.
	 * @param includeUsers Optional parameter. Type: Boolean. Default: false. Whether to include user information.
	 * @return Result object containing: customData address identity info { bio name pfp } platformData { achievements custom xp } project userId leaf_idx tree_id proof { leaf leaf_index node_index proof maxDepth root tree_id canopy_depth } user { address id info { bio name pfp username } socialInfo { discord twitter civic { expiry gatekeeperNetwork walletIndex } steam } wallets { shadow wallets } leaf_idx tree_id proof { leaf leaf_index node_index maxDepth proof root tree_id canopy_depth }  }
	 */

	Variant findProfiles(Array addresses = Array(), Array projects = Array(), PackedInt32Array userIds = PackedInt32Array(), PackedStringArray identities = PackedStringArray(), bool includeProof = false, bool includeUsers = false);
	/**
	 * @brief Create a transaction for creating a new user.
	 * @param wallet is Required parameter. Type: String. Wallet address for the new user.
	 * @param info Optional parameter. Type: UserInfoInput. Default: Variant(nullptr). User information for the new user.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createNewUserTransaction(String wallet, Variant info = Variant(nullptr), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating multiple new users.
	 * @param wallet is Required parameter. Type: [String!]. List of wallet addresses for the new users.
	 * @param info is Required parameter. Type: [UserInfoInput!]. List of user information for the new users.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createNewUserBulkTransaction(PackedStringArray wallet, Array info, String payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating a user.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param info Optional parameter. Type: PartialUserInfoInput. Default: Variant(nullptr). Updated user information.
	 * @param wallets Optional parameter. Type: UpdateWalletInput. Default: Variant(nullptr). Wallet updates.
	 * @param populateCivic Optional parameter. Type: Boolean. Default: false. Whether to populate Civic information.
	 * @param removeSocials Optional parameter. Type: [RemoveSocialsInput!]. Default: Array(). List of socials to remove.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createUpdateUserTransaction(String payer, Variant info = Variant(nullptr), Variant wallets = Variant(nullptr), bool populateCivic = false, Array removeSocials = Array(), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a new project.
	 * @param authority is Required parameter. Type: String. Authority for the new project.
	 * @param name is Required parameter. Type: String. Name of the new project.
	 * @param driver Optional parameter. Type: String. Default: "". Driver for the project (optional).
	 * @param associatedPrograms Optional parameter. Type: [AssociatedProgramInput!]. Default: Array(). List of associated programs.
	 * @param profileDataConfig Optional parameter. Type: ProfileDataConfigInput. Default: Variant(nullptr). Profile data configuration.
	 * @param subsidizeFees Optional parameter. Type: Boolean. Default: false. Whether to subsidize fees.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: project tx { blockhash lastValidBlockHeight transaction }
	 */

	Variant createCreateProjectTransaction(String authority, String name, String driver = "", Array associatedPrograms = Array(), Variant profileDataConfig = Variant(nullptr), bool subsidizeFees = false, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for changing a project's driver.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param driver is Required parameter. Type: String. New driver address.
	 * @param authority is Required parameter. Type: String. Authority for the change.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createChangeProjectDriverTransaction(String project, String driver, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a delegate authority.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param delegate is Required parameter. Type: String. Delegate address.
	 * @param serviceDelegations is Required parameter. Type: ServiceDelegationInput. Service delegations to create.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createCreateDelegateAuthorityTransaction(String project, String delegate, Variant serviceDelegations, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for modifying a delegation.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param delegate is Required parameter. Type: String. Delegate address.
	 * @param modifyDelegation is Required parameter. Type: ModifyDelegationInput. Delegation modifications.
	 * @param authority is Required parameter. Type: String. Authority for the modification.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createModifyDelegationTransaction(String project, String delegate, Variant modifyDelegation, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a profiles tree.
	 * @param treeConfig is Required parameter. Type: TreeSetupConfig. Configuration for the tree setup.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { blockhash lastValidBlockHeight transaction } treeAddress proofBytes space cost maxTreeCapacity
	 */

	Variant createCreateProfilesTreeTransaction(Variant treeConfig, String project, String payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a new profile.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param identity Optional parameter. Type: String. Default: "". Identity for the profile (optional).
	 * @param info Optional parameter. Type: ProfileInfoInput. Default: Variant(nullptr). Profile information.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createNewProfileTransaction(String project, String payer, String identity = "", Variant info = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating a profile.
	 * @param profile is Required parameter. Type: String. Profile address.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param info Optional parameter. Type: ProfileInfoInput. Default: Variant(nullptr). Updated profile information.
	 * @param customData Optional parameter. Type: CustomDataInput. Default: Variant(nullptr). Custom data updates.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createUpdateProfileTransaction(String profile, String payer, Variant info = Variant(nullptr), Variant customData = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating platform data.
	 * @param profile is Required parameter. Type: String. Profile address.
	 * @param authority is Required parameter. Type: String. Authority for the update.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param platformData Optional parameter. Type: PlatformDataInput. Default: Variant(nullptr). Platform data updates.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createUpdatePlatformDataTransaction(String profile, String authority, String payer = "", Variant platformData = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a new user with a profile.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param wallet is Required parameter. Type: String. Wallet address for the new user.
	 * @param userInfo is Required parameter. Type: UserInfoInput. User information for the new user.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param profileIdentity Optional parameter. Type: String. Default: "". Identity for the profile (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createNewUserWithProfileTransaction(String project, String wallet, Variant userInfo, String payer = "", String profileIdentity = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Executes the find_assembler_config() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of assembler configuration addresses.
	 * @param tickers Optional parameter. Type: [String!]. Default: PackedStringArray(). List of tickers to filter by.
	 * @param characterModel Optional parameter. Type: [Bytes!]. Default: Array(). List of character model addresses.
	 * @param project Optional parameter. Type: [Pubkey!]. Default: Array(). List of project addresses.
	 * @return Result object containing: address program_id discriminator ticker project order merkle_trees { active merkle_trees schema }
	 */

	Variant findAssemblerConfig(Array addresses = Array(), PackedStringArray tickers = PackedStringArray(), Array characterModel = Array(), Array project = Array());
	/**
	 * @brief Executes the find_character_traits() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of trait addresses to search for.
	 * @param trees Optional parameter. Type: [Bytes!]. Default: Array(). List of tree addresses to search in.
	 * @param includeProof Optional parameter. Type: Boolean. Default: false. Whether to include proof in the response.
	 * @return Result object containing: address layer leaf_idx name proof { root proof node_index leaf maxDepth leaf_index tree_id canopy_depth } tree_id uri
	 */

	Variant findCharacterTraits(Array addresses = Array(), Array trees = Array(), bool includeProof = false);
	/**
	 * @brief Executes the find_character_models() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of model addresses to search for.
	 * @param project Optional parameter. Type: Pubkey. Default: Variant(nullptr). Project address to filter by.
	 * @param assemblerConfigs Optional parameter. Type: [Pubkey!]. Default: Array(). List of assembler configuration addresses.
	 * @return Result object containing: address program_id discriminator bump key equipableCriteria project attributes merkle_trees { active merkle_trees schema } cooldown { ejection } config { ... on CharacterConfigWrapped { criterias { kind params } kind } ... on CharacterConfigAssembled { kind assemblerConfig name symbol description creators { address share } sellerFeeBasisPoints collectionName mintAs { kind params { ... on MintAsParamsMplBubblegum { merkleTree }  }  }  }  }
	 */

	Variant findCharacterModels(Array addresses = Array(), Variant project = Variant(nullptr), Array assemblerConfigs = Array());
	/**
	 * @brief Executes the find_characters() GraphQL operation in Godot
	 * @param includeProof Optional parameter. Type: Boolean. Default: false. Whether to include proof in the response.
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of character addresses to search for.
	 * @param filters Optional parameter. Type: CharactersFilter. Default: Variant(nullptr). Additional filters for characters.
	 * @param trees Optional parameter. Type: [Bytes!]. Default: Array(). List of tree addresses to search in.
	 * @param wallets Optional parameter. Type: [Pubkey!]. Default: Array(). List of wallet addresses to filter by.
	 * @param mints Optional parameter. Type: [Pubkey!]. Default: Array(). List of mint addresses to filter by.
	 * @param attributeHashes Optional parameter. Type: [Pubkey!]. Default: Array(). List of attribute hashes to filter by.
	 * @return Result object containing: source { kind params { ... on Wrapped { mint criteria { kind params } kind } ... on Assembled { hash mint uri attributes assemblerConfig }  } kind } owner proof { leaf leaf_index node_index maxDepth proof root tree_id canopy_depth } usedBy { kind params { ... on UsedByStaking { pool staker stakedAt claimedAt } ... on UsedByMission { missionId participationId rewards { delta rewardIdx collected } endTime } ... on UsedByGuild { id role { kind } order } ... on UsedByEjected { mint } ... on UsedByCustom { user data }  }  } equipments leaf_idx tree_id address asset cooldown { ejection }
	 */

	Variant findCharacters(bool includeProof = false, Array addresses = Array(), Variant filters = Variant(nullptr), Array trees = Array(), Array wallets = Array(), Array mints = Array(), Array attributeHashes = Array());
	/**
	 * @brief Executes the fetch_character_history() GraphQL operation in Godot
	 * @param addresses is Required parameter. Type: [Bytes!]. List of character addresses to get history for.
	 * @param event Optional parameter. Type: [String!]. Default: PackedStringArray(). List of event types to filter by.
	 * @return Result object containing: address event event_data time
	 */

	Variant fetchCharacterHistory(Array addresses, PackedStringArray event = PackedStringArray());
	/**
	 * @brief Create a transaction for creating an assembler configuration.
	 * @param ticker is Required parameter. Type: String. Ticker for the assembler configuration.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param treeConfig Optional parameter. Type: TreeSetupConfig. Default: Variant(nullptr). Tree configuration for the assembler.
	 * @param order Optional parameter. Type: [String!]. Default: PackedStringArray(). Order of traits.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { blockhash lastValidBlockHeight transaction } assemblerConfig treeAddress proofBytes space cost maxTreeCapacity
	 */

	Variant createCreateAssemblerConfigTransaction(String ticker, String project, String authority, Variant treeConfig = Variant(nullptr), PackedStringArray order = PackedStringArray(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for adding character traits.
	 * @param assemblerConfig is Required parameter. Type: String. Assembler configuration address.
	 * @param traits is Required parameter. Type: [CharacterTraitInput!]. List of traits to add.
	 * @param authority is Required parameter. Type: String. Authority for the addition.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createAddCharacterTraitsTransactions(String assemblerConfig, Array traits, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for removing character traits.
	 * @param assemblerConfig is Required parameter. Type: String. Assembler configuration address.
	 * @param traitsAddresses is Required parameter. Type: [String!]. List of trait addresses to remove.
	 * @param authority is Required parameter. Type: String. Authority for the removal.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createRemoveCharacterTraitsTransactions(String assemblerConfig, PackedStringArray traitsAddresses, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a character model.
	 * @param config is Required parameter. Type: CharacterConfigInput. Configuration for the character model.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param attributes Optional parameter. Type: VecMapGeneric. Default: Array(). Attributes for the character model.
	 * @param mintAs Optional parameter. Type: MintAsInput. Default: Variant(nullptr). Mint configuration for the character model.
	 * @param cooldown Optional parameter. Type: CharacterCooldownInput. Default: Variant(nullptr). Cooldown configuration for the character model.
	 * @param equipableCriteria Optional parameter. Type: [String!]. Default: PackedStringArray(). List of equipable criteria.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: characterModel tx { blockhash lastValidBlockHeight transaction }
	 */

	Variant createCreateCharacterModelTransaction(Variant config, String project, String authority, Array attributes = Array(), Variant mintAs = Variant(nullptr), Variant cooldown = Variant(nullptr), PackedStringArray equipableCriteria = PackedStringArray(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a characters tree.
	 * @param treeConfig is Required parameter. Type: TreeSetupConfig. Tree configuration.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { blockhash lastValidBlockHeight transaction } treeAddress proofBytes space cost maxTreeCapacity
	 */

	Variant createCreateCharactersTreeTransaction(Variant treeConfig, String project, String characterModel, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for assembling a character.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param assemblerConfig is Required parameter. Type: String. Assembler configuration address.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param owner is Required parameter. Type: String. Owner address.
	 * @param authority is Required parameter. Type: String. Authority for the assembly.
	 * @param uri Optional parameter. Type: String. Default: "". URI for the character.
	 * @param attributes Optional parameter. Type: VecMapGeneric. Default: Array(). Attributes for the character.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createAssembleCharacterTransaction(String project, String assemblerConfig, String characterModel, String owner, String authority, String uri = "", Array attributes = Array(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating character traits.
	 * @param characterAddress is Required parameter. Type: String. Character address to update.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param assemblerConfig is Required parameter. Type: String. Assembler configuration address.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param authority is Required parameter. Type: String. Authority for the update.
	 * @param uri Optional parameter. Type: String. Default: "". New URI for the character.
	 * @param attributes Optional parameter. Type: VecMapGeneric. Default: Array(). New attributes for the character.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createUpdateCharacterTraitsTransaction(String characterAddress, String project, String assemblerConfig, String characterModel, String authority, String uri = "", Array attributes = Array(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for populating a character.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param mint is Required parameter. Type: String. Mint address.
	 * @param owner is Required parameter. Type: String. Owner address.
	 * @param updateAuthority is Required parameter. Type: String. Update authority address.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param attributes Optional parameter. Type: VecMapGeneric. Default: Array(). Attributes for the character.
	 * @param assemblerConfig Optional parameter. Type: String. Default: "". Assembler configuration address (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createPopulateCharacterTransaction(String project, String characterModel, String mint, String owner, String updateAuthority, String payer, Array attributes = Array(), String assemblerConfig = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for wrapping assets to a character.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param wallet is Required parameter. Type: String. Wallet address.
	 * @param mintList is Required parameter. Type: [String!]. List of mint addresses to wrap.
	 * @param libreplexDeployment Optional parameter. Type: String. Default: "". Libreplex deployment address (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transactions blockhash lastValidBlockHeight
	 */

	Variant createWrapAssetsToCharacterTransactions(String project, String characterModel, String wallet, PackedStringArray mintList, String libreplexDeployment = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for unwrapping assets from a character.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param characterAddresses is Required parameter. Type: [String!]. List of character addresses to unwrap from.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param wallet is Required parameter. Type: String. Wallet address.
	 * @param libreplexDeployment Optional parameter. Type: String. Default: "". Libreplex deployment address (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createUnwrapAssetsFromCharacterTransactions(String project, PackedStringArray characterAddresses, String characterModel, String wallet, String libreplexDeployment = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for equipping a resource on a character.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param characterAddress is Required parameter. Type: String. Character address.
	 * @param resource is Required parameter. Type: String. Resource address.
	 * @param amount is Required parameter. Type: BigInt. Amount of resource to equip.
	 * @param owner is Required parameter. Type: String. Owner address.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createEquipResourceOnCharacterTransaction(String characterModel, String characterAddress, String resource, int64_t amount, String owner, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for dismounting a resource from a character.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param characterAddress is Required parameter. Type: String. Character address.
	 * @param resource is Required parameter. Type: String. Resource address.
	 * @param amount is Required parameter. Type: BigInt. Amount of resource to dismount.
	 * @param owner is Required parameter. Type: String. Owner address.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createDismountResourceOnCharacterTransaction(String characterModel, String characterAddress, String resource, int64_t amount, String owner, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for using a character.
	 * @param character is Required parameter. Type: String. Character address.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param user is Required parameter. Type: String. User address.
	 * @param data Optional parameter. Type: JSON. Default: Dictionary(). Data for using the character.
	 * @param unUse Optional parameter. Type: Boolean. Default: false. Whether to un-use the character.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createUseCharacterTransaction(String character, String project, String characterModel, String user, Dictionary data = Dictionary(), bool unUse = false, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Executes the find_staking_pools() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of staking pool addresses to search for.
	 * @param projects Optional parameter. Type: [Pubkey!]. Default: Array(). List of project addresses to filter by.
	 * @param names Optional parameter. Type: [String!]. Default: PackedStringArray(). List of pool names to filter by.
	 * @return Result object containing: allowedMints bump characterModels cooldownDuration resource endTime key lockType { kind } maxRewardsDuration minStakeDuration name project resetStakeDuration rewardsDuration rewardsPerDuration startTime totalStaked address
	 */

	Variant findStakingPools(Array addresses = Array(), Array projects = Array(), PackedStringArray names = PackedStringArray());
	/**
	 * @brief Executes the find_stakers() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of staker addresses to search for.
	 * @param stakingPools Optional parameter. Type: [Pubkey!]. Default: Array(). List of staking pool addresses to filter by.
	 * @param wallets Optional parameter. Type: [Pubkey!]. Default: Array(). List of wallet addresses to filter by.
	 * @return Result object containing: bump stakingPool totalStaked wallet
	 */

	Variant findStakers(Array addresses = Array(), Array stakingPools = Array(), Array wallets = Array());
	/**
	 * @brief Executes the find_multipliers() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of multiplier addresses to search for.
	 * @param stakingPools Optional parameter. Type: [Pubkey!]. Default: Array(). List of staking pool addresses to filter by.
	 * @return Result object containing: bump collectionMultipliers { multiplierType { kind params { ... on MultiplierTypeParamsStakeDuration { minDuration } ... on MultiplierTypeParamsNFTCount { minCount } ... on MultiplierTypeParamsCreator { creator } ... on MultiplierTypeParamsCollection { collection }  }  } value } countMultipliers { multiplierType { kind params { ... on MultiplierTypeParamsStakeDuration { minDuration } ... on MultiplierTypeParamsNFTCount { minCount } ... on MultiplierTypeParamsCreator { creator } ... on MultiplierTypeParamsCollection { collection }  }  } value } creatorMultipliers { multiplierType { kind params { ... on MultiplierTypeParamsStakeDuration { minDuration } ... on MultiplierTypeParamsNFTCount { minCount } ... on MultiplierTypeParamsCreator { creator } ... on MultiplierTypeParamsCollection { collection }  }  } value } decimals durationMultipliers { multiplierType { kind params { ... on MultiplierTypeParamsStakeDuration { minDuration } ... on MultiplierTypeParamsNFTCount { minCount } ... on MultiplierTypeParamsCreator { creator } ... on MultiplierTypeParamsCollection { collection }  }  } value } address stakingPool
	 */

	Variant findMultipliers(Array addresses = Array(), Array stakingPools = Array());
	/**
	 * @brief Create a transaction for creating a staking pool.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param resource is Required parameter. Type: String. Resource address.
	 * @param metadata is Required parameter. Type: CreateStakingPoolMetadataInput. Metadata for the staking pool.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param multiplier Optional parameter. Type: InitStakingMultiplierMetadataInput. Default: Variant(nullptr). Multiplier configuration (optional).
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transactions { transactions blockhash lastValidBlockHeight } stakingPoolAddress multipliersAddress
	 */

	Variant createCreateStakingPoolTransaction(String project, String resource, Variant metadata, String authority, Variant multiplier = Variant(nullptr), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating a staking pool.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param stakingPool is Required parameter. Type: String. Staking pool address.
	 * @param authority is Required parameter. Type: String. Authority for the update.
	 * @param metadata Optional parameter. Type: UpdateStakingPoolMetadataInput. Default: Variant(nullptr). Updated metadata for the pool (optional).
	 * @param characterModel Optional parameter. Type: String. Default: "". Character model address (optional).
	 * @param resource Optional parameter. Type: String. Default: "". Resource address (optional).
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createUpdateStakingPoolTransaction(String project, String stakingPool, String authority, Variant metadata = Variant(nullptr), String characterModel = "", String resource = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for initializing multipliers.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param stakingPool is Required parameter. Type: String. Staking pool address.
	 * @param decimals is Required parameter. Type: Int. Number of decimal places for multipliers.
	 * @param multipliers is Required parameter. Type: [AddMultiplierMetadataInput!]. List of multiplier configurations.
	 * @param authority is Required parameter. Type: String. Authority for the initialization.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { transaction blockhash lastValidBlockHeight } multipliersAddress
	 */

	Variant createInitMultipliersTransaction(String project, String stakingPool, int32_t decimals, Array multipliers, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for adding a multiplier.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param multiplier is Required parameter. Type: String. Multiplier address.
	 * @param metadata is Required parameter. Type: AddMultiplierMetadataInput. Metadata for the new multiplier.
	 * @param authority is Required parameter. Type: String. Authority for the addition.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createAddMultiplierTransaction(String project, String multiplier, Variant metadata, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for staking characters.
	 * @param characterAddresses is Required parameter. Type: [String!]. List of character addresses to stake.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param stakingPool is Required parameter. Type: String. Staking pool address.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param feePayer Optional parameter. Type: String. Default: "". Fee payer for the transactions (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createStakeCharactersTransactions(PackedStringArray characterAddresses, String characterModel, String stakingPool, String project, String feePayer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Executes the find_spl_staking_pools() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of pool addresses to search for.
	 * @param projects Optional parameter. Type: [Pubkey!]. Default: Array(). List of project addresses to filter by.
	 * @param names Optional parameter. Type: [String!]. Default: PackedStringArray(). List of pool names to filter by.
	 * @param stakeTokenMints Optional parameter. Type: [Pubkey!]. Default: Array(). List of stake token mint addresses to filter by.
	 * @return Result object containing: address bump nonce endTime maxStakeDurationSecs merkleTrees { active merkle_trees schema } minStakeDurationSecs multipliers { value multiplierType { kind params { ... on SplMultiplierTypeParamsStakeDuration { __typename minDuration } ... on SplMultiplierTypeParamsStakeAmount { __typename minAmount }  }  }  } name project rewardConfig { kind params { ... on SplRewardConfigParamsApy { rewardTokenMint rewardVault rewardsDuration rewardsPerDuration totalRewardAmount } ... on SplRewardConfigParamsStakeWeight { __typename pools { weight }  }  }  } stake_token_mint startTime totalStakedAmount
	 */

	Variant findSplStakingPools(Array addresses = Array(), Array projects = Array(), PackedStringArray names = PackedStringArray(), Array stakeTokenMints = Array());
	/**
	 * @brief Executes the find_spl_staking_recipients() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of recipient addresses to search for.
	 * @param trees Optional parameter. Type: [Bytes!]. Default: Array(). List of tree addresses to search in.
	 * @param stakers Optional parameter. Type: [String!]. Default: PackedStringArray(). List of staker addresses to filter by.
	 * @param projects Optional parameter. Type: [String!]. Default: PackedStringArray(). List of project addresses to filter by.
	 * @param splStakingPools Optional parameter. Type: [String!]. Default: PackedStringArray(). List of SPL staking pool addresses to filter by.
	 * @param includeProof Optional parameter. Type: Boolean. Default: false. Whether to include proof in the response.
	 * @return Result object containing: address leaf_idx lokedTill multiplier proof { canopy_depth leaf leaf_index maxDepth node_index proof root tree_id } rewardsClaimed { kind params { ... on StakerClaimedParamsApy { __typename lastRewardTime } ... on StakerClaimedParamsStakeWeight { __typename amounts }  }  } stakeStartTime staker stakedAmount tree_id
	 */

	Variant findSplStakingRecipients(Array addresses = Array(), Array trees = Array(), PackedStringArray stakers = PackedStringArray(), PackedStringArray projects = PackedStringArray(), PackedStringArray splStakingPools = PackedStringArray(), bool includeProof = false);
	/**
	 * @brief Create a transaction for creating an SPL staking pool.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param stakeTokenMint is Required parameter. Type: String. Stake token mint address.
	 * @param metadata is Required parameter. Type: CreateSplStakingPoolMetadataInput. Metadata for the staking pool.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param multipliers Optional parameter. Type: [AddSplMultiplierMetadataInput!]. Default: Array(). List of multiplier configurations.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { blockhash lastValidBlockHeight transaction } splStakingPoolAddress
	 */

	Variant createCreateSplStakingPoolTransaction(String project, String stakeTokenMint, Variant metadata, String authority, Array multipliers = Array(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating an SPL staking pool.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param splStakingPool is Required parameter. Type: String. SPL staking pool address.
	 * @param metadata is Required parameter. Type: UpdateSplStakingPoolMetadataInput. Updated metadata for the pool.
	 * @param authority is Required parameter. Type: String. Authority for the update.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createUpdateSplStakingPoolTransaction(String project, String splStakingPool, Variant metadata, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a new SPL staking pool tree.
	 * @param treeConfig is Required parameter. Type: TreeSetupConfig. Tree configuration.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param splStakingPool is Required parameter. Type: String. SPL staking pool address.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { blockhash lastValidBlockHeight transaction } treeAddress proofBytes space cost maxTreeCapacity
	 */

	Variant createCreateNewSplStakingPoolTreeTransaction(Variant treeConfig, String project, String splStakingPool, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for adding or removing SPL multipliers.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param splStakingPool is Required parameter. Type: String. SPL staking pool address.
	 * @param authority is Required parameter. Type: String. Authority for the modification.
	 * @param add Optional parameter. Type: [AddSplMultiplierMetadataInput!]. Default: Array(). List of multipliers to add.
	 * @param remove Optional parameter. Type: [Int!]. Default: PackedInt32Array(). List of multiplier indices to remove.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createAddRemoveSplMultipliersTransaction(String project, String splStakingPool, String authority, Array add = Array(), PackedInt32Array remove = PackedInt32Array(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating an SPL reward pool.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param splStakingPool is Required parameter. Type: String. SPL staking pool address.
	 * @param rewardTokenMint is Required parameter. Type: String. Reward token mint address.
	 * @param rewardConfig is Required parameter. Type: CreateSplRewardPoolArgsInput. Reward pool configuration.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createSplRewardPoolTransaction(String project, String splStakingPool, String rewardTokenMint, Variant rewardConfig, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for adding or removing rewards from a reward pool.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param splStakingPool is Required parameter. Type: String. SPL staking pool address.
	 * @param rewardTokenMint is Required parameter. Type: String. Reward token mint address.
	 * @param action is Required parameter. Type: AddRemoveRewardsFromRewardPoolActionInput. Action to perform on the reward pool.
	 * @param authority is Required parameter. Type: String. Authority for the modification.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createAddRemoveRewardsFromRewardPoolTransaction(String project, String splStakingPool, String rewardTokenMint, Variant action, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for staking SPL tokens.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param splStakingPool is Required parameter. Type: String. SPL staking pool address.
	 * @param amount is Required parameter. Type: BigInt. Amount of tokens to stake.
	 * @param lockPeriodSecs is Required parameter. Type: BigInt. Lock period in seconds.
	 * @param staker is Required parameter. Type: String. Staker address.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createStakeSplTokensTransaction(String project, String splStakingPool, int64_t amount, int64_t lockPeriodSecs, String staker, String payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for claiming SPL rewards.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param splStakingPool is Required parameter. Type: String. SPL staking pool address.
	 * @param staker is Required parameter. Type: String. Staker address.
	 * @param stakingReciept is Required parameter. Type: String. Staking receipt address.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createClaimSplRewardsTransaction(String project, String splStakingPool, String staker, String stakingReciept, String payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for unstaking SPL tokens.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param splStakingPool is Required parameter. Type: String. SPL staking pool address.
	 * @param staker is Required parameter. Type: String. Staker address.
	 * @param stakingReciept is Required parameter. Type: String. Staking receipt address.
	 * @param payer is Required parameter. Type: String. Payer for the transaction.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createUnstakeSplTokensTransaction(String project, String splStakingPool, String staker, String stakingReciept, String payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for claiming staking rewards.
	 * @param characterAddresses is Required parameter. Type: [String!]. List of character addresses to claim rewards for.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param feePayer Optional parameter. Type: String. Default: "". Fee payer for the transactions (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createClaimStakingRewardsTransactions(PackedStringArray characterAddresses, String characterModel, String feePayer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for unstaking characters.
	 * @param characterAddresses is Required parameter. Type: [String!]. List of character addresses to unstake.
	 * @param characterModel is Required parameter. Type: String. Character model address.
	 * @param feePayer Optional parameter. Type: String. Default: "". Fee payer for the transactions (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createUnstakeCharactersTransactions(PackedStringArray characterAddresses, String characterModel, String feePayer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Executes the find_missions() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of mission addresses to search for.
	 * @param missionPools Optional parameter. Type: [Pubkey!]. Default: Array(). List of mission pool addresses to filter by.
	 * @return Result object containing: project address program_id discriminator bump missionPool name minXp cost { amount resource_address } requirement { ... on TimeRequirement { kind params { duration }  }  } rewards { min max rewardType { ... on XpRewardType { kind } ... on ResourceRewardType { kind params { address }  }  }  }
	 */

	Variant findMissions(Array addresses = Array(), Array missionPools = Array());
	/**
	 * @brief Executes the find_mission_pools() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of mission pool addresses to search for.
	 * @param project Optional parameter. Type: Pubkey. Default: Variant(nullptr). Project address to filter by.
	 * @return Result object containing: address program_id discriminator bump project name factionsMerkleRoot randomizerRound characterModels guildKits
	 */

	Variant findMissionPools(Array addresses = Array(), Variant project = Variant(nullptr));
	/**
	 * @brief Create a transaction for creating a mission pool.
	 * @param data is Required parameter. Type: NewMissionPoolData. Data for the new mission pool.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { transaction blockhash lastValidBlockHeight } missionPoolAddress
	 */

	Variant createCreateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating a mission pool.
	 * @param data is Required parameter. Type: UpdateMissionPoolData. Data for updating the mission pool.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { transaction blockhash lastValidBlockHeight }
	 */

	Variant createUpdateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a mission.
	 * @param data is Required parameter. Type: NewMissionData. Data for the new mission.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { transaction blockhash lastValidBlockHeight } missionAddress
	 */

	Variant createCreateMissionTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating a mission.
	 * @param missionAddress is Required parameter. Type: String. Address of the mission to update.
	 * @param authority is Required parameter. Type: String. Authority for the update.
	 * @param params is Required parameter. Type: UpdateMissionInput. Parameters for updating the mission.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createUpdateMissionTransaction(String missionAddress, String authority, Variant params, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for sending characters on a mission.
	 * @param data is Required parameter. Type: ParticipateOnMissionData. Data for participating in the mission.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transactions blockhash lastValidBlockHeight
	 */

	Variant createSendCharactersOnMissionTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for recalling characters from a mission.
	 * @param data is Required parameter. Type: RecallFromMissionData. Data for recalling from the mission.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transactions blockhash lastValidBlockHeight
	 */

	Variant createRecallCharactersTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Executes the find_resources() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of resource addresses to search for.
	 * @param projects Optional parameter. Type: [Pubkey!]. Default: Array(). List of project addresses to filter by.
	 * @param mints Optional parameter. Type: [Pubkey!]. Default: Array(). List of mint addresses to filter by.
	 * @return Result object containing: address bump kind { kind params { ... on ResourceKindParamsHplFungible { __typename decimals } ... on ResourceKindParamsWrappedFungible { __typename decimals custody { kind params { burnerDestination }  }  } ... on ResourceKindParamsHplNonFungible { __typename characteristics } ... on ResourceKindParamsWrappedMplCore { __typename characteristics }  }  } mint project tags storage { params { promiseSupply merkle_trees { active merkle_trees schema }  } kind }
	 */

	Variant findResources(Array addresses = Array(), Array projects = Array(), Array mints = Array());
	/**
	 * @brief Executes the find_resources_balance() GraphQL operation in Godot
	 * @param wallets is Required parameter. Type: [String!]. List of wallet addresses to get balances for.
	 * @param addresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of resource addresses to get balances for.
	 * @param projects Optional parameter. Type: [String!]. Default: PackedStringArray(). List of project addresses to filter by.
	 * @param mints Optional parameter. Type: [String!]. Default: PackedStringArray(). List of mint addresses to filter by.
	 * @return Result object containing: address amount mint wallet resourceStorage
	 */

	Variant findResourcesBalance(PackedStringArray wallets, PackedStringArray addresses = PackedStringArray(), PackedStringArray projects = PackedStringArray(), PackedStringArray mints = PackedStringArray());
	/**
	 * @brief Executes the find_faucets() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of faucet addresses to search for.
	 * @param projects Optional parameter. Type: [Pubkey!]. Default: Array(). List of project addresses to filter by.
	 * @param resources Optional parameter. Type: [Pubkey!]. Default: Array(). List of resource addresses to filter by.
	 * @return Result object containing: resource repeatInterval project amount lastClaimed address
	 */

	Variant findFaucets(Array addresses = Array(), Array projects = Array(), Array resources = Array());
	/**
	 * @brief Executes the find_recipes() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of recipe addresses to search for.
	 * @param projects Optional parameter. Type: [Pubkey!]. Default: Array(). List of project addresses to filter by.
	 * @return Result object containing: address bump ingredients { amount resourceAddress } key meal { amount resourceAddress } project xp
	 */

	Variant findRecipes(Array addresses = Array(), Array projects = Array());
	/**
	 * @brief Executes the find_holdings() GraphQL operation in Godot
	 * @param addresses Optional parameter. Type: [Bytes!]. Default: Array(). List of holding addresses to search for.
	 * @param holders Optional parameter. Type: [Pubkey!]. Default: Array(). List of holder addresses to filter by.
	 * @param trees Optional parameter. Type: [Bytes!]. Default: Array(). List of tree addresses to search in.
	 * @param includeProof Optional parameter. Type: Boolean. Default: false. Whether to include proof in the response.
	 * @return Result object containing: address balance characteristics holder leaf_idx tree_id proof { canopy_depth leaf leaf_index maxDepth node_index proof root tree_id }
	 */

	Variant findHoldings(Array addresses = Array(), Array holders = Array(), Array trees = Array(), bool includeProof = false);
	/**
	 * @brief Create a transaction for creating a new resource.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param params is Required parameter. Type: InitResourceInput. Parameters for initializing the resource.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { transaction blockhash lastValidBlockHeight } resource
	 */

	Variant createCreateNewResourceTransaction(String project, String authority, Variant params, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for importing a fungible resource.
	 * @param params is Required parameter. Type: ImportResourceInput. Parameters for importing the resource.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: resource tx { blockhash lastValidBlockHeight transaction }
	 */

	Variant createImportFungibleResourceTransaction(Variant params, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for exporting a fungible resource.
	 * @param resource is Required parameter. Type: String. Resource address to export.
	 * @param authority is Required parameter. Type: String. Authority for the export.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: blockhash lastValidBlockHeight transaction
	 */

	Variant createExportFungibleResourceTransaction(String resource, String authority, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for creating a new resource tree.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param resource is Required parameter. Type: String. Resource address.
	 * @param authority is Required parameter. Type: String. Authority for the creation.
	 * @param treeConfig is Required parameter. Type: TreeSetupConfig. Tree configuration.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { blockhash lastValidBlockHeight transaction } treeAddress proofBytes space cost maxTreeCapacity
	 */

	Variant createCreateNewResourceTreeTransaction(String project, String resource, String authority, Variant treeConfig, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for minting a resource.
	 * @param resource is Required parameter. Type: String. Resource address to mint.
	 * @param owner is Required parameter. Type: String. Owner address.
	 * @param authority is Required parameter. Type: String. Authority for the minting.
	 * @param amount is Required parameter. Type: BigInt. Amount to mint.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createMintResourceTransaction(String resource, String owner, String authority, int64_t amount, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for burning a resource.
	 * @param resource is Required parameter. Type: String. Resource address to burn.
	 * @param amount is Required parameter. Type: BigInt. Amount to burn.
	 * @param authority is Required parameter. Type: String. Authority for the burning.
	 * @param owner Optional parameter. Type: String. Default: "". Owner address (optional).
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createBurnResourceTransaction(String resource, int64_t amount, String authority, String owner = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for transferring a resource.
	 * @param resource is Required parameter. Type: String. Resource address to transfer.
	 * @param owner is Required parameter. Type: String. Owner address.
	 * @param recipient is Required parameter. Type: String. Recipient address.
	 * @param amount is Required parameter. Type: BigInt. Amount to transfer.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createTransferResourceTransaction(String resource, String owner, String recipient, int64_t amount, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for wrapping a holding.
	 * @param resource is Required parameter. Type: String. Resource address.
	 * @param amount is Required parameter. Type: BigInt. Amount to wrap.
	 * @param authority is Required parameter. Type: String. Authority for the wrapping.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createCreateWrapHoldingTransaction(String resource, int64_t amount, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for unwrapping a holding.
	 * @param resource is Required parameter. Type: String. Resource address.
	 * @param amount is Required parameter. Type: BigInt. Amount to unwrap.
	 * @param authority is Required parameter. Type: String. Authority for the unwrapping.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createCreateUnwrapHoldingTransaction(String resource, int64_t amount, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for initializing a faucet.
	 * @param resource is Required parameter. Type: String. Resource address.
	 * @param amount is Required parameter. Type: Int. Amount for the faucet.
	 * @param repeatInterval is Required parameter. Type: Int. Repeat interval for the faucet.
	 * @param authority is Required parameter. Type: String. Authority for the initialization.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: tx { transaction blockhash lastValidBlockHeight } faucet
	 */

	Variant createInitializeFaucetTransaction(String resource, int32_t amount, int32_t repeatInterval, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for claiming from a faucet.
	 * @param faucet is Required parameter. Type: String. Faucet address.
	 * @param owner is Required parameter. Type: String. Owner address.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createClaimFaucetTransaction(String faucet, String owner, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for initializing a recipe.
	 * @param project is Required parameter. Type: String. Project address.
	 * @param xp is Required parameter. Type: BigInt. Experience points for the recipe.
	 * @param ingredients is Required parameter. Type: [IngredientsInput!]. List of ingredients for the recipe.
	 * @param meal is Required parameter. Type: MealInput. Meal configuration for the recipe.
	 * @param authority is Required parameter. Type: String. Authority for the initialization.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: recipe transactions { transactions blockhash lastValidBlockHeight }
	 */

	Variant createInitializeRecipeTransaction(String project, int64_t xp, Array ingredients, Variant meal, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for adding ingredients to a recipe.
	 * @param recipe is Required parameter. Type: String. Recipe address.
	 * @param ingredients is Required parameter. Type: [IngredientsInput!]. List of ingredients to add.
	 * @param authority is Required parameter. Type: String. Authority for the addition.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transactions blockhash lastValidBlockHeight
	 */

	Variant createAddIngredientsTransaction(String recipe, Array ingredients, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for removing ingredients from a recipe.
	 * @param recipe is Required parameter. Type: String. Recipe address.
	 * @param ingredients is Required parameter. Type: [String!]. List of ingredient addresses to remove.
	 * @param authority is Required parameter. Type: String. Authority for the removal.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transactions blockhash lastValidBlockHeight
	 */

	Variant createRemoveIngredientsTransaction(String recipe, PackedStringArray ingredients, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create transactions for initializing a cooking process.
	 * @param recipe is Required parameter. Type: String. Recipe address.
	 * @param authority is Required parameter. Type: String. Authority for the initialization.
	 * @param payer Optional parameter. Type: String. Default: "". Payer for the transaction (optional).
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @return Result object containing: blockhash lastValidBlockHeight transactions
	 */

	Variant createInitCookingProcessTransactions(String recipe, String authority, String payer = "", int32_t computeUnitPrice = -1, PackedStringArray lutAddresses = PackedStringArray());
	/**
	 * @brief Create a transaction for initializing badge criteria.
	 * @param args is Required parameter. Type: CreateBadgeCriteriaInput. Arguments for creating badge criteria.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createInitializeBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for claiming badge criteria.
	 * @param args is Required parameter. Type: ClaimBadgeCriteriaInput. Arguments for claiming badge criteria.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createClaimBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);
	/**
	 * @brief Create a transaction for updating badge criteria.
	 * @param args is Required parameter. Type: UpdateBadgeCriteriaInput. Arguments for updating badge criteria.
	 * @param lutAddresses Optional parameter. Type: [String!]. Default: PackedStringArray(). List of lookup table addresses.
	 * @param computeUnitPrice Optional parameter. Type: Int. Default: -1. Price per compute unit.
	 * @return Result object containing: transaction blockhash lastValidBlockHeight
	 */

	Variant createUpdateBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);

#define REGISTER_HONEYCOMB_TYPES                                                                   \
	ClassDB::register_class<honeycomb_resource::CharacterHistory>();                               \
	ClassDB::register_class<honeycomb_resource::AssemblerConfig>();                                \
	ClassDB::register_class<honeycomb_resource::CharacterTrait>();                                 \
	ClassDB::register_class<honeycomb_resource::CharacterModel>();                                 \
	ClassDB::register_class<honeycomb_resource::CharacterConfigWrapped>();                         \
	ClassDB::register_class<honeycomb_resource::CharacterConfigAssembled>();                       \
	ClassDB::register_class<honeycomb_resource::NftCreator>();                                     \
	ClassDB::register_class<honeycomb_resource::MintAs>();                                         \
	ClassDB::register_class<honeycomb_resource::MintAsParamsMplBubblegum>();                       \
	ClassDB::register_class<honeycomb_resource::CharacterCooldown>();                              \
	ClassDB::register_class<honeycomb_resource::Character>();                                      \
	ClassDB::register_class<honeycomb_resource::CharacterSource>();                                \
	ClassDB::register_class<honeycomb_resource::Wrapped>();                                        \
	ClassDB::register_class<honeycomb_resource::AssetCriteria>();                                  \
	ClassDB::register_class<honeycomb_resource::Assembled>();                                      \
	ClassDB::register_class<honeycomb_resource::CharacterUsedBy>();                                \
	ClassDB::register_class<honeycomb_resource::UsedByStaking>();                                  \
	ClassDB::register_class<honeycomb_resource::UsedByMission>();                                  \
	ClassDB::register_class<honeycomb_resource::EarnedRewards>();                                  \
	ClassDB::register_class<honeycomb_resource::UsedByGuild>();                                    \
	ClassDB::register_class<honeycomb_resource::GuildRole>();                                      \
	ClassDB::register_class<honeycomb_resource::UsedByEjected>();                                  \
	ClassDB::register_class<honeycomb_resource::UsedByCustom>();                                   \
	ClassDB::register_class<honeycomb_resource::Global>();                                         \
	ClassDB::register_class<honeycomb_resource::Project>();                                        \
	ClassDB::register_class<honeycomb_resource::DelegateAuthority>();                              \
	ClassDB::register_class<honeycomb_resource::ServiceDelegation>();                              \
	ClassDB::register_class<honeycomb_resource::Service>();                                        \
	ClassDB::register_class<honeycomb_resource::ServiceParamsAssembler>();                         \
	ClassDB::register_class<honeycomb_resource::ServiceParamsAssetManager>();                      \
	ClassDB::register_class<honeycomb_resource::ServiceParamsStaking>();                           \
	ClassDB::register_class<honeycomb_resource::ServiceParamsMissions>();                          \
	ClassDB::register_class<honeycomb_resource::ServiceParamsRaffles>();                           \
	ClassDB::register_class<honeycomb_resource::ServiceParamsGuildKit>();                          \
	ClassDB::register_class<honeycomb_resource::AssociatedProgram>();                              \
	ClassDB::register_class<honeycomb_resource::SerializableActions>();                            \
	ClassDB::register_class<honeycomb_resource::ProfileDataConfig>();                              \
	ClassDB::register_class<honeycomb_resource::User>();                                           \
	ClassDB::register_class<honeycomb_resource::HoneycombUserInfo>();                              \
	ClassDB::register_class<honeycomb_resource::CivicInfo>();                                      \
	ClassDB::register_class<honeycomb_resource::SocialInfo>();                                     \
	ClassDB::register_class<honeycomb_resource::Wallets>();                                        \
	ClassDB::register_class<honeycomb_resource::Profile>();                                        \
	ClassDB::register_class<honeycomb_resource::ProfileInfo>();                                    \
	ClassDB::register_class<honeycomb_resource::PlatformData>();                                   \
	ClassDB::register_class<honeycomb_resource::BadgeCriteria>();                                  \
	ClassDB::register_class<honeycomb_resource::HoneycombAccount>();                               \
	ClassDB::register_class<honeycomb_resource::CompressedAccount>();                              \
	ClassDB::register_class<honeycomb_resource::HoneycombNode>();                                  \
	ClassDB::register_class<honeycomb_resource::Proof>();                                          \
	ClassDB::register_class<honeycomb_resource::ControlledMerkleTrees>();                          \
	ClassDB::register_class<honeycomb_resource::MissionPool>();                                    \
	ClassDB::register_class<honeycomb_resource::Mission>();                                        \
	ClassDB::register_class<honeycomb_resource::MissionCost>();                                    \
	ClassDB::register_class<honeycomb_resource::TimeRequirement>();                                \
	ClassDB::register_class<honeycomb_resource::TimeRequirementParams>();                          \
	ClassDB::register_class<honeycomb_resource::Reward>();                                         \
	ClassDB::register_class<honeycomb_resource::XpRewardType>();                                   \
	ClassDB::register_class<honeycomb_resource::ResourceRewardType>();                             \
	ClassDB::register_class<honeycomb_resource::ResourceRewardTypeParams>();                       \
	ClassDB::register_class<honeycomb_resource::StakingPool>();                                    \
	ClassDB::register_class<honeycomb_resource::LockType>();                                       \
	ClassDB::register_class<honeycomb_resource::Multipliers>();                                    \
	ClassDB::register_class<honeycomb_resource::Multiplier>();                                     \
	ClassDB::register_class<honeycomb_resource::MultiplierType>();                                 \
	ClassDB::register_class<honeycomb_resource::MultiplierTypeParamsStakeDuration>();              \
	ClassDB::register_class<honeycomb_resource::MultiplierTypeParamsNFTCount>();                   \
	ClassDB::register_class<honeycomb_resource::MultiplierTypeParamsCreator>();                    \
	ClassDB::register_class<honeycomb_resource::MultiplierTypeParamsCollection>();                 \
	ClassDB::register_class<honeycomb_resource::Staker>();                                         \
	ClassDB::register_class<honeycomb_resource::SplStakingPool>();                                 \
	ClassDB::register_class<honeycomb_resource::SplMultiplier>();                                  \
	ClassDB::register_class<honeycomb_resource::SplMultiplierType>();                              \
	ClassDB::register_class<honeycomb_resource::SplMultiplierTypeParamsStakeDuration>();           \
	ClassDB::register_class<honeycomb_resource::SplMultiplierTypeParamsStakeAmount>();             \
	ClassDB::register_class<honeycomb_resource::SplRewardConfig>();                                \
	ClassDB::register_class<honeycomb_resource::SplRewardConfigParamsApy>();                       \
	ClassDB::register_class<honeycomb_resource::SplRewardConfigParamsStakeWeight>();               \
	ClassDB::register_class<honeycomb_resource::SplRewardConfigParamsNotSet>();                    \
	ClassDB::register_class<honeycomb_resource::SplStakeWeightConfig>();                           \
	ClassDB::register_class<honeycomb_resource::StakingRecipient>();                               \
	ClassDB::register_class<honeycomb_resource::StakerClaimed>();                                  \
	ClassDB::register_class<honeycomb_resource::StakerClaimedParamsApy>();                         \
	ClassDB::register_class<honeycomb_resource::StakerClaimedParamsStakeWeight>();                 \
	ClassDB::register_class<honeycomb_resource::StakerClaimedParamsNone>();                        \
	ClassDB::register_class<honeycomb_resource::HoneycombTransaction>();                           \
	ClassDB::register_class<honeycomb_resource::CreateCreateProjectTransactionResponse>();         \
	ClassDB::register_class<honeycomb_resource::CreateAssemblerConfigTransactionResponse>();       \
	ClassDB::register_class<honeycomb_resource::CreateCharacterModelTransactionResponse>();        \
	ClassDB::register_class<honeycomb_resource::CreateInitResourceTransactionResponse>();          \
	ClassDB::register_class<honeycomb_resource::CreateCreateNewResourceTreeTransactionResponse>(); \
	ClassDB::register_class<honeycomb_resource::CreateInitializeRecipeTransactionResponse>();      \
	ClassDB::register_class<honeycomb_resource::CreateBeginCookingTransactionResponse>();          \
	ClassDB::register_class<honeycomb_resource::Transactions>();                                   \
	ClassDB::register_class<honeycomb_resource::TransactionResponse>();                            \
	ClassDB::register_class<honeycomb_resource::TransactionBundleResponse>();                      \
	ClassDB::register_class<honeycomb_resource::AuthResponse>();                                   \
	ClassDB::register_class<honeycomb_resource::AuthConfirmed>();                                  \
	ClassDB::register_class<honeycomb_resource::OutputHoldingTree>();                              \
	ClassDB::register_class<honeycomb_resource::CreateCreateHolderAccountTransactionResponse>();   \
	ClassDB::register_class<honeycomb_resource::CreateInitMultipliersTransactionResponse>();       \
	ClassDB::register_class<honeycomb_resource::CreateCreateStakingPoolTransactionResponse>();     \
	ClassDB::register_class<honeycomb_resource::CreateCreateSplStakingPoolTransactionResponse>();  \
	ClassDB::register_class<honeycomb_resource::CreateCreateMissionPoolTransactionResponse>();     \
	ClassDB::register_class<honeycomb_resource::CreateUpdateMissionPoolTransactionResponse>();     \
	ClassDB::register_class<honeycomb_resource::CreateCreateMissionTransactionResponse>();         \
	ClassDB::register_class<honeycomb_resource::CreateUpdateMissionTransactionResponse>();         \
	ClassDB::register_class<honeycomb_resource::CreateInitializeFaucetTransactionResponse>();      \
	ClassDB::register_class<honeycomb_resource::TreeSetupResponse>();                              \
	ClassDB::register_class<honeycomb_resource::HoneycombResource>();                              \
	ClassDB::register_class<honeycomb_resource::ResourceBalance>();                                \
	ClassDB::register_class<honeycomb_resource::ResourceStorage>();                                \
	ClassDB::register_class<honeycomb_resource::ResourceStorageParams>();                          \
	ClassDB::register_class<honeycomb_resource::ResourceKind>();                                   \
	ClassDB::register_class<honeycomb_resource::ResourceKindParamsHplFungible>();                  \
	ClassDB::register_class<honeycomb_resource::ResourceKindParamsWrappedFungible>();              \
	ClassDB::register_class<honeycomb_resource::ResourceCustody>();                                \
	ClassDB::register_class<honeycomb_resource::ResourceCustodyParams>();                          \
	ClassDB::register_class<honeycomb_resource::ResourceKindParamsHplNonFungible>();               \
	ClassDB::register_class<honeycomb_resource::ResourceKindParamsWrappedMplCore>();               \
	ClassDB::register_class<honeycomb_resource::Recipe>();                                         \
	ClassDB::register_class<honeycomb_resource::Ingredient>();                                     \
	ClassDB::register_class<honeycomb_resource::Meal>();                                           \
	ClassDB::register_class<honeycomb_resource::Faucet>();                                         \
	ClassDB::register_class<honeycomb_resource::Holding>();                                        \
	ClassDB::register_class<honeycomb_resource::CharacterTraitInput>();                            \
	ClassDB::register_class<honeycomb_resource::MintAsInput>();                                    \
	ClassDB::register_class<honeycomb_resource::CharacterCooldownInput>();                         \
	ClassDB::register_class<honeycomb_resource::MintAsMplBubblegumInput>();                        \
	ClassDB::register_class<honeycomb_resource::CharacterConfigInput>();                           \
	ClassDB::register_class<honeycomb_resource::AssemblerConfigInput>();                           \
	ClassDB::register_class<honeycomb_resource::NftCreatorInput>();                                \
	ClassDB::register_class<honeycomb_resource::AssetCriteriaInput>();                             \
	ClassDB::register_class<honeycomb_resource::CharactersFilter>();                               \
	ClassDB::register_class<honeycomb_resource::CharacterSourceFilter>();                          \
	ClassDB::register_class<honeycomb_resource::CharacterSourceParamsFilter>();                    \
	ClassDB::register_class<honeycomb_resource::AssetCriteriaFilter>();                            \
	ClassDB::register_class<honeycomb_resource::CharacterUsedByFilter>();                          \
	ClassDB::register_class<honeycomb_resource::CharacterUsedByParamsFilter>();                    \
	ClassDB::register_class<honeycomb_resource::EarnedRewardsFilter>();                            \
	ClassDB::register_class<honeycomb_resource::UsedByGuildFilter>();                              \
	ClassDB::register_class<honeycomb_resource::GuildRoleFilter>();                                \
	ClassDB::register_class<honeycomb_resource::CivicInfoInput>();                                 \
	ClassDB::register_class<honeycomb_resource::UserInfoInput>();                                  \
	ClassDB::register_class<honeycomb_resource::PartialUserInfoInput>();                           \
	ClassDB::register_class<honeycomb_resource::UpdateWalletInput>();                              \
	ClassDB::register_class<honeycomb_resource::AssociatedProgramInput>();                         \
	ClassDB::register_class<honeycomb_resource::SerializableActionsInput>();                       \
	ClassDB::register_class<honeycomb_resource::ProfileDataConfigInput>();                         \
	ClassDB::register_class<honeycomb_resource::ProfileInfoInput>();                               \
	ClassDB::register_class<honeycomb_resource::CustomDataInput>();                                \
	ClassDB::register_class<honeycomb_resource::PlatformDataInput>();                              \
	ClassDB::register_class<honeycomb_resource::ServiceDelegationInput>();                         \
	ClassDB::register_class<honeycomb_resource::ServiceDelegationHiveControl>();                   \
	ClassDB::register_class<honeycomb_resource::ServiceDelegationCharacterManager>();              \
	ClassDB::register_class<honeycomb_resource::ServiceDelegationResourceManager>();               \
	ClassDB::register_class<honeycomb_resource::ServiceDelegationNectarStaking>();                 \
	ClassDB::register_class<honeycomb_resource::ServiceDelegationNectarMissions>();                \
	ClassDB::register_class<honeycomb_resource::ServiceDelegationBuzzGuild>();                     \
	ClassDB::register_class<honeycomb_resource::ModifyServiceDelegationInput>();                   \
	ClassDB::register_class<honeycomb_resource::ModifyDelegationInput>();                          \
	ClassDB::register_class<honeycomb_resource::UpdateMissionInput>();                             \
	ClassDB::register_class<honeycomb_resource::CreateStakingPoolMetadataInput>();                 \
	ClassDB::register_class<honeycomb_resource::UpdateStakingPoolMetadataInput>();                 \
	ClassDB::register_class<honeycomb_resource::MultiplierTypeInput>();                            \
	ClassDB::register_class<honeycomb_resource::AddMultiplierMetadataInput>();                     \
	ClassDB::register_class<honeycomb_resource::InitStakingMultiplierMetadataInput>();             \
	ClassDB::register_class<honeycomb_resource::CreateSplStakingPoolMetadataInput>();              \
	ClassDB::register_class<honeycomb_resource::UpdateSplStakingPoolMetadataInput>();              \
	ClassDB::register_class<honeycomb_resource::AddRemoveRewardsFromRewardPoolActionInput>();      \
	ClassDB::register_class<honeycomb_resource::CreateSplRewardPoolArgsInput>();                   \
	ClassDB::register_class<honeycomb_resource::RewardPoolConfigApyInput>();                       \
	ClassDB::register_class<honeycomb_resource::SplMultiplierInput>();                             \
	ClassDB::register_class<honeycomb_resource::AddSplMultiplierMetadataInput>();                  \
	ClassDB::register_class<honeycomb_resource::DisrcriminatorFilter>();                           \
	ClassDB::register_class<honeycomb_resource::TreeLeaf>();                                       \
	ClassDB::register_class<honeycomb_resource::TreeSetupConfig>();                                \
	ClassDB::register_class<honeycomb_resource::BasicTreeConfig>();                                \
	ClassDB::register_class<honeycomb_resource::AdvancedTreeConfig>();                             \
	ClassDB::register_class<honeycomb_resource::RecipeInputResource>();                            \
	ClassDB::register_class<honeycomb_resource::RecipeInputResources>();                           \
	ClassDB::register_class<honeycomb_resource::RecipeOutputResource>();                           \
	ClassDB::register_class<honeycomb_resource::RecipeResources>();                                \
	ClassDB::register_class<honeycomb_resource::NewMissionPoolData>();                             \
	ClassDB::register_class<honeycomb_resource::UpdateMissionPoolData>();                          \
	ClassDB::register_class<honeycomb_resource::NewMissionCost>();                                 \
	ClassDB::register_class<honeycomb_resource::MissionReward>();                                  \
	ClassDB::register_class<honeycomb_resource::NewMissionData>();                                 \
	ClassDB::register_class<honeycomb_resource::UpdateMissionData>();                              \
	ClassDB::register_class<honeycomb_resource::ParticipateOnMissionData>();                       \
	ClassDB::register_class<honeycomb_resource::RecallFromMissionData>();                          \
	ClassDB::register_class<honeycomb_resource::CreateBadgeCriteriaInput>();                       \
	ClassDB::register_class<honeycomb_resource::ClaimBadgeCriteriaInput>();                        \
	ClassDB::register_class<honeycomb_resource::UpdateBadgeCriteriaInput>();                       \
	ClassDB::register_class<honeycomb_resource::SendTransactionsOptions>();                        \
	ClassDB::register_class<honeycomb_resource::SendTransactionBundlesOptions>();                  \
	ClassDB::register_class<honeycomb_resource::TransactionBundlesOptions>();                      \
	ClassDB::register_class<honeycomb_resource::InitResourceInput>();                              \
	ClassDB::register_class<honeycomb_resource::IngredientsInput>();                               \
	ClassDB::register_class<honeycomb_resource::MealInput>();                                      \
	ClassDB::register_class<honeycomb_resource::ImportResourceInput>();                            \
	ClassDB::register_class<honeycomb_resource::ImportResourceInputCustodyInput>();
};

} // namespace godot

#endif