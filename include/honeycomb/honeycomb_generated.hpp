#define HONEYCOMB_METHOD_DEFS	Variant findCompressedAccounts(Array addresses, Variant identity = Variant(nullptr), Array leaves = Array(), Dictionary parsedData = Dictionary());\
	Variant fetchProofs(Array leaves);\
	Variant findAccounts(PackedByteArray programId, PackedByteArray discriminator = PackedByteArray(), Dictionary parsedData = Dictionary());\
	Variant sendTransactionBundles(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options = Variant(nullptr));\
	Variant signWithShadowSignerAndSendTransactionBundles(Array txs, String blockhash, int32_t lastValidBlockHeight, String authToken, Variant options = Variant(nullptr));\
	Variant sendBulkTransactions(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options = Variant(nullptr));\
	Variant signWithShadowSignerAndSendBulkTransactions(Array txs, String blockhash, int32_t lastValidBlockHeight, Variant options = Variant(nullptr));\
	Variant authRequest(String wallet, bool useTx = false, String useRpc = "");\
	Variant authConfirm(String wallet, String signature);\
	Variant createTransferAssetsTransactions(PackedStringArray addresses, String to, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createBurnAssetsTransactions(PackedStringArray addresses, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant findGlobal(String env = "");\
	Variant findProjects(Array addresses = Array(), PackedStringArray names = PackedStringArray(), Array authorities = Array());\
	Variant findDelegateAuthority(Array addresses = Array(), Array delegates = Array(), Array projects = Array());\
	Variant findUsers(Array addresses = Array(), PackedInt32Array ids = PackedInt32Array(), Array wallets = Array(), bool includeProof = false, Array includeProjectProfiles = Array());\
	Variant findProfiles(Array addresses = Array(), Array projects = Array(), PackedInt32Array userIds = PackedInt32Array(), PackedStringArray identities = PackedStringArray(), bool includeProof = false, bool includeUsers = false);\
	Variant createNewUserTransaction(String wallet, Variant info = Variant(nullptr), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createNewUserBulkTransaction(PackedStringArray wallet, Array info, String payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateUserTransaction(String payer, Variant info = Variant(nullptr), Variant wallets = Variant(nullptr), bool populateCivic = false, Array removeSocials = Array(), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateProjectTransaction(String authority, String name, String driver = "", Array associatedPrograms = Array(), Variant profileDataConfig = Variant(nullptr), bool subsidizeFees = false, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createChangeProjectDriverTransaction(String project, String driver, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateDelegateAuthorityTransaction(String project, String delegate, Variant serviceDelegations, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createModifyDelegationTransaction(String project, String delegate, Variant modifyDelegation, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateProfilesTreeTransaction(Variant treeConfig, String project, String payer, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createNewProfileTransaction(String project, String payer, String identity = "", Variant info = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateProfileTransaction(String profile, String payer, Variant info = Variant(nullptr), Variant customData = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdatePlatformDataTransaction(String profile, String authority, String delegateAuthority = "", String payer = "", Variant platformData = Variant(nullptr), PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createNewUserWithProfileTransaction(String project, String wallet, Variant userInfo, String payer = "", String profileIdentity = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant findAssemblerConfig(Array addresses = Array(), PackedStringArray tickers = PackedStringArray(), Array characterModel = Array(), Array project = Array());\
	Variant findCharacterTraits(Array addresses = Array(), Array trees = Array(), bool includeProof = false);\
	Variant findCharacterModels(Array addresses = Array(), Variant project = Variant(nullptr), Array assemblerConfigs = Array());\
	Variant findCharacters(bool includeProof = false, Array addresses = Array(), Variant filters = Variant(nullptr), Array trees = Array(), Array wallets = Array(), Array mints = Array(), Array attributeHashes = Array());\
	Variant fetchCharacterHistory(Array addresses, PackedStringArray event = PackedStringArray());\
	Variant createCreateAssemblerConfigTransaction(String ticker, String project, String authority, Variant treeConfig = Variant(nullptr), PackedStringArray order = PackedStringArray(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createAddCharacterTraitsTransactions(String assemblerConfig, Array traits, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createRemoveCharacterTraitsTransactions(String assemblerConfig, PackedStringArray traitsAddresses, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateCharacterModelTransaction(Variant config, String project, String authority, Array attributes = Array(), Variant mintAs = Variant(nullptr), Variant cooldown = Variant(nullptr), PackedStringArray equipableCriteria = PackedStringArray(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateCharactersTreeTransaction(Variant treeConfig, String project, String characterModel, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createAssembleCharacterTransaction(String project, String assemblerConfig, String characterModel, String owner, String authority, String uri = "", Array attributes = Array(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateCharacterTraitsTransaction(String characterAddress, String project, String assemblerConfig, String characterModel, String authority, String uri = "", Array attributes = Array(), String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createPopulateCharacterTransaction(String project, String characterModel, String mint, String owner, String updateAuthority, String payer, Array attributes = Array(), String assemblerConfig = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createWrapAssetsToCharacterTransactions(String project, String characterModel, String wallet, PackedStringArray mintList, String libreplexDeployment = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUnwrapAssetsFromCharacterTransactions(String project, PackedStringArray characterAddresses, String characterModel, String wallet, String libreplexDeployment = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createEquipResourceOnCharacterTransaction(String characterModel, String characterAddress, String resource, int64_t amount, String owner, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createDismountResourceOnCharacterTransaction(String characterModel, String characterAddress, String resource, int64_t amount, String owner, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUseCharacterTransaction(String character, String project, String characterModel, String user, Dictionary data = Dictionary(), bool unUse = false, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant findStakingPools(Array addresses = Array(), Array projects = Array(), PackedStringArray names = PackedStringArray());\
	Variant findStakers(Array addresses = Array(), Array stakingPools = Array(), Array wallets = Array());\
	Variant findMultipliers(Array addresses = Array(), Array stakingPools = Array());\
	Variant createCreateStakingPoolTransaction(String project, String resource, Variant metadata, String authority, Variant multiplier = Variant(nullptr), String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateStakingPoolTransaction(String project, String stakingPool, String authority, Variant metadata = Variant(nullptr), String characterModel = "", String resource = "", String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createInitMultipliersTransaction(String project, String stakingPool, int32_t decimals, Array multipliers, String authority, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createAddMultiplierTransaction(String project, String multiplier, Variant metadata, String authority, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createStakeCharactersTransactions(PackedStringArray characterAddresses, String characterModel, String stakingPool, String project, String feePayer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createClaimStakingRewardsTransactions(PackedStringArray characterAddresses, String characterModel, String feePayer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUnstakeCharactersTransactions(PackedStringArray characterAddresses, String characterModel, String feePayer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant findMissions(Array addresses = Array(), Array missionPools = Array());\
	Variant findMissionPools(Array addresses = Array(), Variant project = Variant(nullptr));\
	Variant createCreateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateMissionPoolTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateMissionTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateMissionTransaction(String missionAddress, String authority, Variant params, String payer = "", String delegateAuthority = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createSendCharactersOnMissionTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createRecallCharactersTransaction(Variant data, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant findResources(Array addresses = Array(), Array projects = Array(), Array mints = Array());\
	Variant findResourcesBalance(PackedStringArray wallets, PackedStringArray addresses = PackedStringArray(), PackedStringArray projects = PackedStringArray(), PackedStringArray mints = PackedStringArray());\
	Variant findFaucets(Array addresses = Array(), Array projects = Array(), Array resources = Array());\
	Variant findRecipes(Array addresses = Array(), Array projects = Array());\
	Variant findHoldings(Array addresses = Array(), Array holders = Array(), Array trees = Array(), bool includeProof = false);\
	Variant createCreateNewResourceTransaction(String project, String authority, Variant params, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createImportFungibleResourceTransaction(Variant params, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createExportFungibleResourceTransaction(String resource, String authority, String delegateAuthority = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateNewResourceTreeTransaction(String project, String resource, String authority, Variant treeConfig, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createMintResourceTransaction(String resource, String owner, String authority, int64_t amount, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createBurnResourceTransaction(String resource, int64_t amount, String authority, String owner = "", String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createTransferResourceTransaction(String resource, String owner, String recipient, int64_t amount, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateWrapHoldingTransaction(String resource, int64_t amount, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createCreateUnwrapHoldingTransaction(String resource, int64_t amount, String authority, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createInitializeFaucetTransaction(String resource, int32_t amount, int32_t repeatInterval, String authority, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createClaimFaucetTransaction(String faucet, String owner, String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createInitializeRecipeTransaction(String project, int64_t xp, Array ingredients, Variant meal, String authority, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createAddIngredientsTransaction(String recipe, Array ingredients, String authority, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createRemoveIngredientsTransaction(String recipe, PackedStringArray ingredients, String authority, String delegateAuthority = "", String payer = "", PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createInitCookingProcessTransactions(String recipe, String authority, String payer = "", int32_t computeUnitPrice = -1, PackedStringArray lutAddresses = PackedStringArray());\
	Variant createInitializeBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createClaimBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);\
	Variant createUpdateBadgeCriteriaTransaction(Variant args, PackedStringArray lutAddresses = PackedStringArray(), int32_t computeUnitPrice = -1);

#define REGISTER_HONEYCOMB_TYPES ClassDB::register_class<honeycomb_resource::CharacterHistory>(); \
ClassDB::register_class<honeycomb_resource::AssemblerConfig>(); \
ClassDB::register_class<honeycomb_resource::CharacterTrait>(); \
ClassDB::register_class<honeycomb_resource::CharacterModel>(); \
ClassDB::register_class<honeycomb_resource::CharacterConfigWrapped>(); \
ClassDB::register_class<honeycomb_resource::CharacterConfigAssembled>(); \
ClassDB::register_class<honeycomb_resource::NftCreator>(); \
ClassDB::register_class<honeycomb_resource::MintAs>(); \
ClassDB::register_class<honeycomb_resource::MintAsParamsMplBubblegum>(); \
ClassDB::register_class<honeycomb_resource::CharacterCooldown>(); \
ClassDB::register_class<honeycomb_resource::Character>(); \
ClassDB::register_class<honeycomb_resource::CharacterSource>(); \
ClassDB::register_class<honeycomb_resource::Wrapped>(); \
ClassDB::register_class<honeycomb_resource::AssetCriteria>(); \
ClassDB::register_class<honeycomb_resource::Assembled>(); \
ClassDB::register_class<honeycomb_resource::CharacterUsedBy>(); \
ClassDB::register_class<honeycomb_resource::UsedByStaking>(); \
ClassDB::register_class<honeycomb_resource::UsedByMission>(); \
ClassDB::register_class<honeycomb_resource::EarnedRewards>(); \
ClassDB::register_class<honeycomb_resource::UsedByGuild>(); \
ClassDB::register_class<honeycomb_resource::GuildRole>(); \
ClassDB::register_class<honeycomb_resource::UsedByEjected>(); \
ClassDB::register_class<honeycomb_resource::UsedByCustom>(); \
ClassDB::register_class<honeycomb_resource::Global>(); \
ClassDB::register_class<honeycomb_resource::Project>(); \
ClassDB::register_class<honeycomb_resource::DelegateAuthority>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegation>(); \
ClassDB::register_class<honeycomb_resource::Service>(); \
ClassDB::register_class<honeycomb_resource::ServiceParamsAssembler>(); \
ClassDB::register_class<honeycomb_resource::ServiceParamsAssetManager>(); \
ClassDB::register_class<honeycomb_resource::ServiceParamsStaking>(); \
ClassDB::register_class<honeycomb_resource::ServiceParamsMissions>(); \
ClassDB::register_class<honeycomb_resource::ServiceParamsRaffles>(); \
ClassDB::register_class<honeycomb_resource::ServiceParamsGuildKit>(); \
ClassDB::register_class<honeycomb_resource::AssociatedProgram>(); \
ClassDB::register_class<honeycomb_resource::SerializableActions>(); \
ClassDB::register_class<honeycomb_resource::ProfileDataConfig>(); \
ClassDB::register_class<honeycomb_resource::User>(); \
ClassDB::register_class<honeycomb_resource::HoneycombUserInfo>(); \
ClassDB::register_class<honeycomb_resource::CivicInfo>(); \
ClassDB::register_class<honeycomb_resource::SocialInfo>(); \
ClassDB::register_class<honeycomb_resource::Wallets>(); \
ClassDB::register_class<honeycomb_resource::Profile>(); \
ClassDB::register_class<honeycomb_resource::ProfileInfo>(); \
ClassDB::register_class<honeycomb_resource::PlatformData>(); \
ClassDB::register_class<honeycomb_resource::BadgeCriteria>(); \
ClassDB::register_class<honeycomb_resource::HoneycombAccount>(); \
ClassDB::register_class<honeycomb_resource::CompressedAccount>(); \
ClassDB::register_class<honeycomb_resource::HoneycombNode>(); \
ClassDB::register_class<honeycomb_resource::Proof>(); \
ClassDB::register_class<honeycomb_resource::ControlledMerkleTrees>(); \
ClassDB::register_class<honeycomb_resource::MissionPool>(); \
ClassDB::register_class<honeycomb_resource::Mission>(); \
ClassDB::register_class<honeycomb_resource::MissionCost>(); \
ClassDB::register_class<honeycomb_resource::TimeRequirement>(); \
ClassDB::register_class<honeycomb_resource::TimeRequirementParams>(); \
ClassDB::register_class<honeycomb_resource::Reward>(); \
ClassDB::register_class<honeycomb_resource::XpRewardType>(); \
ClassDB::register_class<honeycomb_resource::ResourceRewardType>(); \
ClassDB::register_class<honeycomb_resource::ResourceRewardTypeParams>(); \
ClassDB::register_class<honeycomb_resource::StakingPool>(); \
ClassDB::register_class<honeycomb_resource::LockType>(); \
ClassDB::register_class<honeycomb_resource::Multipliers>(); \
ClassDB::register_class<honeycomb_resource::Multiplier>(); \
ClassDB::register_class<honeycomb_resource::MultiplierType>(); \
ClassDB::register_class<honeycomb_resource::MultiplierTypeParamsStakeDuration>(); \
ClassDB::register_class<honeycomb_resource::MultiplierTypeParamsNFTCount>(); \
ClassDB::register_class<honeycomb_resource::MultiplierTypeParamsCreator>(); \
ClassDB::register_class<honeycomb_resource::MultiplierTypeParamsCollection>(); \
ClassDB::register_class<honeycomb_resource::Staker>(); \
ClassDB::register_class<honeycomb_resource::HoneycombTransaction>(); \
ClassDB::register_class<honeycomb_resource::CreateCreateProjectTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateAssemblerConfigTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateCharacterModelTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateInitResourceTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateCreateNewResourceTreeTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateInitializeRecipeTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateBeginCookingTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::Transactions>(); \
ClassDB::register_class<honeycomb_resource::TransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::TransactionBundleResponse>(); \
ClassDB::register_class<honeycomb_resource::AuthResponse>(); \
ClassDB::register_class<honeycomb_resource::AuthConfirmed>(); \
ClassDB::register_class<honeycomb_resource::OutputHoldingTree>(); \
ClassDB::register_class<honeycomb_resource::CreateCreateHolderAccountTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateInitMultipliersTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateCreateStakingPoolTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateCreateMissionPoolTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateUpdateMissionPoolTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateCreateMissionTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateUpdateMissionTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::CreateInitializeFaucetTransactionResponse>(); \
ClassDB::register_class<honeycomb_resource::TreeSetupResponse>(); \
ClassDB::register_class<honeycomb_resource::HoneycombResource>(); \
ClassDB::register_class<honeycomb_resource::ResourceBalance>(); \
ClassDB::register_class<honeycomb_resource::ResourceStorage>(); \
ClassDB::register_class<honeycomb_resource::ResourceStorageParams>(); \
ClassDB::register_class<honeycomb_resource::ResourceKind>(); \
ClassDB::register_class<honeycomb_resource::ResourceKindParamsHplFungible>(); \
ClassDB::register_class<honeycomb_resource::ResourceKindParamsWrappedFungible>(); \
ClassDB::register_class<honeycomb_resource::ResourceCustody>(); \
ClassDB::register_class<honeycomb_resource::ResourceCustodyParams>(); \
ClassDB::register_class<honeycomb_resource::ResourceKindParamsHplNonFungible>(); \
ClassDB::register_class<honeycomb_resource::ResourceKindParamsWrappedMplCore>(); \
ClassDB::register_class<honeycomb_resource::Recipe>(); \
ClassDB::register_class<honeycomb_resource::Ingredient>(); \
ClassDB::register_class<honeycomb_resource::Meal>(); \
ClassDB::register_class<honeycomb_resource::Faucet>(); \
ClassDB::register_class<honeycomb_resource::Holding>(); \
ClassDB::register_class<honeycomb_resource::CharacterTraitInput>(); \
ClassDB::register_class<honeycomb_resource::MintAsInput>(); \
ClassDB::register_class<honeycomb_resource::CharacterCooldownInput>(); \
ClassDB::register_class<honeycomb_resource::MintAsMplBubblegumInput>(); \
ClassDB::register_class<honeycomb_resource::CharacterConfigInput>(); \
ClassDB::register_class<honeycomb_resource::AssemblerConfigInput>(); \
ClassDB::register_class<honeycomb_resource::NftCreatorInput>(); \
ClassDB::register_class<honeycomb_resource::AssetCriteriaInput>(); \
ClassDB::register_class<honeycomb_resource::CharactersFilter>(); \
ClassDB::register_class<honeycomb_resource::CharacterSourceFilter>(); \
ClassDB::register_class<honeycomb_resource::CharacterSourceParamsFilter>(); \
ClassDB::register_class<honeycomb_resource::AssetCriteriaFilter>(); \
ClassDB::register_class<honeycomb_resource::CharacterUsedByFilter>(); \
ClassDB::register_class<honeycomb_resource::CharacterUsedByParamsFilter>(); \
ClassDB::register_class<honeycomb_resource::EarnedRewardsFilter>(); \
ClassDB::register_class<honeycomb_resource::UsedByGuildFilter>(); \
ClassDB::register_class<honeycomb_resource::GuildRoleFilter>(); \
ClassDB::register_class<honeycomb_resource::CivicInfoInput>(); \
ClassDB::register_class<honeycomb_resource::UserInfoInput>(); \
ClassDB::register_class<honeycomb_resource::PartialUserInfoInput>(); \
ClassDB::register_class<honeycomb_resource::UpdateWalletInput>(); \
ClassDB::register_class<honeycomb_resource::AssociatedProgramInput>(); \
ClassDB::register_class<honeycomb_resource::SerializableActionsInput>(); \
ClassDB::register_class<honeycomb_resource::ProfileDataConfigInput>(); \
ClassDB::register_class<honeycomb_resource::ProfileInfoInput>(); \
ClassDB::register_class<honeycomb_resource::CustomDataInput>(); \
ClassDB::register_class<honeycomb_resource::PlatformDataInput>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationInput>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationHiveControl>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationCharacterManager>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationResourceManager>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationNectarStaking>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationNectarMissions>(); \
ClassDB::register_class<honeycomb_resource::ServiceDelegationBuzzGuild>(); \
ClassDB::register_class<honeycomb_resource::ModifyServiceDelegationInput>(); \
ClassDB::register_class<honeycomb_resource::ModifyDelegationInput>(); \
ClassDB::register_class<honeycomb_resource::UpdateMissionInput>(); \
ClassDB::register_class<honeycomb_resource::CreateStakingPoolMetadataInput>(); \
ClassDB::register_class<honeycomb_resource::UpdateStakingPoolMetadataInput>(); \
ClassDB::register_class<honeycomb_resource::MultiplierTypeInput>(); \
ClassDB::register_class<honeycomb_resource::AddMultiplierMetadataInput>(); \
ClassDB::register_class<honeycomb_resource::InitStakingMultiplierMetadataInput>(); \
ClassDB::register_class<honeycomb_resource::DisrcriminatorFilter>(); \
ClassDB::register_class<honeycomb_resource::TreeLeaf>(); \
ClassDB::register_class<honeycomb_resource::TreeSetupConfig>(); \
ClassDB::register_class<honeycomb_resource::BasicTreeConfig>(); \
ClassDB::register_class<honeycomb_resource::AdvancedTreeConfig>(); \
ClassDB::register_class<honeycomb_resource::RecipeInputResource>(); \
ClassDB::register_class<honeycomb_resource::RecipeInputResources>(); \
ClassDB::register_class<honeycomb_resource::RecipeOutputResource>(); \
ClassDB::register_class<honeycomb_resource::RecipeResources>(); \
ClassDB::register_class<honeycomb_resource::NewMissionPoolData>(); \
ClassDB::register_class<honeycomb_resource::UpdateMissionPoolData>(); \
ClassDB::register_class<honeycomb_resource::NewMissionCost>(); \
ClassDB::register_class<honeycomb_resource::MissionReward>(); \
ClassDB::register_class<honeycomb_resource::NewMissionData>(); \
ClassDB::register_class<honeycomb_resource::UpdateMissionData>(); \
ClassDB::register_class<honeycomb_resource::ParticipateOnMissionData>(); \
ClassDB::register_class<honeycomb_resource::RecallFromMissionData>(); \
ClassDB::register_class<honeycomb_resource::CreateBadgeCriteriaInput>(); \
ClassDB::register_class<honeycomb_resource::ClaimBadgeCriteriaInput>(); \
ClassDB::register_class<honeycomb_resource::UpdateBadgeCriteriaInput>(); \
ClassDB::register_class<honeycomb_resource::SendTransactionsOptions>(); \
ClassDB::register_class<honeycomb_resource::SendTransactionBundlesOptions>(); \
ClassDB::register_class<honeycomb_resource::TransactionBundlesOptions>(); \
ClassDB::register_class<honeycomb_resource::InitResourceInput>(); \
ClassDB::register_class<honeycomb_resource::IngredientsInput>(); \
ClassDB::register_class<honeycomb_resource::MealInput>(); \
ClassDB::register_class<honeycomb_resource::ImportResourceInput>(); \
ClassDB::register_class<honeycomb_resource::ImportResourceInputCustodyInput>(); 

