// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPISMRepresentationFragments.h"
#include "MassEntityTraitBase.h"
#include "FPISMRepresentationTrait.generated.h"

UCLASS(meta = (DisplayName = "FP ISM Representation Trait"))
class FPMASS_API UFPISMRepresentationTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(EditAnywhere)
	FFPISMParameters ISMSharedParameters;
};
