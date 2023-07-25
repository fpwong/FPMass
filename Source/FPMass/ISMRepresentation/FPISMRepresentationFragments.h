// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "FPVertexAnimationTextures/FPAnimToTextureDataAsset.h"
#include "FPISMRepresentationFragments.generated.h"

class AFPISMActor;
class UAnimToTextureDataAsset;

DECLARE_DYNAMIC_DELEGATE_OneParam(FFPOnISMAnimNotify, const FString&, Id);
DECLARE_DYNAMIC_DELEGATE(FFPOnISMAnimEnded);

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMAnimationCallbacks
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFPOnISMAnimNotify OnAnimNotify;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFPOnISMAnimEnded OnAnimEnded;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMAnimationState
{
	GENERATED_BODY()

	uint8 AnimIndex = 0;

	UPROPERTY(VisibleAnywhere, Category = Default)
	int32 StartFrame = 0;

	UPROPERTY(VisibleAnywhere, Category = Default)
	int32 NumFrames = 0;

	UPROPERTY(VisibleAnywhere, Category = Default)
	float CurrentFrame = 0;

	UPROPERTY()
	FFPISMAnimationCallbacks AnimationCallbacks;
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
struct FPMASS_API FFPISMAnimationNotifyEventFragment : public FMassFragment
{
	GENERATED_BODY()

	FFPISMAnimationNotifyEventFragment() = default;
	FFPISMAnimationNotifyEventFragment(const FFPAnimNotifyEvent& InEvent) : Event(InEvent) {}
	FFPAnimNotifyEvent Event;

	FFPOnISMAnimNotify OnAnimNotify;
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
struct FPMASS_API FFPISMParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimToTextureDataAsset* AnimToTextureData;

	UPROPERTY(EditAnywhere)
	FTransform RelativeTransform;
};
