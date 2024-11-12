#define HONEYCOMB_METHOD_DEFS 	Variant createCreateNewResourceTransaction(const Variant& project, const Variant& authority, Variant params, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateNewResourceTreeTransaction(const Variant& project, const Variant& resource, const Variant& authority, Variant treeConfig, const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createMintResourceTransaction(const Variant& resource, const Variant& owner, const Variant& authority, int64_t amount, const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createBurnResourceTransaction(const Variant& resource, int64_t amount, const Variant& authority, const Variant& owner = Variant(nullptr), const Variant& payer = Variant(nullptr), const Variant& delegateAuthority = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateUnwrapHoldingTransaction(String resource, int64_t amount, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateWrapHoldingTransaction(String resource, int64_t amount, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createTransferResourceTransaction(String resource, const Variant& owner, String recipient, int64_t amount, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createInitializeRecipeTransaction(const Variant& project, int64_t xp, Array ingredients, Variant meal, const Variant& authority, const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createAddIngredientsTransaction(const Variant& recipe, Array ingredients, const Variant& authority, const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createRemoveIngredientsTransaction(const Variant& recipe, PackedStringArray ingredients, const Variant& authority, const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createInitCookingProcessTransactions(const Variant& recipe, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateMissionTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateMissionTransaction(const Variant& missionAddress, const Variant& authority, Variant params, const Variant& payer = Variant(nullptr), const Variant& delegateAuthority = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createSendCharactersOnMissionTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateStakingPoolTransaction(const Variant& project, const Variant& resource, Variant metadata, const Variant& authority, Variant multiplier = Variant(nullptr), const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateStakingPoolTransaction(const Variant& project, const Variant& stakingPool, const Variant& authority, Variant metadata = Variant(nullptr), const Variant& characterModel = Variant(nullptr), const Variant& resource = Variant(nullptr), const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createInitMultipliersTransaction(const Variant& project, const Variant& stakingPool, int32_t decimals, Array multipliers, const Variant& authority, const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createAddMultiplierTransaction(const Variant& project, const Variant& multiplier, Variant metadata, const Variant& authority, const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createStakeCharactersTransactions(PackedStringArray characterAddresses, const Variant& project, const Variant& characterModel, const Variant& stakingPool, const Variant& feePayer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createClaimStakingRewardsTransactions(PackedStringArray characterAddresses, const Variant& characterModel, const Variant& feePayer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUnstakeCharactersTransactions(PackedStringArray characterAddresses, const Variant& characterModel, const Variant& feePayer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createAddCharacterTraitsTransactions(const Variant& assemblerConfig, Array traits, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createRemoveCharacterTraitsTransactions(const Variant& assemblerConfig, PackedStringArray traitsAddresses, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateCharacterModelTransaction(Variant config, const Variant& project, const Variant& authority, Array attributes = Array(), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateCharactersTreeTransaction(Variant treeConfig, const Variant& project, const Variant& characterModel, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createAssembleCharacterTransaction(Array attributes, const Variant& project, const Variant& assemblerConfig, const Variant& characterModel, const Variant& charactersTree, const Variant& wallet, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateCharacterTraitsTransaction(const Variant& characterAddress, Array attributes, const Variant& project, const Variant& assemblerConfig, const Variant& characterModel, const Variant& charactersTree, const Variant& wallet, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createPopulateAssembleablCharacterTransaction(Array attributes, const Variant& project, const Variant& characterModel, const Variant& charactersTree, const Variant& mint, const Variant& owner, const Variant& updateAuthority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createWrapAssetsToCharacterTransactions(PackedStringArray mintList, const Variant& project, const Variant& characterModel, const Variant& wallet, String libreplexDeployment = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUnwrapAssetsFromCharacterTransactions(PackedStringArray characterAddresses, const Variant& project, String characterModel, const Variant& wallet, String libreplexDeployment = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createRecallCharactersTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant signWithShadowSignerAndSendTransactionBundles(Array txs, String blockhash, int32_t lastValidBlockHeight, String authToken, Variant options = Variant(nullptr));\
	Variant createInitializeFaucetTransaction(const Variant& resource, int32_t amount, int32_t repeatInterval, const Variant& authority, const Variant& delegateAuthority = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createClaimFaucetTransaction(const Variant& faucet, const Variant& owner, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createNewUserTransaction(const Variant& wallet, Variant info = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createNewUserBulkTransaction(Array info, PackedStringArray wallet, const Variant& payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateUserTransaction(const Variant& payer, Variant info = Variant(nullptr), Variant wallets = Variant(nullptr), bool populateCivic = false, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateProfilesTreeTransaction(Variant treeConfig, const Variant& project, const Variant& payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createNewProfileTransaction(const Variant& project, const Variant& payer, String identity = "", Variant info = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateProfileTransaction(const Variant& profile, const Variant& payer, Variant info = Variant(nullptr), Variant customData = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createNewUserWithProfileTransaction(const Variant& project, const Variant& wallet, Variant userInfo = Variant(nullptr), String profileIdentity = "", Variant profileInfo = Variant(nullptr), const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createClaimBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateProjectTransaction(const Variant& authority, String name, const Variant& driver = Variant(nullptr), Array associatedPrograms = Array(), Variant profileDataConfig = Variant(nullptr), bool subsidizeFees = false, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createChangeProjectDriverTransaction(const Variant& project, const Variant& driver, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateDelegateAuthorityTransaction(const Variant& project, const Variant& delegate, Variant serviceDelegations, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createModifyDelegationTransaction(const Variant& project, const Variant& delegate, Variant modifyDelegation, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateAssemblerConfigTransaction(Variant treeConfig, String ticker, PackedStringArray order, const Variant& project, const Variant& authority, const Variant& payer = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createInitializeBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);

#define REGISTER_HONEYCOMB_TYPES ClassDB::register_class<honeycomb_resource::HoneycombTransaction>(); \
ClassDB::register_class<honeycomb_resource::SendTransactionBundlesOptions>(); \
ClassDB::register_class<honeycomb_resource::CreateBadgeCriteriaInput>(); \
ClassDB::register_class<honeycomb_resource::ModifyDelegationInput>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationInput>(); \
ClassDB::register_class<honeycomb_resource::ProfileDataConfigInput>(); \
ClassDB::register_class<honeycomb_resource::UpdateBadgeCriteriaInput>(); \
ClassDB::register_class<honeycomb_resource::ClaimBadgeCriteriaInput>(); \
ClassDB::register_class<honeycomb_resource::ProfileInfoInput>(); \
ClassDB::register_class<honeycomb_resource::CustomDataInput>(); \
ClassDB::register_class<honeycomb_resource::UpdateWalletInput>(); \
ClassDB::register_class<honeycomb_resource::PartialUserInfoInput>(); \
ClassDB::register_class<honeycomb_resource::TreeSetupConfig>(); \
ClassDB::register_class<honeycomb_resource::UserInfoInput>(); \
ClassDB::register_class<honeycomb_resource::RecallFromMissionData>(); \
ClassDB::register_class<honeycomb_resource::CharacterConfigInput>(); \
ClassDB::register_class<honeycomb_resource::AddMultiplierMetadataInput>(); \
ClassDB::register_class<honeycomb_resource::UpdateStakingPoolMetadataInput>(); \
ClassDB::register_class<honeycomb_resource::InitStakingMultiplierMetadataInput>(); \
ClassDB::register_class<honeycomb_resource::CreateStakingPoolMetadataInput>(); \
ClassDB::register_class<honeycomb_resource::ParticipateOnMissionData>(); \
ClassDB::register_class<honeycomb_resource::UpdateMissionInput>(); \
ClassDB::register_class<honeycomb_resource::NewMissionData>(); \
ClassDB::register_class<honeycomb_resource::UpdateMissionPoolData>(); \
ClassDB::register_class<honeycomb_resource::NewMissionPoolData>(); \
ClassDB::register_class<honeycomb_resource::MealInput>(); \
ClassDB::register_class<honeycomb_resource::InitResourceInput>(); \
ClassDB::register_class<honeycomb_resource::MultiplierTypeInput>(); \
ClassDB::register_class<honeycomb_resource::AssemblerConfigInput>(); \
ClassDB::register_class<honeycomb_resource::ModifyServiceDelegationInput>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationHiveControl>(); \
ClassDB::register_class<honeycomb_resource::TransactionBundlesOptions>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationAssetAssembler>(); \
ClassDB::register_class<honeycomb_resource::NewMissionCost>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationAssetManager>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationCurrencyManager>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationNectarStaking>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationNectarMissions>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationBuzzGuild>(); \
ClassDB::register_class<honeycomb_resource::BasicTreeConfig>(); \
ClassDB::register_class<honeycomb_resource::AdvancedTreeConfig>(); \
ClassDB::register_class<honeycomb_resource::AssociatedProgramInput>(); 

