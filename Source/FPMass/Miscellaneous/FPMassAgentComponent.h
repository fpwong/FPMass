// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassAgentComponent.h"
#include "FPMassAgentComponent.generated.h"

UCLASS(ClassGroup=(FPMass), meta=(BlueprintSpawnableComponent))
class FPMASS_API UFPMassAgentComponent : public UMassAgentComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	UMassEntityConfigAsset* MassConfigAsset = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetMassConfig(UMassEntityConfigAsset* Config) { MassConfigAsset = Config; }

	virtual void OnRegister() override;
};
