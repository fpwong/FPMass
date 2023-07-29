// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FPISMMisc.generated.h"

class UAnimToTextureDataAsset;
struct FAnimToTextureAnimInfo;

USTRUCT(BlueprintType)
struct FPMASS_API FFPISMAnimation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimToTextureDataAsset> AnimData;

	UPROPERTY(EditAnywhere)
	int AnimationIndex = 2; // assume 0 is idle and 1 is walk

	bool IsValid() const;

	const FAnimToTextureAnimInfo& GetAnimInfo() const;
};
