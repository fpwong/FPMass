// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"

#include "FPISMRepresentationTrait.generated.h"

class AFPISMActor;

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMAnimationState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Default)
	int32 StartFrame = 0;

	UPROPERTY(VisibleAnywhere, Category = Default)
	int32 NumFrames = 0;

	UPROPERTY(VisibleAnywhere, Category = Default)
	float CurrentFrame = 0;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMAnimationFragment : public FMassFragment
{
	GENERATED_BODY()

	float WalkBlend = 0;

	float MontageBlend = 0;

	TOptional<FFPISMAnimationState> CurrentMontage;

	TArray<float> AsCustomData() const;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMInstanceIdFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int InstanceId;

	UPROPERTY()
	FTransform PrevTransform;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMSharedFragment : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditAnywhere)
	FTransform RelativeTransform;
};

UCLASS(meta = (DisplayName = "FP ISM Representation Trait"))
class FPMASS_API UFPISMRepresentationTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(EditAnywhere)
	FFPISMSharedFragment StaticMeshSharedFragment;
	// TSoftObjectPtr<UStaticMesh> StaticMesh;
};
