// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FPMassBlueprintLibrary.generated.h"

class UAbilitySystemComponent;
struct FMSEntityViewBPWrapper;
struct FFPISMRepresentationFragment;
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
	static bool SetEntityISMRepresentation(const FMSEntityViewBPWrapper EntityHandle, FFPISMRepresentationFragment Representation);

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static FFPISMRepresentationFragment GetEntityISMRepresentation(const FMSEntityViewBPWrapper EntityHandle);

	UFUNCTION(BlueprintCallable, Category = "FPMass")
	static bool SetEntityMaxSpeed(const FMSEntityViewBPWrapper EntityHandle, float NewSpeed);
};
