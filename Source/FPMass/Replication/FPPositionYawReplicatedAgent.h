#pragma once

#include "MassClientBubbleHandler.h"
#include "MassReplicationTransformHandlers.h"
#include "MassReplicationTypes.h"
#include "FPPositionYawReplicatedAgent.generated.h"

/** The data that is replicated specific to each Crowd agent */
USTRUCT()
struct FPMASS_API FFPReplicatedAgent_PositionYaw : public FReplicatedAgentBase
{
	GENERATED_BODY()

	const FReplicatedAgentPositionYawData& GetReplicatedPositionYawData() const { return PositionYaw; }
	FReplicatedAgentPositionYawData& GetReplicatedPositionYawDataMutable() { return PositionYaw; }

private:
	UPROPERTY()
	FReplicatedAgentPositionYawData PositionYaw; // replicated data
};

/** Fast array item for efficient agent replication. Remember to make this dirty if any FReplicatedCrowdAgent member variables are modified */
USTRUCT()
struct FPMASS_API FFPMassFastArrayItem_PositionYaw : public FMassFastArrayItemBase
{
	GENERATED_BODY()

	FFPMassFastArrayItem_PositionYaw() = default;

	FFPMassFastArrayItem_PositionYaw(const FFPReplicatedAgent_PositionYaw& InAgent, const FMassReplicatedAgentHandle InHandle)
		: FMassFastArrayItemBase(InHandle)
		, Agent(InAgent) {}

	/** This typedef is required to be provided in FMassFastArrayItemBase derived classes (with the associated FReplicatedAgentBase derived class) */
	using FReplicatedAgentType = FFPReplicatedAgent_PositionYaw;

	UPROPERTY()
	FFPReplicatedAgent_PositionYaw Agent;
};
