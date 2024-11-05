Transaction = """
Transaction
transaction - Bytes!
blockhash - String!
lastValidBlockHeight - Int!
"""

SendTransactionBundlesOptions = """
SendTransactionBundlesOptions
commitment - String
skipPreflight - Boolean
bundles - TransactionBundlesOptions
"""

CreateBadgeCriteriaInput = """
CreateBadgeCriteriaInput
projectAddress - Pubkey!
authority - Pubkey!
payer - Pubkey
startTime - Int
endTime - Int
badgeIndex - Int!
condition - BadgesCondition!
"""

ModifyDelegationInput = """
ModifyDelegationInput
delegation - ModifyServiceDelegationInput
index - Int
"""

ServiceDelegationInput = """
ServiceDelegationInput
HiveControl - [ServiceDelegationHiveControl!]
AssetAssembler - [ServiceDelegationAssetAssembler!]
AssetManager - [ServiceDelegationAssetManager!]
CurrencyManager - [ServiceDelegationCurrencyManager!]
NectarStaking - [ServiceDelegationNectarStaking!]
NectarMissions - [ServiceDelegationNectarMissions!]
BuzzGuild - [ServiceDelegationBuzzGuild!]
"""

ProfileDataConfigInput = """
ProfileDataConfigInput
achievements - [String!]!
customDataFields - [String!]!
"""

UpdateBadgeCriteriaInput = """
UpdateBadgeCriteriaInput
projectAddress - Pubkey!
authority - Pubkey!
criteriaIndex - Int!
payer - Pubkey
startTime - Int
endTime - Int
condition - BadgesCondition!
"""

ClaimBadgeCriteriaInput = """
ClaimBadgeCriteriaInput
projectAddress - Pubkey!
profileAddress - Pubkey!
payer - Pubkey!
criteriaIndex - Int!
proof - BadgesCondition!
"""

ProfileInfoInput = """
ProfileInfoInput
name - String
bio - String
pfp - String
"""

CustomDataInput = """
CustomDataInput
add - JSON
remove - [String!]
"""

UpdateWalletInput = """
UpdateWalletInput
add - [String]
remove - [String]
"""

PartialUserInfoInput = """
PartialUserInfoInput
username - String
name - String
bio - String
pfp - String
"""