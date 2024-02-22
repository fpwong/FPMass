// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSimpleEnemyProcessor.h"
#include "MassEntityTraitBase.h"
#include "FPSimpleEnemyTrait.generated.h"

UCLASS()
class FPMASS_API UFPSimpleEnemyTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	// UPROPERTY(EditAnywhere)
	// FFPSimpleEnemyParameters SimpleEnemyParameters;
};
