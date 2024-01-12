// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FPMassBlueprintLibrary.generated.h"

class UAbilitySystemComponent;
struct FMSEntityViewBPWrapper;
struct FFPISMRepresentationFragment;
struct FFPISMDescription;

/**
 *
 */
UCLASS()
class FPMASS_API UFPMassBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "FPMass", meta=(DisplayName="PlayISMAnimation"))
	static void PlayISMAnimation(FMSEntityViewBPWrapper EntityHandle, int AnimIndex, FFPISMAnimationCallbacks AnimationCallbacks);

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static UAbilitySystemComponent* GetAbilitySystemFromEntity(const FMSEntityViewBPWrapper EntityHandle);

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static bool SetEntityISMScale(const FMSEntityViewBPWrapper EntityHandle, FVector Scale = FVector(1, 1, 1));

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static bool ScaleUpEntity(const FMSEntityViewBPWrapper EntityHandle, FVector Scale = FVector(1, 1, 1));

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static bool ScaleDownEntity(const FMSEntityViewBPWrapper EntityHandle, FVector Scale = FVector(1, 1, 1));

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static FVector GetEntityISMScale(const FMSEntityViewBPWrapper EntityHandle);

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static FGuid AddEntityISMLayer(const FMSEntityViewBPWrapper EntityHandle, FGameplayTag LayerTag, FFPISMDescription Description);

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static bool RemoveEntityISMLayer(const FMSEntityViewBPWrapper EntityHandle, FGameplayTag LayerTag, FGuid Guid);

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static FFPISMRepresentationFragment GetEntityISMRepresentation(const FMSEntityViewBPWrapper EntityHandle);

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static bool SetEntityMaxSpeed(const FMSEntityViewBPWrapper EntityHandle, float NewSpeed);
};
