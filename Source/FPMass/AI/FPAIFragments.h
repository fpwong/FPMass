// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "FPAIFragments.generated.h"

USTRUCT(BlueprintType)
struct FPMASS_API FFPTargetDataFragment : public FMassFragment
{
	GENERATED_BODY()

	FGameplayAbilityTargetDataHandle TargetData;
};
