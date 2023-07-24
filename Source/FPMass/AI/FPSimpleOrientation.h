// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "FPSimpleOrientation.generated.h"

UCLASS()
class FPMASS_API UFPSimpleOrientationProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UFPSimpleOrientationProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	FMassEntityQuery EntityQuery;
};
