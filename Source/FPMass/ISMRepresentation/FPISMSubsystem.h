#pragma once
#include "CoreMinimal.h"
#include "MassRepresentationProcessor.h"
#include "FPISMSubsystem.generated.h"

class AFPISMActor;

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

	void OnProcessingPhaseStarted(const float DeltaSeconds, const EMassProcessingPhase Phase) const;

	void BeginVisualChanges() const;

	void EndVisualChanges() const;

	UPROPERTY()
	TMap<uint32, AFPISMActor*> ISMActors;
};
