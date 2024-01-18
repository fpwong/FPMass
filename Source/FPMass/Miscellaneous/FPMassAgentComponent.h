// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassAgentComponent.h"
#include "FPMassAgentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFPOnMassAgentInitialized);

UCLASS(ClassGroup=(FPMass), meta=(BlueprintSpawnableComponent))
class FPMASS_API UFPMassAgentComponent : public UMassAgentComponent
{
	GENERATED_BODY()

public:
	UFPMassAgentComponent();

	UPROPERTY(BlueprintAssignable)
	FFPOnMassAgentInitialized OnMassAgentInitialized;

	UPROPERTY(EditDefaultsOnly)
	UMassEntityConfigAsset* MassConfigAsset = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetMassConfig(UMassEntityConfigAsset* Config) { MassConfigAsset = Config; }

	UFUNCTION(BlueprintCallable)
	bool IsAgentValid();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnRegister() override;

protected:
	bool bWaitingInitialized = true;
};
