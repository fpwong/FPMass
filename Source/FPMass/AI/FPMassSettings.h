// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "UObject/Object.h"
#include "FPMassSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class FPMASS_API UFPMassSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere)
	FGameplayAttribute AttackRangeAttribute;

	static UFPMassSettings& Get()
	{
		return *GetMutableDefault<UFPMassSettings>();
	}
};
