#pragma once
#include "CoreMinimal.h"
#include "FPISMRepresentationFragments.h"
#include "MassRepresentationProcessor.h"
#include "FPISMSubsystem.generated.h"

class AFPISMActor;

USTRUCT()
struct FPMASS_API FFPISMData
{
	GENERATED_BODY()

	FFPISMData() = default;

	/** Buffer holding current frame transforms for the static mesh instances, used to batch update the transforms */
	TArray<int32> UpdateInstanceIds;
	TArray<FTransform> StaticMeshInstanceTransforms;
	TArray<FTransform> StaticMeshInstancePrevTransforms;

	/** Buffer holding current frame custom floats for the static mesh instances, used to batch update the ISMs custom data */
	TArray<float> StaticMeshInstanceCustomFloats;

	// When initially adding to StaticMeshInstanceCustomFloats, can use the size as the write iterator, but on subsequent processors, we need to know where to start writing
	int32 WriteIterator = 0;
};

UCLASS()
class FPMASS_API UFPISMSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
protected:

	TSharedPtr<FMassEntityManager> MassManager;

	FMassArchetypeHandle BulletArchetype;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override
	{
		MassManager.Reset();
	};

public:
	AFPISMActor* FindOrCreateISM(UStaticMesh* StaticMesh);
	AFPISMActor* FindOrCreateISM(const FFPISMDescription& MeshDesc);

	void OnProcessingPhaseStarted(const float DeltaSeconds, const EMassProcessingPhase Phase) const;

	void BeginVisualChanges() const;

	void EndVisualChanges() const;

	UPROPERTY()
	TMap<uint32, AFPISMActor*> ISMActors;
};
