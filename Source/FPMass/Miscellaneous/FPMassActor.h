// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPMassActor.generated.h"

class UMassEntityConfigAsset;
class UFPMassAgentComponent;

UCLASS()
class FPMASS_API AFPMassActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFPMassActor();

protected:
	virtual void PreRegisterAllComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFPMassAgentComponent* MassAgentComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	UMassEntityConfigAsset* MassEntityConfigAsset = nullptr;
};
