// Fill out your copyright notice in the Description page of Project Settings.

#include "FPMassActor.h"

#include "FPMassAgentComponent.h"

// Sets default values
AFPMassActor::AFPMassActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MassAgentComponent = CreateDefaultSubobject<UFPMassAgentComponent>("FPMassAgentComponent");
}

void AFPMassActor::PreRegisterAllComponents()
{
	Super::PreRegisterAllComponents();

	check(MassAgentComponent);
	MassAgentComponent->SetMassConfig(MassEntityConfigAsset);
}

// Called when the game starts or when spawned
void AFPMassActor::BeginPlay()
{
	Super::BeginPlay();
}

