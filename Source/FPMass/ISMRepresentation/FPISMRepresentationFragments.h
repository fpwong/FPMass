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

	UPROPERTY(VisibleAnywhere, Category = Default)
	float AnimSpeed = 1.0f;

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
struct FPMASS_API FFPISMStateFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	int InstanceId = INDEX_NONE;

	UPROPERTY()
	FTransform PrevTransform;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimToTextureDataAsset* AnimToTextureData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMaterialInterface>> MaterialOverrides;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform;

	UPROPERTY(Transient)
	FGuid Guid;

	FFPISMDescription()
	{
		// RelativeTransform = FTransform(FRotator(0, -90, 0), FVector(0, 0, -100));
		RelativeTransform = FTransform::Identity;
		Guid = FGuid::NewGuid();
	}

	bool operator==(const FFPISMDescription& Other) const
	{
		return MaterialOverrides == Other.MaterialOverrides && AnimToTextureData == Other.AnimToTextureData;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FFPISMDescription& Desc)
	{
		uint32 Hash = 0x0;
		Hash = PointerHash(Desc.AnimToTextureData, Hash);
		for (UMaterialInterface* MaterialOverride : Desc.MaterialOverrides)
		{
			if (MaterialOverride)
			{
				Hash = PointerHash(MaterialOverride, Hash);
			}
		}
		return Hash;
	}
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMDescriptionLayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FFPISMDescription Description;

	UPROPERTY()
	FGuid Guid;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMDescriptionLayers
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FFPISMDescriptionLayer> Layers;
};

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMRepresentationFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RelativeTransform;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// FFPISMDescription ISMDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FFPISMDescriptionLayers> Layers;

	void ForEachActiveISMDescription(TFunctionRef<void(const FFPISMDescription& Description)> Func) const;

	FGuid AddLayer(const FGameplayTag& LayerTag, const FFPISMDescription& Description);

	FFPISMRepresentationFragment()
	{
		RelativeTransform = FTransform(FRotator(0, -90, 0), FVector(0, 0, -100));
	}
};

// USTRUCT(BlueprintType)
// struct FPMASS_API FFPISMParameters : public FMassSharedFragment
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere)
// 	FFPISMDescription ISMDescription;
//
// 	UPROPERTY(EditAnywhere)
// 	FTransform RelativeTransform;
// };

