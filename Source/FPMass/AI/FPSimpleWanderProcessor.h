#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "NavigationSystem.h"
#include "FPSimpleWanderProcessor.generated.h"

USTRUCT()
struct FFPMassTag_SimpleWander : public FMassTag
{
	GENERATED_BODY()
};

UCLASS()
class FPMASS_API UFPSimpleWanderProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPSimpleWanderProcessor();

	virtual void ConfigureQueries() override;
	virtual void Initialize(UObject& Owner) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	UPROPERTY()
	TObjectPtr<UNavigationSystemV1> NavigationSystem;

	FMassEntityQuery EntityQuery;
	FMassEntityQuery PushArraysToNiagaraSystems;
};
