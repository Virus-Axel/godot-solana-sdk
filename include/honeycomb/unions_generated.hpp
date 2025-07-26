using CharacterConfig = std::variant<
		CharacterConfigWrapped,
		CharacterConfigAssembled>;

using MintAsParams = std::variant<
		MintAsParamsMplBubblegum>;

using CharacterSourceParams = std::variant<
		Wrapped,
		Assembled>;

using CharacterUsedByParams = std::variant<
		UsedByStaking,
		UsedByMission,
		UsedByGuild,
		UsedByEjected,
		UsedByCustom>;

using ServiceParams = std::variant<
		ServiceParamsAssembler,
		ServiceParamsAssetManager,
		ServiceParamsStaking,
		ServiceParamsMissions,
		ServiceParamsRaffles,
		ServiceParamsGuildKit>;

using MissionRequirement = std::variant<
		TimeRequirement>;

using RewardType = std::variant<
		XpRewardType,
		ResourceRewardType>;

using MultiplierTypeParams = std::variant<
		MultiplierTypeParamsStakeDuration,
		MultiplierTypeParamsNFTCount,
		MultiplierTypeParamsCreator,
		MultiplierTypeParamsCollection>;

using SplMultiplierTypeParams = std::variant<
		SplMultiplierTypeParamsStakeDuration,
		SplMultiplierTypeParamsStakeAmount>;

using SplRewardConfigParams = std::variant<
		SplRewardConfigParamsApy,
		SplRewardConfigParamsStakeWeight,
		SplRewardConfigParamsNotSet>;

using StakerClaimedParams = std::variant<
		StakerClaimedParamsApy,
		StakerClaimedParamsStakeWeight,
		StakerClaimedParamsNone>;

using OutputHolding = std::variant<
		Holding,
		OutputHoldingTree>;

using ResourceKindParams = std::variant<
		ResourceKindParamsHplFungible,
		ResourceKindParamsWrappedFungible,
		ResourceKindParamsHplNonFungible,
		ResourceKindParamsWrappedMplCore>;
