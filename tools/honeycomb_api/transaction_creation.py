CREATE_NEW_RESOURCE = """
query CreateCreateNewResourceTransaction(
  $project: String!,
  $authority: String!,
  $params: InitResourceInput!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateNewResourceTransaction(
    project: $project,
    authority: $authority,
    params: $params,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    resource
  }
}
"""

CREATE_NEW_RESOURCE_TREE = """
query CreateCreateNewResourceTreeTransaction(
  $project: String!,
  $resource: String!,
  $authority: String!,
  $treeConfig: TreeSetupConfig!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateNewResourceTreeTransaction(
    project: $project,
    resource: $resource,
    authority: $authority,
    treeConfig: $treeConfig,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    treeAddress
    proofBytes
    space
    cost
    maxTreeCapacity
  }
}
"""

MINT_RESOURCE_TRANSACTION = """
query CreateMintResourceTransaction(
  $resource: String!,
  $owner: String!,
  $authority: String!,
  $amount: BigInt!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createMintResourceTransaction(
    resource: $resource,
    owner: $owner,
    authority: $authority,
    amount: $amount,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CREATE_BURN_RESOURCE_TRANSACTION = """
query CreateBurnResourceTransaction(
  $resource: String!,
  $amount: BigInt!,
  $authority: String!,
  $owner: String,
  $payer: String,
  $delegateAuthority: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createBurnResourceTransaction(
    resource: $resource,
    amount: $amount,
    authority: $authority,
    owner: $owner,
    payer: $payer,
    delegateAuthority: $delegateAuthority,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CREATE_UNWRAP_HOLDING_TRANSACTION = """
query CreateCreateUnwrapHoldingTransaction(
  $resource: String!,
  $amount: BigInt!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateUnwrapHoldingTransaction(
    resource: $resource,
    amount: $amount,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateCreateWrapHoldingTransaction = """
query CreateCreateWrapHoldingTransaction(
  $resource: String!,
  $amount: BigInt!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateWrapHoldingTransaction(
    resource: $resource,
    amount: $amount,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateTransferResourceTransaction = """
query CreateTransferResourceTransaction(
  $resource: String!,
  $owner: String!,
  $recipient: String!,
  $amount: BigInt!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createTransferResourceTransaction(
    resource: $resource,
    owner: $owner,
    recipient: $recipient,
    amount: $amount,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateInitializeRecipeTransaction = """
query CreateInitializeRecipeTransaction(
  $project: String!,
  $xp: BigInt!,
  $ingredients: [IngredientsInput!]!,
  $meal: MealInput!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitializeRecipeTransaction(
    project: $project,
    xp: $xp,
    ingredients: $ingredients,
    meal: $meal,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions {
      transactions
      blockhash
      lastValidBlockHeight
    }
    recipe
  }
}
"""

CreateInitializeRecipeTransaction = """
query CreateInitializeRecipeTransaction(
  $project: String!,
  $xp: BigInt!,
  $ingredients: [IngredientsInput!]!,
  $meal: MealInput!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitializeRecipeTransaction(
    project: $project,
    xp: $xp,
    ingredients: $ingredients,
    meal: $meal,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions {
      transactions
      blockhash
      lastValidBlockHeight
    }
    recipe
  }
}"""

CreateAddIngredientsTransaction = """
query CreateAddIngredientsTransaction(
  $recipe: String!,
  $ingredients: [IngredientsInput!]!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createAddIngredientsTransaction(
    recipe: $recipe,
    ingredients: $ingredients,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateRemoveIngredientsTransaction = """
query CreateRemoveIngredientsTransaction(
  $recipe: String!,
  $ingredients: [String!]!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createRemoveIngredientsTransaction(
    recipe: $recipe,
    ingredients: $ingredients,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateInitCookingProcessTransactions = """
query CreateInitCookingProcessTransactions(
  $recipe: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitCookingProcessTransactions(
    recipe: $recipe,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateCreateMissionPoolTransaction = """
query CreateCreateMissionPoolTransaction(
  $data: NewMissionPoolData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateMissionPoolTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    missionPoolAddress
  }
}"""

CreateUpdateMissionPoolTransaction = """
query CreateUpdateMissionPoolTransaction(
  $data: UpdateMissionPoolData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateMissionPoolTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
  }
}"""

CreateCreateMissionTransaction = """
query CreateCreateMissionTransaction(
  $data: NewMissionData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateMissionTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    missionAddress
  }
}
"""

CreateUpdateMissionTransaction = """
query CreateUpdateMissionTransaction(
  $missionAddress: String!,
  $authority: String!,
  $params: UpdateMissionInput!,
  $payer: String,
  $delegateAuthority: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateMissionTransaction(
    missionAddress: $missionAddress,
    authority: $authority,
    params: $params,
    payer: $payer,
    delegateAuthority: $delegateAuthority,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateSendCharactersOnMissionTransaction = """
query CreateSendCharactersOnMissionTransaction(
  $data: ParticipateOnMissionData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createSendCharactersOnMissionTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateStakingPoolTransaction = """
query CreateCreateStakingPoolTransaction(
  $project: String!,
  $resource: String!,
  $metadata: CreateStakingPoolMetadataInput!,
  $authority: String!,
  $multiplier: InitStakingMultiplierMetadataInput,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateStakingPoolTransaction(
    project: $project,
    resource: $resource,
    metadata: $metadata,
    authority: $authority,
    multiplier: $multiplier,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions {
      transactions
      blockhash
      lastValidBlockHeight
    }
    stakingPoolAddress
    multipliersAddress
  }
}"""

CreateUpdateStakingPoolTransaction = """
query CreateUpdateStakingPoolTransaction(
  $project: String!,
  $stakingPool: String!,
  $metadata: UpdateStakingPoolMetadataInput,
  $authority: String!,
  $characterModel: String,
  $resource: String,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateStakingPoolTransaction(
    project: $project,
    stakingPool: $stakingPool,
    metadata: $metadata,
    authority: $authority,
    characterModel: $characterModel,
    resource: $resource,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateInitMultipliersTransaction = """
query CreateInitMultipliersTransaction(
  $project: String!,
  $stakingPool: String!,
  $decimals: Int!,
  $multipliers: [AddMultiplierMetadataInput]!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitMultipliersTransaction(
    project: $project,
    stakingPool: $stakingPool,
    decimals: $decimals,
    multipliers: $multipliers,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    multipliersAddress
  }
}"""

CreateAddMultiplierTransaction = """
query CreateAddMultiplierTransaction(
  $project: String!,
  $multiplier: String!,
  $metadata: AddMultiplierMetadataInput!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createAddMultiplierTransaction(
    project: $project,
    multiplier: $multiplier,
    metadata: $metadata,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateStakeCharactersTransactions = """
query CreateStakeCharactersTransactions(
  $characterAddresses: [String!]!,
  $project: String!,
  $characterModel: String!,
  $stakingPool: String!,
  $feePayer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createStakeCharactersTransactions(
    characterAddresses: $characterAddresses,
    project: $project,
    characterModel: $characterModel,
    stakingPool: $stakingPool,
    feePayer: $feePayer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateClaimStakingRewardsTransactions = """
query CreateClaimStakingRewardsTransactions(
  $characterAddresses: [String!]!,
  $characterModel: String!,
  $feePayer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createClaimStakingRewardsTransactions(
    characterAddresses: $characterAddresses,
    characterModel: $characterModel,
    feePayer: $feePayer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateUnstakeCharactersTransactions = """
query CreateUnstakeCharactersTransactions(
  $characterAddresses: [String!]!,
  $characterModel: String!,
  $feePayer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUnstakeCharactersTransactions(
    characterAddresses: $characterAddresses,
    characterModel: $characterModel,
    feePayer: $feePayer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateAddCharacterTraitsTransactions = """
query CreateAddCharacterTraitsTransactions(
  $assemblerConfig: String!,
  $traits: [CharacterTraitInput!]!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createAddCharacterTraitsTransactions(
    assemblerConfig: $assemblerConfig,
    traits: $traits,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateRemoveCharacterTraitsTransactions = """
query CreateRemoveCharacterTraitsTransactions(
  $assemblerConfig: String!,
  $traitsAddresses: [String!]!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createRemoveCharacterTraitsTransactions(
    assemblerConfig: $assemblerConfig,
    traitsAddresses: $traitsAddresses,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateCharacterModelTransaction = """
query CreateCreateCharacterModelTransaction(
  $config: CharacterConfigInput!,
  $attributes: VecMapGeneric,
  $project: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateCharacterModelTransaction(
    config: $config,
    attributes: $attributes,
    project: $project,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    characterModel
  }
}"""

CreateCreateCharactersTreeTransaction = """
query CreateCreateCharactersTreeTransaction(
  $treeConfig: TreeSetupConfig!,
  $project: String!,
  $characterModel: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateCharactersTreeTransaction(
    treeConfig: $treeConfig,
    project: $project,
    characterModel: $characterModel,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    treeAddress
    proofBytes
    space
    cost
    maxTreeCapacity
  }
}"""

CreateAssembleCharacterTransaction = """
query CreateAssembleCharacterTransaction(
  $attributes: VecMapGeneric!,
  $project: String!,
  $assemblerConfig: String!,
  $characterModel: String!,
  $charactersTree: String!,
  $wallet: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createAssembleCharacterTransaction(
    attributes: $attributes,
    project: $project,
    assemblerConfig: $assemblerConfig,
    characterModel: $characterModel,
    charactersTree: $charactersTree,
    wallet: $wallet,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateUpdateCharacterTraitsTransaction = """
query CreateUpdateCharacterTraitsTransaction(
  $characterAddress: String!,
  $attributes: VecMapGeneric!,
  $project: String!,
  $assemblerConfig: String!,
  $characterModel: String!,
  $charactersTree: String!,
  $wallet: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateCharacterTraitsTransaction(
    characterAddress: $characterAddress,
    attributes: $attributes,
    project: $project,
    assemblerConfig: $assemblerConfig,
    characterModel: $characterModel,
    charactersTree: $charactersTree,
    wallet: $wallet,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreatePopulateAssembleablCharacterTransaction = """
query CreatePopulateAssembleablCharacterTransaction(
  $attributes: VecMapGeneric!,
  $project: String!,
  $characterModel: String!,
  $charactersTree: String!,
  $mint: String!,
  $owner: String!,
  $updateAuthority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createPopulateAssembleablCharacterTransaction(
    attributes: $attributes,
    project: $project,
    characterModel: $characterModel,
    charactersTree: $charactersTree,
    mint: $mint,
    owner: $owner,
    updateAuthority: $updateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateWrapAssetsToCharacterTransactions = """
query CreateWrapAssetsToCharacterTransactions(
  $mintList: [String!]!,
  $project: String!,
  $characterModel: String!,
  $wallet: String!,
  $libreplexDeployment: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createWrapAssetsToCharacterTransactions(
    mintList: $mintList,
    project: $project,
    characterModel: $characterModel,
    wallet: $wallet,
    libreplexDeployment: $libreplexDeployment,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateUnwrapAssetsFromCharacterTransactions = """
query CreateUnwrapAssetsFromCharacterTransactions(
  $characterAddresses: [String!]!,
  $project: String!,
  $characterModel: String!,
  $wallet: String!,
  $libreplexDeployment: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUnwrapAssetsFromCharacterTransactions(
    characterAddresses: $characterAddresses,
    project: $project,
    characterModel: $characterModel,
    wallet: $wallet,
    libreplexDeployment: $libreplexDeployment,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateRecallCharactersTransaction = """
query CreateRecallCharactersTransaction(
  $data: RecallFromMissionData!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createRecallCharactersTransaction(
    data: $data,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

SignWithShadowSignerAndSendTransactionBundles = """
query SignWithShadowSignerAndSendTransactionBundles(
  $txs: [Bytes!]!,
  $blockhash: String!,
  $lastValidBlockHeight: Int!,
  $authToken: String!,
  $options: SendTransactionBundlesOptions
) {
  signWithShadowSignerAndSendTransactionBundles(
    txs: $txs,
    blockhash: $blockhash,
    lastValidBlockHeight: $lastValidBlockHeight,
    authToken: $authToken,
    options: $options
  ) {
    bundleId
    responses {
      signature
      error
      status
    }
  }
}"""

CreateInitializeFaucetTransaction = """
query CreateInitializeFaucetTransaction(
  $resource: String!,
  $amount: Int!,
  $repeatInterval: Int!,
  $authority: String!,
  $delegateAuthority: String,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitializeFaucetTransaction(
    resource: $resource,
    amount: $amount,
    repeatInterval: $repeatInterval,
    authority: $authority,
    delegateAuthority: $delegateAuthority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    faucet
  }
}"""

CreateClaimFaucetTransaction = """
query CreateClaimFaucetTransaction(
  $faucet: String!,
  $owner: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createClaimFaucetTransaction(
    faucet: $faucet,
    owner: $owner,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateNewUserBulkTransaction = """
query CreateNewUserBulkTransaction(
  $info: [UserInfoInput!]!,
  $wallet: [String!]!,
  $payer: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createNewUserBulkTransaction(
    info: $info,
    wallet: $wallet,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transactions
    blockhash
    lastValidBlockHeight
  }
}"""

CreateNewUserTransaction = """
query CreateNewUserTransaction(
  $info: UserInfoInput,
  $wallet: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createNewUserTransaction(
    info: $info,
    wallet: $wallet,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateUpdateUserTransaction = """
query CreateUpdateUserTransaction(
  $info: PartialUserInfoInput,
  $wallets: UpdateWalletInput,
  $populateCivic: Boolean,
  $payer: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateUserTransaction(
    info: $info,
    wallets: $wallets,
    populateCivic: $populateCivic,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateProfilesTreeTransaction = """
query CreateCreateProfilesTreeTransaction(
  $treeConfig: TreeSetupConfig!,
  $project: String!,
  $payer: String!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateProfilesTreeTransaction(
    treeConfig: $treeConfig,
    project: $project,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    treeAddress
    proofBytes
    space
    cost
    maxTreeCapacity
  }
}
"""

CreateNewProfileTransaction = """
query CreateNewProfileTransaction(
  $project: String!,
  $payer: String!,
  $identity: String,
  $info: ProfileInfoInput,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createNewProfileTransaction(
    project: $project,
    payer: $payer,
    identity: $identity,
    info: $info,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateUpdateProfileTransaction = """
query CreateUpdateProfileTransaction(
  $profile: String!,
  $payer: String!,
  $info: ProfileInfoInput,
  $customData: CustomDataInput,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateProfileTransaction(
    profile: $profile,
    payer: $payer,
    info: $info,
    customData: $customData,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateNewUserWithProfileTransaction = """
query CreateNewUserWithProfileTransaction(
  $userInfo: UserInfoInput,
  $project: String!,
  $profileIdentity: String,
  $profileInfo: ProfileInfoInput,
  $wallet: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createNewUserWithProfileTransaction(
    userInfo: $userInfo,
    project: $project,
    profileIdentity: $profileIdentity,
    profileInfo: $profileInfo,
    wallet: $wallet,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateClaimBadgeCriteriaTransaction = """
query CreateClaimBadgeCriteriaTransaction(
  $args: ClaimBadgeCriteriaInput!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createClaimBadgeCriteriaTransaction(
    args: $args,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}
"""

CreateUpdateBadgeCriteriaTransaction = """
query CreateUpdateBadgeCriteriaTransaction(
  $args: UpdateBadgeCriteriaInput!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createUpdateBadgeCriteriaTransaction(
    args: $args,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""


CreateCreateProjectTransaction = """
query CreateCreateProjectTransaction(
  $authority: String!,
  $name: String!,
  $driver: String,
  $associatedPrograms: [AssociatedProgramInput!],
  $profileDataConfig: ProfileDataConfigInput,
  $subsidizeFees: Boolean,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateProjectTransaction(
    authority: $authority,
    name: $name,
    driver: $driver,
    associatedPrograms: $associatedPrograms,
    profileDataConfig: $profileDataConfig,
    subsidizeFees: $subsidizeFees,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    project
  }
}"""

CreateChangeProjectDriverTransaction = """
query CreateChangeProjectDriverTransaction(
  $project: String!,
  $driver: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createChangeProjectDriverTransaction(
    project: $project,
    driver: $driver,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateDelegateAuthorityTransaction = """
query CreateCreateDelegateAuthorityTransaction(
  $project: String!,
  $delegate: String!,
  $serviceDelegations: ServiceDelegationInput!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateDelegateAuthorityTransaction(
    project: $project,
    delegate: $delegate,
    serviceDelegations: $serviceDelegations,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateModifyDelegationTransaction = """
query CreateModifyDelegationTransaction(
  $project: String!,
  $delegate: String!,
  $modifyDelegation: ModifyDelegationInput!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createModifyDelegationTransaction(
    project: $project,
    delegate: $delegate,
    modifyDelegation: $modifyDelegation,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""

CreateCreateAssemblerConfigTransaction = """
query CreateCreateAssemblerConfigTransaction(
  $treeConfig: TreeSetupConfig!,
  $ticker: String!,
  $order: [String!]!,
  $project: String!,
  $authority: String!,
  $payer: String,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createCreateAssemblerConfigTransaction(
    treeConfig: $treeConfig,
    ticker: $ticker,
    order: $order,
    project: $project,
    authority: $authority,
    payer: $payer,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    tx {
      transaction
      blockhash
      lastValidBlockHeight
    }
    assemblerConfig
    treeAddress
    proofBytes
    space
    cost
    maxTreeCapacity
  }
}"""

CreateInitializeBadgeCriteriaTransaction = """
query CreateInitializeBadgeCriteriaTransaction(
  $args: CreateBadgeCriteriaInput!,
  $lutAddresses: [String!],
  $computeUnitPrice: Int
) {
  createInitializeBadgeCriteriaTransaction(
    args: $args,
    lutAddresses: $lutAddresses,
    computeUnitPrice: $computeUnitPrice
  ) {
    transaction
    blockhash
    lastValidBlockHeight
  }
}"""