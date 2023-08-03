// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "FPProjectileProcessors.generated.h"

UCLASS()
class FPMASS_API UFPProjectileProcessors : public UMassProcessor
{
	GENERATED_BODY()

	UFPProjectileProcessors();

	virtual void ConfigureQueries() override;

	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery LineTraceFromPreviousPosition;
};
