// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPISMSubsystem.h"
#include "FPISMActor.generated.h"

UCLASS()
class FPMASS_API AFPISMActor : public AActor
{
	GENERATED_BODY()

public:
	AFPISMActor();

	UPROPERTY()
	UInstancedStaticMeshComponent* InstancedStaticMesh;

	FFPISMData SharedData;
};
