#pragma once

#include "MassReplicationProcessor.h"

#include "FPReplicator.generated.h"

/** Class that handles replication and only runs on the server. It queries Mass entity fragments and sets those values when appropriate using the MassClientBubbleHandler. */
UCLASS()
class FPMASS_API UFPMassReplicator : public UMassReplicatorBase
{
	GENERATED_BODY()

public:
	/**
	* Overridden to add specific entity query requirements for replication.
	* Usually we add replication processor handler requirements.
	*/
	virtual void AddRequirements(FMassEntityQuery& EntityQuery) override;

	/**
	* Overridden to process the client replication.
	* This methods should call CalculateClientReplication with the appropriate callback implementation.
	*/
	virtual void ProcessClientReplication(FMassExecutionContext& Context, FMassReplicationContext& ReplicationContext) override;
};
