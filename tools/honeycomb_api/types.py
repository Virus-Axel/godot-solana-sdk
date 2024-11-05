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