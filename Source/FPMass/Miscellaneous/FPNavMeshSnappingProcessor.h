// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "NavigationSystem.h"
#include "FPNavMeshSnappingProcessor.generated.h"

/*
 * This is terrible method of having the entities avoid walls
 */
UCLASS()
class FPMASS_API UFPNavMeshSnappingProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPNavMeshSnappingProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Initialize(UObject& Owner) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	UPROPERTY()
	TObjectPtr<UNavigationSystemV1> NavigationSystem;

	FMassEntityQuery EntityQuery;
};
