#pragma once

#include "MassReplicationTransformHandlers.h"
#include "MassClientBubbleHandler.h"
#include "MassClientBubbleInfoBase.h"
#include "MassEntityView.h"
#include "FPPositionYawReplicatedAgent.h"

#include "FPBubbleInfo.generated.h"

class FPMASS_API FFPUnitClientBubbleHandler final : public TClientBubbleHandlerBase<FFPMassFastArrayItem_PositionYaw>
{
public:
	typedef TClientBubbleHandlerBase<FFPMassFastArrayItem_PositionYaw> Super;
	typedef TMassClientBubbleTransformHandler<FFPMassFastArrayItem_PositionYaw> FMassClientBubbleTransformHandler;

	FFPUnitClientBubbleHandler()
		: TransformHandler(*this) {}

#if UE_REPLICATION_COMPILE_SERVER_CODE
	const FMassClientBubbleTransformHandler& GetTransformHandler() const { return TransformHandler; }
	FMassClientBubbleTransformHandler& GetTransformHandlerMutable() { return TransformHandler; }
#endif // UE_REPLICATION_COMPILE_SERVER_CODE

protected:
#if UE_REPLICATION_COMPILE_CLIENT_CODE
	virtual void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize) override;
	virtual void PostReplicatedChange(const TArrayView<int32> ChAMSedIndices, int32 FinalSize) override;

	void PostReplicatedChangeEntity(const FMassEntityView& EntityView, const FFPReplicatedAgent_PositionYaw& Item) const;
#endif //UE_REPLICATION_COMPILE_CLIENT_CODE

	FMassClientBubbleTransformHandler TransformHandler;
};

/** Mass client bubble, there will be one of these per client and it will handle replicating the fast array of Agents between the server and clients */
USTRUCT()
struct FPMASS_API FFPUnitClientBubbleSerializer : public FMassClientBubbleSerializerBase
{
	GENERATED_BODY()

	FFPUnitClientBubbleSerializer()
	{
		Bubble.Initialize(Units, *this);
	};

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FFPMassFastArrayItem_PositionYaw, FFPUnitClientBubbleSerializer>(Units, DeltaParams, *this);
	}

public:
	FFPUnitClientBubbleHandler Bubble;

protected:
	/** Fast Array of Agents for efficient replication. Maintained as a freelist on the server, to keep index consistency as indexes are used as Handles into the Array
	*  Note array order is not guaranteed between server and client so handles will not be consistent between them, FMassNetworkID will be.
	*/
	UPROPERTY(Transient)
	TArray<FFPMassFastArrayItem_PositionYaw> Units;
};

template<>
struct TStructOpsTypeTraits<FFPUnitClientBubbleSerializer> : public TStructOpsTypeTraitsBase2<FFPUnitClientBubbleSerializer>
{
	enum
	{
		WithNetDeltaSerializer = true,
		WithCopy = false,
	};
};

/**
 *  This class will allow us to replicate Mass data based on the fidelity required for each player controller. There is one AMassReplicationActor per PlayerController and
 *  which is also its owner.
 */
UCLASS()
class FPMASS_API AFPUnitClientBubbleInfo : public AMassClientBubbleInfoBase
{
	GENERATED_BODY()

public:
	AFPUnitClientBubbleInfo(const FObjectInitializer& ObjectInitializer);

	FFPUnitClientBubbleSerializer& GetUnitSerializer() { return UnitSerializer; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated, Transient)
	FFPUnitClientBubbleSerializer UnitSerializer;
};
