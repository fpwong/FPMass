#pragma once
#include "MassSpawnerSubsystem.h"
#include "MassSpawnLocationProcessor.h"
#include "FPEntitySystem.generated.h"

class UMassEntityConfigAsset;

UCLASS()
class UFPEntitySystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TestReplication")
	void ReplicatedSpawnEntity(UMassEntityConfigAsset* EntityConfig, const FTransform& Transform, int Count = 1);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void PostInitialize() override;

private:
	TSharedPtr<FMassEntityManager> EntityManager;
};
