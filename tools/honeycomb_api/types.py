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

TreeSetupConfig = """
TreeSetupConfig
basic - BasicTreeConfig
advanced - AdvancedTreeConfig
"""

UserInfoInput = """
UserInfoInput
username - String!
name - String!
bio - String!
pfp - String!
"""

RecallFromMissionData = """
RecallFromMissionData
mission - String!
characterAddresses - [String!]!
authority - String!
payer - String
"""

CharacterConfigInput = """
CharacterConfigInput
kind - String!
criterias - [NftWrapCriteriaInput!]
assemblerConfigInput - AssemblerConfigInput
"""

AddMultiplierMetadataInput = """
AddMultiplierMetadataInput
value - BigInt!
type - MultiplierTypeInput
"""

UpdateStakingPoolMetadataInput = """
UpdateStakingPoolMetadataInput
name - String
rewardsPerDuration - BigInt
rewardsDuration - BigInt
maxRewardsDuration - BigInt
minStakeDuration - BigInt
cooldownDuration - BigInt
resetStakeDuration - Boolean
startTime - BigInt
endTime - BigInt
"""

InitStakingMultiplierMetadataInput = """
InitStakingMultiplierMetadataInput
decimals - Int!
multipliers - [AddMultiplierMetadataInput]!
"""

CreateStakingPoolMetadataInput = """
CreateStakingPoolMetadataInput
name - String!
rewardsPerDuration - BigInt!
rewardsDuration - BigInt!
maxRewardsDuration - BigInt
minStakeDuration - BigInt
cooldownDuration - BigInt
resetStakeDuration - Boolean!
startTime - BigInt!
lockType - LockTypeEnum
endTime - BigInt
"""

ParticipateOnMissionData = """
ParticipateOnMissionData
mission - String!
characterAddresses - [String!]!
authority - String!
payer - String
"""

UpdateMissionInput = """
UpdateMissionInput
cost - NewMissionCost
minXp - BigInt
duration - BigInt
removeRewards - [Int!]
newRewards - [MissionReward!]
updateRewards - [MissionReward!]
"""

NewMissionData = """
NewMissionData
project - String!
missionPool - String!
authority - String!
delegateAuthority - String
payer - String!
name - String!
minXp - BigInt!
cost - NewMissionCost!
duration - BigInt!
rewards - [MissionReward]!
"""

UpdateMissionPoolData = """
UpdateMissionPoolData
project - String!
missionPool - String!
characterModel - String
authority - String!
delegateAuthority - String
payer - String!
"""

NewMissionPoolData = """
NewMissionPoolData
project - String!
authority - String!
delegateAuthority - String
payer - String!
characterModel - String!
name - String!
"""

MealInput = """
MealInput
resourceAddress - String!
amount - BigInt!
"""

InitResourceInput = """
InitResourceInput
name - String!
symbol - String!
uri - String!
decimals - Int!
storage - ResourceStorageEnum!
"""