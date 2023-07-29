// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FPMassReplicationSettings.generated.h"

/**
 *
 */
UCLASS(Config=Game, defaultconfig)
class FPMASS_API UFPMassReplicationSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	TArray<TSoftObjectPtr<UMassEntityConfigAsset>> PreloadedEntityConfigs;
};
