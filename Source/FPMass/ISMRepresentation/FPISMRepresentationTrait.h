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
	UFPISMRepresentationTrait()
	{
		RelativeTransform = FTransform(FRotator(0, -90, 0), FVector(0, 0, -100));
	}

	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	// UPROPERTY(EditAnywhere)
	// FFPISMParameters ISMSharedParameters;

	UPROPERTY(EditAnywhere)
	FFPISMRepresentationFragment Representation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFPISMDescription ISMDescription;

// #if WITH_EDITORONLY_DATA
// 	virtual void Serialize(FArchive& Ar) override;
// #endif
};
