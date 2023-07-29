// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "FPSimpleVelocityOrientationProcessor.generated.h"

USTRUCT()
struct FFPMassTag_VelocityOrientation : public FMassTag
{
	GENERATED_BODY()
};

UCLASS()
class FPMASS_API UFPSimpleVelocityOrientationProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPSimpleVelocityOrientationProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};
